/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define PI 3.1415926536
#define M_SQRT1_2 0.70710678118654752440
#define MAX_NUM_BITS 2048
//#define DEBUG
//#define STATIC

typedef struct bitSet
{
    unsigned int bits[MAX_NUM_BITS];
    bool is_frozen[MAX_NUM_BITS];
    unsigned int length;
} bitSet;

typedef struct binaryProb
{
    double prob0;
    double prob1;
    double llr;
} binaryProb;

typedef struct probSet
{
    binaryProb probabilities[MAX_NUM_BITS];
    unsigned int length;
} probSet;

typedef struct dataSet
{
    double data[MAX_NUM_BITS];
    bool is_frozen[MAX_NUM_BITS];
    binaryProb probabilities[MAX_NUM_BITS];
    unsigned int length;
} dataSet;

// AWGN generator from https://www.embeddedrelated.com/showcode/311.php
double AWGN_generator()
{/* Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1. */
 
  double temp1;
  double temp2;
  double result;
  int p;

  p = 1;

  while( p > 0 )
  {
	temp2 = ( rand() / ( (double)RAND_MAX ) ); /*  rand() function generates an
                                                       integer between 0 and  RAND_MAX,
                                                       which is defined in stdlib.h.
                                                   */

    if ( temp2 == 0 )
    {// temp2 is >= (RAND_MAX / 2)
      p = 1;
    }// end if
    else
    {// temp2 is < (RAND_MAX / 2)
       p = -1;
    }// end else

  }// end while()

  temp1 = cos( ( 2.0 * (double)PI ) * rand() / ( (double)RAND_MAX ) );
  result = sqrt( -2.0 * log( temp2 ) ) * temp1;

  return result;	// return the generated random sample to the caller

}// end AWGN_generator()

double snrToVariance(double snr_db)
{
    double snr_bel = (double)(-snr_db/10.0);
    double variance = pow(10, snr_bel);
#ifdef DEBUG    
    printf("variance is: %g\n", variance);
#endif
    return variance;
}

// wrapper for AWGN_generator
double awgnSnrDb(double snr_db)
{
    double awgn_sample = AWGN_generator();
    double variance = snrToVariance(snr_db);
#ifdef DEBUG    
    printf("sigma is: %g\n", sqrt(variance));
#endif
    return sqrt(variance) * awgn_sample;
}

void initBitSet(unsigned int * bits, unsigned int length, bitSet * bit_set)
{
    for (int i = 0; i < length; i++)
    {
        bit_set->bits[i] = bits[i];
    }
    bit_set->length = length;
}

void initUBits(bitSet * data_bits, bitSet * frozen_bits, bitSet * U_bits)
{
    int frozen_bit_idx = 0;
    int data_bit_idx = 0;
    int total_length = data_bits->length + frozen_bits -> length;
    for (int i = 0; i < total_length; i++)
    {
        U_bits->bits[i] = 1; // initialize everything to 1, so our frozen bits will stand out
        U_bits->is_frozen[i] = false;
    }
    for (int i = 0; i < frozen_bits -> length; i++)
    {
        /* 
           frozen bits are set to 0 while everything else is still set to 1,
           so when we go to add our data bits, we'll know which bits are already frozen 
        */
        frozen_bit_idx = frozen_bits->bits[i];
        U_bits->bits[frozen_bit_idx] = 0; 
        U_bits->is_frozen[frozen_bit_idx] = true;
    }
    for (int i = 0; i < total_length; i++)
    {
        if (U_bits->bits[i] != 0)
        {
            U_bits->bits[i] = data_bits->bits[data_bit_idx];
            data_bit_idx++;
        }
    }
    U_bits->length = total_length;
}


