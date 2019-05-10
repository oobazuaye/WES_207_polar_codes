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

    //simulatePolarBERCurve(1, 10, MAX_NUM_BITS);
    //simulatePolarBER(2, MAX_NUM_BITS);
    simulateListPolarBER(2, MAX_NUM_BITS, 2);
    return 0;
}
