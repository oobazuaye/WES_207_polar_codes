#include <stdio.h>
#include <stdlib.h>
#include "polar_codes.h"

int main()
{
    srand(time(0));
    //bitSet frozen_bits;
    //bitSet data_bits;
    /*
    bitSet U_bits;
    bitSet encoded_bits;
    bitSet decoded_bits;
    dataSet tx_data;
    double snr_db = 2.0;
    */
    /*
    initBitSet((unsigned int[]){1, 1}, 2, &data_bits);
    initBitSet((unsigned int[]){0, 2}, 2, &frozen_bits);
    initBitSet((unsigned int[]){0, 0, 0, 0}, 4, &data_bits);
    initBitSet((unsigned int[]){0, 1, 1, 0}, 4, &data_bits);
    initBitSet((unsigned int[]){1, 4, 6, 2}, 4, &frozen_bits);

    initUBits(&data_bits, &frozen_bits, &U_bits);
    encodePolarDataV2(&data_bits, &frozen_bits, &encoded_bits);
    */
    /*
    generateUBits(MAX_NUM_BITS, MAX_NUM_BITS >> 1, &U_bits);
    encodePolarData(&U_bits, &encoded_bits);

    transmitBitsOverAwgn(snr_db, &encoded_bits, &tx_data);
    */
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

    //decodePolarData(&tx_data, &decoded_bits);

    //printf("\ndecoded bits:");
    //printBitSet(&decoded_bits);
    /*
    printf("TESTING SC vs SCL, BIT SIZE %d, SNR %g dB\n", MAX_NUM_BITS, TEST_SNR);
    printf("Running normal SC decoder...\n");
    simulatePolarBERCurve(0.5, 3, MAX_NUM_BITS);
    printf("\n\nRunning list SC decoder...\n");
    */
    simulateMultiListPolarBERCurve(1, 3, MAX_NUM_BITS, MAX_NUM_PATHS);
    /*
    printf("\n\nRunning list SC decoder with list size %d...\n", MAX_NUM_PATHS);
    simulateListPolarBERCurve(1, 10, MAX_NUM_BITS, MAX_NUM_PATHS);
    */
    /*
    printf("TESTING SC vs SCL, BIT SIZE %d, SNR %g dB\n", MAX_NUM_BITS, TEST_SNR);
    printf("Running normal SC decoder...\n");
    simulatePolarBER(TEST_SNR, MAX_NUM_BITS);
    printf("\nRunning list SC decoder with list size %d...\n", MAX_NUM_PATHS);
    simulateListPolarBER(TEST_SNR, MAX_NUM_BITS, MAX_NUM_PATHS);
    */
    return 0;
}