void encodePolarDataV1(bitSet * data_bits, bitSet * frozen_bits, bitSet * encoded_bits)
{
    //unsigned int U_bits[MAX_NUM_BITS];
    initUBits(data_bits, frozen_bits, encoded_bits);
    int U_iter = 0;
    int U_pair_iter = 0;
    for (int i = 1; i <= encoded_bits->length >> 1; i <<= 1)
    {
        U_iter = 0;
        U_pair_iter = 0;
        while (U_iter < encoded_bits->length)
        {
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++)
            {
#ifdef DEBUG
                printf("XORing index %d into index %d\n", U_pair_iter + i, U_pair_iter);
#endif
                encoded_bits->bits[U_pair_iter] ^= encoded_bits->bits[U_pair_iter+i];   
            }            
            U_iter = U_pair_iter + i;
        }
    }
}

void encodePolarDataV2(bitSet * data_bits, bitSet * frozen_bits, bitSet * encoded_bits)
{
    //unsigned int U_bits[MAX_NUM_BITS];
    initUBits(data_bits, frozen_bits, encoded_bits);
    int U_pair_iter = 0;
    int U_iter_inc = 0;
    int num_iter = 0;
    for (int i = 1; i <= encoded_bits->length >> 1; i <<= 1)
    {
        U_pair_iter = 0;
        U_iter_inc = i << 1;
        for (int U_iter = 0; U_iter < encoded_bits->length; U_iter += U_iter_inc)
        {
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++)
            {
#ifdef DEBUG                
                printf("XORing index %d into index %d\n", U_pair_iter + i, U_pair_iter);
#endif                
                encoded_bits->bits[U_pair_iter] ^= encoded_bits->bits[U_pair_iter+i];   
                num_iter++;
            }            
        }
    }
#ifdef DEBUG
    printf("NUM ITERATIONS: %d\n", num_iter);
#endif
}

void encodePolarData(bitSet * U_bits, bitSet * encoded_bits)
{
    //unsigned int U_bits[MAX_NUM_BITS];
    
    encoded_bits->length = U_bits->length;
    for (int i = 0; i < U_bits->length; i++)
    {
        encoded_bits->bits[i] = U_bits->bits[i];
        encoded_bits->is_frozen[i] = U_bits->is_frozen[i];
    }
    
    int U_pair_iter = 0;
    int U_iter_inc = 0;
    int num_iter = 0;
    for (int i = 1; i <= encoded_bits->length >> 1; i <<= 1)
    {
        U_pair_iter = 0;
        U_iter_inc = i << 1;
        for (int U_iter = 0; U_iter < encoded_bits->length; U_iter += U_iter_inc)
        {
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++)
            {
#ifdef DEBUG                
                printf("XORing index %d into index %d\n", U_pair_iter + i, U_pair_iter);
#endif                
                encoded_bits->bits[U_pair_iter] ^= encoded_bits->bits[U_pair_iter+i];   
                num_iter++;
            }            
        }
    }
#ifdef DEBUG
    printf("NUM ITERATIONS: %d\n", num_iter);
#endif
}

void transmitBitsOverAwgn(double snr_db, bitSet * bits_x, dataSet * data_y)
{
    double variance = snrToVariance(snr_db);
    double pdf_const = 1.0 / sqrt(2.0*PI*variance);
    double std_dev = sqrt(variance);
#ifdef DEBUG     
    printf("std_dev is: %g\n", std_dev);
#endif
    double awgn_sample;
    double prob0;
    double prob1;    
    
    data_y->length = bits_x->length;
    
    
    for (int i = 0; i < bits_x->length; i++)
    {
#ifdef STATIC
        awgn_sample = std_dev / 2.0;
#else
        awgn_sample = AWGN_generator() * std_dev;
#endif
        if (bits_x->bits[i] == 0)
        {
            data_y->data[i] = -1.0 + awgn_sample;
        }
        else
        {
            data_y->data[i] = 1.0 + awgn_sample;
        }
        //prob0 = pdf_const*exp(-(double)(pow(data_y->data[i] + 1.0, 2)) / (2.0*variance));
        //prob1 = pdf_const*exp(-(double)(pow(data_y->data[i] - 1.0, 2)) / (2.0*variance));
        prob0 = 0.5 * erfc(data_y->data[i] * M_SQRT1_2 * (1.0 / std_dev));
        prob1 = 1 - prob0;
        data_y->probabilities[i].prob0 = prob0;
        data_y->probabilities[i].prob1 = prob1;
        data_y->probabilities[i].llr = log(prob0 / prob1);
#ifdef DEBUG        
        printf("\nbit[%d] = %d, gets noise %g to become %g, prob0 = %g, prob1 = %g\n", 
                i, 
                bits_x->bits[i],
                awgn_sample, 
                data_y->data[i],
                prob0,
                prob1);
#endif
        data_y->is_frozen[i] = bits_x->is_frozen[i];
    }
}

