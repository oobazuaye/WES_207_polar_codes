/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include "encoder_inc.h"

void encodeStage(bitType U_bits[CODE_LENGTH], int stage_num, bitType encoded_bits[CODE_LENGTH])
{
    int U_pair_inc = 1 << stage_num;
	int U_iter_inc = U_pair_inc << 1;
	int U_bottom_idx = 0;
    for (int U_iter = 0; U_iter < CODE_LENGTH; U_iter += U_iter_inc)
    {
#pragma HLS LOOP_TRIPCOUNT max=32
        for (int U_top_idx = U_iter; U_top_idx < U_iter + U_pair_inc; U_top_idx++)
        {
#pragma HLS LOOP_TRIPCOUNT max=32
            U_bottom_idx = U_top_idx + U_pair_inc;
        	encoded_bits[U_top_idx] = U_bits[U_top_idx] ^ U_bits[U_bottom_idx];
        	encoded_bits[U_bottom_idx] = U_bits[U_bottom_idx];
        }
    }
}

void encodePolarDataUsingStages(bitType U_bits[CODE_LENGTH], bitType encoded_bits[CODE_LENGTH])
{
    bitType stage1_bits[CODE_LENGTH];
    bitType stage2_bits[CODE_LENGTH];
    bitType stage3_bits[CODE_LENGTH];
    bitType stage4_bits[CODE_LENGTH];
#pragma HLS PIPELINE
    encodeStage(U_bits, 0, stage1_bits);
    encodeStage(stage1_bits, 1, stage2_bits);
    encodeStage(stage2_bits, 2, stage3_bits);
    encodeStage(stage3_bits, 3, stage4_bits);
    encodeStage(stage4_bits, 4, encoded_bits);
}

#ifdef PYNQ_VERSION
void encodePolarData32(axis_t U_bits[CODE_LENGTH], axis_t encoded_bits[CODE_LENGTH])
{
#pragma HLS INTERFACE s_axilite port=return bundle=ctrl_io
#pragma HLS INTERFACE axis depth=32 port=U_bits
#pragma HLS INTERFACE axis depth=32 port=encoded_bits

	bitType input_bits[CODE_LENGTH];
	bitType output_bits[CODE_LENGTH];

	for (int i = 0; i < CODE_LENGTH; i++)
	{
		input_bits[i] = U_bits[i].data;
	}

	encodePolarDataUsingStages(input_bits, output_bits);

	for (int i = 0; i < CODE_LENGTH; i++)
	{
		encoded_bits[i].data = output_bits[i];
		encoded_bits[i].last = (i == CODE_LENGTH-1) ? 1 : 0;
	}

}
#else
void encodePolarData32(bitType U_bits[CODE_LENGTH], bitType encoded_bits[CODE_LENGTH])
{
    encodePolarDataUsingStages(U_bits, encoded_bits);
}
#endif








