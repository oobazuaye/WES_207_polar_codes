/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include "polar_codes.h"

// using Lee Daniel Crocker's solution from
// https://stackoverflow.com/questions/3064926/how-to-write-log-base2-in-c-c
int intLog2(int x)
{
    int result = 0;
    while (x >>= 1)
    {
        result++;
    }
    return result;
}

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

void initDecodeList(dataSet * initial_rx_data, int list_size, decodeList * decode)
{
    int num_bits = initial_rx_data->length;
    decode->num_bits = num_bits;
    if (list_size < 1)
    {
        // provision to make sure list size is at least 1
        list_size = 1;
    }
    decode->list_size = list_size;
    for (int i = 0; i < num_bits; i++)
    {
        // always start with the first path active: the initial dataSet
        copyDataSet(&(decode->path_list[i][0]), initial_rx_data);
        decode->paths_in_use[i] = 1;
        decode->path_active[i][0] = true;
        for (int j = 1; j < list_size; j++)
        {
            decode->path_active[i][j] = false;
        }
    }
}

// function for deep copying a dataSet struct
void copyDataSet(dataSet * dest, dataSet * src)
{
    dest->length = src->length;
    for (int i = 0; i < src->length; i++)
    {
        dest->data[i] = src->data[i];
        dest->is_frozen[i] = src->is_frozen[i];
        dest->probabilities[i].llr = src->probabilities[i].llr;
        dest->probabilities[i].prob0 = src->probabilities[i].prob0;
        dest->probabilities[i].prob1 = src->probabilities[i].prob1;
    }
}

void copyPathActiveArray(bool * dest, bool * src, int length)
{
    for (int i = 0; i < length; i++)
    {
        dest[i] = src[i];
    }
}

void dumpPathsToProbSetArrayAndClear(decodeList * decode_list, int bit_idx, probSet * probs)
{
    int prob_idx = 0;
    for (int i = 0; i < decode_list->list_size; i++)
    {
        if (decode_list->path_active[bit_idx][i] == true)
        {
            probs->probabilities[prob_idx] = decode_list->path_list[bit_idx][i].probabilities[bit_idx];
            decode_list->path_active[bit_idx][i] = false;
            prob_idx++;
        }
    }
    probs->length = prob_idx;
}

double probPairMinValue(binaryProb * probs)
{
    return (probs->prob0 < probs->prob1 ? probs->prob0 : probs->prob1);
}

double probPairMaxValue(binaryProb * probs)
{
    return (probs->prob0 > probs->prob1 ? probs->prob0 : probs->prob1);
}

void transmitBitsOverAwgn(double snr_db, bitSet * bits_x, dataSet * data_y)
{
    double variance = snrToVariance(snr_db);
    //double pdf_const = 1.0 / sqrt(2.0*PI*variance);
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
    double ber = 0;
#ifdef BER_TESTING
    printf("\nbits in error: ");
#endif
    for (int i = 0; i < codeword->length; i++)
    {
        if (codeword->bits[i] != senseword->bits[i])
        {
#ifdef BER_TESTING
            printf("%d ", i);
#endif
            num_errors++;
        }
    }
#ifdef BER_TESTING
            printf("\n");
#endif
    ber = ((double)num_errors)/num_bits;
    printf("BER is: %g\n", ber);
    return ber;
}

double simulatePolarBER(double snr_db, int num_bits)
{
    bitSet U_bits;
    bitSet encoded_bits;
    bitSet decoded_bits;
    dataSet tx_data;
    generateUBits(num_bits, num_bits >> 1, &U_bits);
    encodePolarData(&U_bits, &encoded_bits);

    transmitBitsOverAwgn(snr_db, &encoded_bits, &tx_data);
    decodePolarData(&tx_data, &decoded_bits);
    return calculateBER(&U_bits, &decoded_bits);
}

void simulatePolarBERCurve(int snr_start, int snr_end, int num_bits)
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
        bers[i] = simulatePolarBER(snrs[i], num_bits);
        printf("%g%s", bers[i], (i + 1 >= num_points) ? "]\n" : ", ");
    }
}

double simulateListPolarBER(double snr_db, int num_bits, int list_size)
{
    bitSet U_bits;
    bitSet encoded_bits;
    bitSet decoded_bits;
    dataSet tx_data;
    generateUBits(num_bits, num_bits >> 1, &U_bits);
    encodePolarData(&U_bits, &encoded_bits);

    transmitBitsOverAwgn(snr_db, &encoded_bits, &tx_data);
    decodeListPolarData(&tx_data, &decoded_bits, list_size);
    return calculateBER(&U_bits, &decoded_bits);
}

void simulateListPolarBERCurve(int snr_start, int snr_end, int num_bits, int list_size)
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
        bers[i] = simulateListPolarBER(snrs[i], num_bits, list_size);
        printf("%g%s", bers[i], (i + 1 >= num_points) ? "]\n" : ", ");
    }
}