void decodePolarData(dataSet * rx_data, bitSet * decoded_bits)
{
    int U_pair_iter = 0;
    int U_iter_inc = 0;
    int num_iter = 0;
    
    double p0;
    double p1;
    double q0;
    double q1;
    double normalizer;
    
    decoded_bits->length = rx_data->length;
    
    for (int i = rx_data->length >> 1; i >= 1; i >>= 1)
    {
        U_pair_iter = 0;
        U_iter_inc = i << 1;
        for (int U_iter = 0; U_iter < rx_data->length; U_iter += U_iter_inc)
        {
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++)
            {
#ifdef DEBUG                
                printf("XORing index %d into index %d\n", U_pair_iter + i, U_pair_iter);
#endif          
                p0 = rx_data->probabilities[U_pair_iter].prob0;
                p1 = rx_data->probabilities[U_pair_iter].prob1;
                q0 = rx_data->probabilities[U_pair_iter+i].prob0;
                q1 = rx_data->probabilities[U_pair_iter+i].prob1;
                
                rx_data->probabilities[U_pair_iter].prob0 = (p0 * q0) + (p1 * q1);
                rx_data->probabilities[U_pair_iter].prob1 = (p0 * q1) + (p1 * q0);
                
                if (rx_data->probabilities[U_pair_iter].prob0 > rx_data->probabilities[U_pair_iter].prob1)
                {
                    normalizer = rx_data->probabilities[U_pair_iter].prob0;
                    rx_data->probabilities[U_pair_iter+i].prob0 = (p0 * q0)/normalizer;
                    rx_data->probabilities[U_pair_iter+i].prob1 = (p1 * q1)/normalizer;
                }
                else
                {
                    normalizer = rx_data->probabilities[U_pair_iter].prob1;
                    rx_data->probabilities[U_pair_iter+i].prob0 = (p1 * q0)/normalizer;
                    rx_data->probabilities[U_pair_iter+i].prob1 = (p0 * q1)/normalizer;    
                }
                rx_data->probabilities[U_pair_iter].prob0 /= normalizer;
                rx_data->probabilities[U_pair_iter].prob1 /= normalizer;                
                num_iter++;
            }            
        }
    }
    
#ifdef DEBUG
    printDataSet(rx_data); 
#endif  

    for (int i = 0; i < rx_data->length; i++)
    {
        decoded_bits->is_frozen[i] = rx_data->is_frozen[i];
        if (rx_data->is_frozen[i] == true)
        {
            decoded_bits->bits[i] = 0;
        }
        else if (rx_data->probabilities[i].prob0 > rx_data->probabilities[i].prob1)
        {
            decoded_bits->bits[i] = 0;
        }
        else
        {
            decoded_bits->bits[i] = 1;
        }
    }
    
}

