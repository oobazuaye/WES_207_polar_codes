/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#ifndef ENCODER_INC_H
#define ENCODER_INC_H

#define PYNQ_VERSION

#include <ap_fixed.h>
#include <ap_int.h>

typedef ap_uint<1> bitType;
typedef ap_ufixed<32, 1, AP_RND, AP_WRAP, 1> probabilityType;

struct axis_t
{
	probabilityType data;
	ap_int<1> last;
};

#define CODE_LENGTH 32

void decodeStage(probabilityType probabilities_0_in[CODE_LENGTH],
				 probabilityType probabilities_1_in[CODE_LENGTH],
		         int stage_num,
				 probabilityType probabilities_0_out[CODE_LENGTH],
				 probabilityType probabilities_1_out[CODE_LENGTH]);

void decodeFinalProbabilities(probabilityType probabilities_0[CODE_LENGTH],
							  probabilityType probabilities_1[CODE_LENGTH],
							  bitType frozen_bits[CODE_LENGTH],
							  bitType decoded_bits[CODE_LENGTH]);

void decodePolarDataUsingStages(probabilityType probabilities_0[CODE_LENGTH],
								probabilityType probabilities_1[CODE_LENGTH],
								bitType frozen_bits[CODE_LENGTH],
								bitType decoded_bits[CODE_LENGTH]);

#ifdef PYNQ_VERSION
void decodePolarData32(axis_t probabilities_0[CODE_LENGTH],
					   axis_t probabilities_1[CODE_LENGTH],
					   axis_t frozen_bits[CODE_LENGTH],
					   axis_t decoded_bits[CODE_LENGTH]);
#else
	void decodePolarData32(probabilityType probabilities_0[CODE_LENGTH],
						   probabilityType probabilities_1[CODE_LENGTH],
						   bitType frozen_bits[CODE_LENGTH],
						   bitType decoded_bits[CODE_LENGTH]);
#endif

#endif






