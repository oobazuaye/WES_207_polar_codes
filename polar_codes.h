/******************************************************************************

polar_codes.h

header file for polar codes implementation

obosa obazuaye, 2019

*******************************************************************************/
#ifndef POLARCODES_H
#define POLARCODES_H


#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

/******************
 **** defines *****
 ******************/
#define PI 3.1415926536
#define M_SQRT1_2 0.70710678118654752440
#define MAX_NUM_BITS 1024
#define MAX_NUM_PATHS 2
//#define DEBUG
#define BER_TESTING
//#define NORMALIZE

#define STATIC
#define PRUNING_PARAMETER_MIN_LIST_LIKELIHOOD 0.1
#define USE_PRUNING_PARAMETR

/******************
 ****** types *****
 ******************/
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

typedef struct decodeIterator
{
    int branch_num;
    int branch_section;
    int upper_bit;
    bool taking_bit_1;
} decodeIterator;

typedef struct activePath
{
    bool is_active;
    int branch_width;
} activePath;

typedef struct decodeList
{
    dataSet path_list[MAX_NUM_BITS][MAX_NUM_PATHS];
    int num_bits;
    int list_size;
    bool path_active[MAX_NUM_BITS][MAX_NUM_PATHS];
    //activePath path_active[MAX_NUM_BITS][MAX_NUM_PATHS];
    int paths_in_use[MAX_NUM_BITS];
} decodeList;


/******************
 **** functions ***
 ******************/

// polar_utils.c
int intLog2(int x);
double AWGN_generator();
double snrToVariance(double snr_db);
double awgnSnrDb(double snr_db);

void printBitSet(bitSet * bits_to_print);
void printDataSet(dataSet * data_to_print);
void printProbSet(probSet * probs_to_print);

void copyDataSet(dataSet * dest, dataSet * src);
void copyPathActiveArray(bool * dest, bool * src, int length);
void copyActivePathArray(bool * dest, bool * src, int length);
void dumpPathsToProbSetArrayAndClear(decodeList * decode_list, int bit_idx, probSet * probs);

void initBitSet(unsigned int * bits, unsigned int length, bitSet * bit_set);
void initUBits(bitSet * data_bits, bitSet * frozen_bits, bitSet * U_bits);
void initDecodeList(dataSet * initial_rx_data, int list_size, decodeList * decode);

void transmitBitsOverAwgn(double snr_db, bitSet * bits_x, dataSet * data_y);

void generateDataBits(int length, bitSet * data_bits);
void generateUBits(int length, int num_frozen_bits, bitSet * U_bits);

double probPairMaxValue(binaryProb * probs);
double probPairMinValue(binaryProb * probs);
int findListMaxLikelihoodPath(int bit_idx, decodeList * decode_list);
int findListMinLikelihoodPath(int bit_idx, decodeList * decode_list);
void removePath(int bit_idx, int path_idx, decodeList * decode_list);

double calculateBER(bitSet * codeword, bitSet * senseword);
double simulatePolarBER(double snr_db, int num_bits);
void simulatePolarBERCurve(int snr_start, int snr_end, int num_bits);
double simulateListPolarBER(double snr_db, int num_bits, int list_size);
void simulateListPolarBERCurve(int snr_start, int snr_end, int num_bits, int list_size);

// polar_encode.c
void encodePolarDataV1(bitSet * data_bits, bitSet * frozen_bits, bitSet * encoded_bits);
void encodePolarDataV2(bitSet * data_bits, bitSet * frozen_bits, bitSet * encoded_bits);
void encodePolarData(bitSet * U_bits, bitSet * encoded_bits);

// polar_decode.c
void addToDecodeList(dataSet * rx_data, int bit_idx, int path_idx, decodeList * decode_list);
void splitPathAndAddToDecodeList(int path_idx, int bit_idx, int bit_pair_idx, probSet * bit_pair_probs, decodeList * decode_list);
void decodePolarData(dataSet * rx_data, bitSet * decoded_bits);
void decodeListPolarData(dataSet * rx_data, bitSet * decoded_bits, int list_size);


#endif /* POLARCODES_H */