void decodeListPolarData(dataSet * rx_data, bitSet * decoded_bits, int list_size)
{
    int U_pair_iter = 0;
    int U_iter_inc = 0;
    int num_iter = 0;
    
    double p0;
    double p1;
    double q0;
    double q1;
    double normalizer;
    
    decoded_bits->length = rx_data->length;
    
    for (int i = rx_data->length >> 1; i >= 1; i >>= 1)
    {
        U_pair_iter = 0;
        U_iter_inc = i << 1;
        for (int U_iter = 0; U_iter < rx_data->length; U_iter += U_iter_inc)
        {
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++)
            {
#ifdef DEBUG                
                printf("XORing index %d into index %d\n", U_pair_iter + i, U_pair_iter);
#endif          
                p0 = rx_data->probabilities[U_pair_iter].prob0;
                p1 = rx_data->probabilities[U_pair_iter].prob1;
                q0 = rx_data->probabilities[U_pair_iter+i].prob0;
                q1 = rx_data->probabilities[U_pair_iter+i].prob1;
                
                rx_data->probabilities[U_pair_iter].prob0 = (p0 * q0) + (p1 * q1);
                rx_data->probabilities[U_pair_iter].prob1 = (p0 * q1) + (p1 * q0);
                
                if (rx_data->probabilities[U_pair_iter].prob0 > rx_data->probabilities[U_pair_iter].prob1)
                {
                    normalizer = rx_data->probabilities[U_pair_iter].prob0;
                    rx_data->probabilities[U_pair_iter+i].prob0 = (p0 * q0)/normalizer;
                    rx_data->probabilities[U_pair_iter+i].prob1 = (p1 * q1)/normalizer;
                }
                else
                {
                    normalizer = rx_data->probabilities[U_pair_iter].prob1;
                    rx_data->probabilities[U_pair_iter+i].prob0 = (p1 * q0)/normalizer;
                    rx_data->probabilities[U_pair_iter+i].prob1 = (p0 * q1)/normalizer;    
                }
                rx_data->probabilities[U_pair_iter].prob0 /= normalizer;
                rx_data->probabilities[U_pair_iter].prob1 /= normalizer;                
                num_iter++;
            }            
        }
    }
    
#ifdef DEBUG
    printDataSet(rx_data); 
#endif  

    for (int i = 0; i < rx_data->length; i++)
    {
        decoded_bits->is_frozen[i] = rx_data->is_frozen[i];
        if (rx_data->is_frozen[i] == true)
        {
            decoded_bits->bits[i] = 0;
        }
        else if (rx_data->probabilities[i].prob0 > rx_data->probabilities[i].prob1)
        {
            decoded_bits->bits[i] = 0;
        }
        else
        {
            decoded_bits->bits[i] = 1;
        }
    }
    
}

void generateDataBits(int length, bitSet * data_bits)
{
    data_bits->length = length;
    for (int i = 0; i < length; i++)
    {
        data_bits->bits[i] = rand() % 2;
    }
}

void generateUBits(int length, int num_frozen_bits, bitSet * U_bits)
{
    if (num_frozen_bits >= length)
    {
        return;
    }
    
    int rand_idx = 0;
    U_bits->length = length;
    for (int i = 0; i < length; i++)
    {
        U_bits->is_frozen[i] = false;
    }
    
    int frozen_bit_count = 0;
    while (frozen_bit_count < num_frozen_bits)
    {
#ifdef STATIC
        rand_idx = frozen_bit_count;
#else
        rand_idx = rand() % length;
#endif
        if (U_bits->is_frozen[rand_idx] == false)
        {
            U_bits->is_frozen[rand_idx] = true;
            frozen_bit_count++;
        }
    }
    
    for (int i = 0; i < length; i++)
    {
        if (U_bits->is_frozen[i] == false)
        {
#ifdef STATIC   
            U_bits->bits[i] = 0;
#else
            U_bits->bits[i] = rand() % 2;
#endif
        }
        else
        {
            U_bits->bits[i] = 0;
        }
    }
        
}

double calculateBER(bitSet * codeword, bitSet * senseword)
{
    double num_bits = (double)codeword->length;
    int num_errors = 0;
#ifdef DEBUG
    printf("\nbits in error: ");
#endif
    for (int i = 0; i < codeword->length; i++)
    {
        if (codeword->bits[i] != senseword->bits[i])
        {
#ifdef DEBUG
            printf("%d ", i);
#endif
            num_errors++;
        }
    }
#ifdef DEBUG
            printf("\n");
#endif    
    return ((double)num_errors)/num_bits;
}

void printBitSet(bitSet * bits_to_print)
{
    printf("\n\tBitSet of length %d: [", bits_to_print->length);
    for (int i = 0; i < bits_to_print->length; i++)
    {
#ifdef DEBUG
        printf("%d (%s frozen)%s", bits_to_print->bits[i], bits_to_print->is_frozen[i] ? "is" : "not", i < bits_to_print->length - 1 ? ", " : "]\n");
#else
        printf("%d%s", bits_to_print->bits[i], i < bits_to_print->length - 1 ? ", " : "]\n");
#endif
    }
}

