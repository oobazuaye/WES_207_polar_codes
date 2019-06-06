/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include "decoder_inc.h"

void decodeStage(probabilityType probabilities_0_in[CODE_LENGTH],
				 probabilityType probabilities_1_in[CODE_LENGTH],
		         int stage_num,
				 probabilityType probabilities_0_out[CODE_LENGTH],
				 probabilityType probabilities_1_out[CODE_LENGTH])
{
    int U_pair_inc = 1 << stage_num;
	int U_iter_inc = U_pair_inc << 1;
	int U_bottom_idx = 0;
	probabilityType a0, a1, b0, b1;
    for (int U_iter = 0; U_iter < CODE_LENGTH; U_iter += U_iter_inc)
    {
#pragma HLS LOOP_TRIPCOUNT max=32
        for (int U_top_idx = U_iter; U_top_idx < U_iter + U_pair_inc; U_top_idx++)
        {
#pragma HLS LOOP_TRIPCOUNT max=32
            U_bottom_idx = U_top_idx + U_pair_inc;
            a0 = (probabilities_0_in[U_top_idx] * probabilities_0_in[U_bottom_idx]) +
            	 (probabilities_1_in[U_top_idx] * probabilities_1_in[U_bottom_idx]);
            a1 = (probabilities_1_in[U_top_idx] * probabilities_0_in[U_bottom_idx]) +
            	 (probabilities_0_in[U_top_idx] * probabilities_1_in[U_bottom_idx]);
            if (a0 > a1)
            {
            	b0 = probabilities_0_in[U_top_idx] * probabilities_0_in[U_bottom_idx];
            	b1 = probabilities_1_in[U_top_idx] * probabilities_1_in[U_bottom_idx];
            }
            else
            {
            	b0 = probabilities_1_in[U_top_idx] * probabilities_0_in[U_bottom_idx];
            	b1 = probabilities_0_in[U_top_idx] * probabilities_1_in[U_bottom_idx];
            }
            probabilities_0_out[U_top_idx] = a0;
            probabilities_1_out[U_top_idx] = a1;
            probabilities_0_out[U_bottom_idx] = b0;
            probabilities_1_out[U_bottom_idx] = b1;
        }
    }
}

void decodeFinalProbabilities(probabilityType probabilities_0[CODE_LENGTH],
							  probabilityType probabilities_1[CODE_LENGTH],
							  bitType frozen_bits[CODE_LENGTH],
							  bitType decoded_bits[CODE_LENGTH])
{
	for (int i = 0; i < CODE_LENGTH; i++)
	{
		if (frozen_bits[i] ||
			probabilities_0[i] > probabilities_1[i])
		{
			decoded_bits[i] = 0;
		}
		else
		{
			decoded_bits[i] = 1;
		}
	}
}

void decodePolarDataUsingStages(probabilityType probabilities_0[CODE_LENGTH],
								probabilityType probabilities_1[CODE_LENGTH],
								bitType frozen_bits[CODE_LENGTH],
								bitType decoded_bits[CODE_LENGTH])
{
	probabilityType stage4_probabilities0[CODE_LENGTH], stage4_probabilities1[CODE_LENGTH];
	probabilityType stage3_probabilities0[CODE_LENGTH], stage3_probabilities1[CODE_LENGTH];
	probabilityType stage2_probabilities0[CODE_LENGTH], stage2_probabilities1[CODE_LENGTH];
	probabilityType stage1_probabilities0[CODE_LENGTH], stage1_probabilities1[CODE_LENGTH];
	probabilityType final_probabilities0[CODE_LENGTH],  final_probabilities1[CODE_LENGTH];
#pragma HLS DATAFLOW
    decodeStage(probabilities_0, probabilities_1,
    		    4,
				stage4_probabilities0, stage4_probabilities1);
    decodeStage(stage4_probabilities0, stage4_probabilities1,
    		    3,
				stage3_probabilities0, stage3_probabilities1);
    decodeStage(stage3_probabilities0, stage3_probabilities1,
    		    2,
				stage2_probabilities0, stage2_probabilities1);
    decodeStage(stage2_probabilities0, stage2_probabilities1,
    		    1,
				stage1_probabilities0, stage1_probabilities1);
    decodeStage(stage1_probabilities0, stage1_probabilities1,
    		    0,
				final_probabilities0, final_probabilities1);
    decodeFinalProbabilities(final_probabilities0,
    		                 final_probabilities1,
							 frozen_bits,
							 decoded_bits);
}

#ifdef PYNQ_VERSION
void decodePolarData32(axis_t probabilities_0[CODE_LENGTH],
					   axis_t probabilities_1[CODE_LENGTH],
					   axis_t frozen_bits[CODE_LENGTH],
					   axis_t decoded_bits[CODE_LENGTH])
{
//#pragma HLS INTERFACE s_axilite port=return bundle=ctrl_io
#pragma HLS INTERFACE axis depth=32 port=probabilities_0
#pragma HLS INTERFACE axis depth=32 port=probabilities_1
#pragma HLS INTERFACE axis depth=32 port=frozen_bits
#pragma HLS INTERFACE axis depth=32 port=decoded_bits

	probabilityType input_0[CODE_LENGTH];
	probabilityType input_1[CODE_LENGTH];
	bitType frozen_bit_input[CODE_LENGTH];
	bitType output[CODE_LENGTH];

	for (int i = 0; i < CODE_LENGTH; i++)
	{
		input_0[i] = probabilities_0[i].data;
		input_1[i] = probabilities_1[i].data;
		frozen_bit_input[i] = frozen_bits[i].data == (probabilityType)1.0;
	}

	decodePolarDataUsingStages(input_0,
							   input_1,
							   frozen_bit_input,
							   output);

	for (int i = 0; i < CODE_LENGTH; i++)
	{
		decoded_bits[i].data = (output[i] == 1) ? (probabilityType)1.0 : (probabilityType)0.0;
		decoded_bits[i].last = (i == CODE_LENGTH-1) ? 1 : 0;
	}
}
#else
void decodePolarData32(probabilityType probabilities_0[CODE_LENGTH],
					   probabilityType probabilities_1[CODE_LENGTH],
					   bitType frozen_bits[CODE_LENGTH],
					   bitType decoded_bits[CODE_LENGTH])
{
	decodePolarDataUsingStages(probabilities_0,
							   probabilities_1,
							   frozen_bits,
							   decoded_bits);
}
#endif







