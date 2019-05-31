/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include "encoder_inc.h"

int main()
{
    bitType in_bits[CODE_LENGTH] = {0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0};
    bitType out_bits[CODE_LENGTH];
    bitType expected_out_bits[CODE_LENGTH] = {0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0};
#ifdef PYNQ_VERSION
    axis_t input[CODE_LENGTH];
    axis_t output[CODE_LENGTH];
    for (int i = 0; i < CODE_LENGTH; i++)
    {
    	input[i].data = in_bits[i];
    }

    encodePolarData32(input, output);

    for (int i = 0; i < CODE_LENGTH; i++)
    {
    	out_bits[i] = output[i].data;
    }
#else
    encodePolarData32(in_bits, out_bits);
#endif
    int failure = 0;
    for (int i = 0; i < CODE_LENGTH; i++)
    {
        std::cout << "bit " << i << ": expected = " << expected_out_bits[i] << ", calculated = " << out_bits[i] << std::endl;
        if (expected_out_bits[i] != out_bits[i])
        {
            failure = 1;
            printf("FAILURE ON BIT %d\n", i);
        }
    }
    
    if (failure)
    {
        fprintf(stdout, "\n\nFAILURE!!\n");
    }
    else
    {
        fprintf(stdout, "\n\nSUCCESS!!\n");
    }
    
    return failure;
}