void printDataSet(dataSet * data_to_print)
{
    printf("\n\tTX data of length %d:\n", data_to_print->length);
    for (int i = 0; i < data_to_print->length; i++)
    {
        printf("\t\tbit %d = %g :: p(0)=%g, p(1)=%g\n", 
                i,
                data_to_print->data[i], 
                data_to_print->probabilities[i].prob0, 
                data_to_print->probabilities[i].prob1);
    }
}

void printProbSet(probSet * probs_to_print)
{
    printf("\n\tProbSet of length %d: [", probs_to_print->length);
    for (int i = 0; i < probs_to_print->length; i++)
    {
        printf("(p(0)=%g, p(1)=%g)%s", 
                probs_to_print->probabilities[i].prob0, 
                probs_to_print->probabilities[i].prob1, 
                i < probs_to_print->length - 1 ? ", " : "]\n");
    }
}

double simulatePolarBER(double snr_db)
{
    bitSet U_bits;
    bitSet encoded_bits;
    bitSet decoded_bits;
    dataSet tx_data;
    generateUBits(MAX_NUM_BITS, MAX_NUM_BITS >> 1, &U_bits);
    encodePolarData(&U_bits, &encoded_bits);

    transmitBitsOverAwgn(snr_db, &encoded_bits, &tx_data);    
    decodePolarData(&tx_data, &decoded_bits);
    return calculateBER(&U_bits, &decoded_bits);
}

void simulatePolarBERCurve(int snr_start, int snr_end)
{
    int num_points = 0;
    double snrs[MAX_NUM_BITS];
    double bers[MAX_NUM_BITS];
    
    printf("\nSNRs: [");
    for (double snr = snr_start; snr <= snr_end; snr = snr + 0.5)
    {
        snrs[num_points] = snr;
        printf("%g%s", snr, (snr + 0.5 > snr_end) ? "]\n" : ", ");
        num_points++;
    }

    printf("\nBERs: [");
    for (int i = 0; i < num_points; i++)
    {
        bers[i] = simulatePolarBER(snrs[i]);
        printf("%g%s", bers[i], (i + 1 >= num_points) ? "]\n" : ", ");
    }
    
    
}

int main()
{
    srand(time(0));
    //bitSet frozen_bits;
    //bitSet data_bits;
    bitSet U_bits;
    bitSet encoded_bits;
    bitSet decoded_bits;
    dataSet tx_data;
    double snr_db = 2.0;
    /*
    initBitSet((unsigned int[]){1, 1}, 2, &data_bits);
    initBitSet((unsigned int[]){0, 2}, 2, &frozen_bits);   
    initBitSet((unsigned int[]){0, 0, 0, 0}, 4, &data_bits);
    initBitSet((unsigned int[]){0, 1, 1, 0}, 4, &data_bits);
    initBitSet((unsigned int[]){1, 4, 6, 2}, 4, &frozen_bits);
    
    initUBits(&data_bits, &frozen_bits, &U_bits);
    encodePolarDataV2(&data_bits, &frozen_bits, &encoded_bits);
    */
    generateUBits(MAX_NUM_BITS, MAX_NUM_BITS >> 1, &U_bits);
    encodePolarData(&U_bits, &encoded_bits);

    transmitBitsOverAwgn(snr_db, &encoded_bits, &tx_data);
    /*
    printf("\ndata_bits:");
    printBitSet(&data_bits);
    
    printf("\nfrozen_bits:");
    printBitSet(&frozen_bits);
    */
    //printf("\nU_bits:");
    //printBitSet(&U_bits);
    
    //printf("\nencoded_bits:");
    //printBitSet(&encoded_bits);    
    
    //printf("\ntransmitted data:");
    //printDataSet(&tx_data); 
    
    decodePolarData(&tx_data, &decoded_bits);    

    simulatePolarBERCurve(1, 10);
    
    return 0;
}



