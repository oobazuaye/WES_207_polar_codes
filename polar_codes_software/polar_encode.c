/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include "polar_codes.h"

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

    // iterate over the branch lengths, which are powers of 2 from 1 to n/2
    for (int i = 1; i <= encoded_bits->length >> 1; i <<= 1)
    {
        U_pair_iter = 0;
        U_iter_inc = i << 1; // skip over 2*branch length to get to the next pair start
        for (int U_iter = 0; U_iter < encoded_bits->length; U_iter += U_iter_inc)
        {
            for (U_pair_iter = U_iter; U_pair_iter < U_iter + i; U_pair_iter++) // match up all pairs within the branch length
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





