/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#ifndef ENCODER_INC_H
#define ENCODER_INC_H

#define PYNQ_VERSION

#include <ap_int.h>

#ifdef PYNQ_VERSION
typedef ap_uint<32> bitType;
#else
typedef ap_uint<1> bitType;
#endif

struct axis_t
{
	bitType data;
	ap_int<1> last;
};

#define CODE_LENGTH 32

void encodeStage(bitType U_bits[CODE_LENGTH], int stage_num, bitType encoded_bits[CODE_LENGTH]);
void encodePolarDataUsingStages(bitType U_bits[CODE_LENGTH], bitType encoded_bits[CODE_LENGTH]);


#ifdef PYNQ_VERSION
void encodePolarData32(axis_t U_bits[CODE_LENGTH], axis_t encoded_bits[CODE_LENGTH]);
#else
void encodePolarData32(bitType U_bits[CODE_LENGTH], bitType encoded_bits[CODE_LENGTH]);
#endif



#endif






