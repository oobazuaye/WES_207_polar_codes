/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include "decoder_inc.h"

int main()
{
	probabilityType probabilities_0[CODE_LENGTH] = {0.09396054564868606, 0.05565834485383597, 0.17983825239875917, 0.8140618062660875, 0.8727103069758461, 0.5662173317263992, 0.2799230237468361, 0.18834142770837226, 0.7838191900506764, 0.9134714371387935, 0.8552726242651743, 0.1839100986070899, 0.8199858603777943, 0.28245051116428266, 0.8755904585955676, 0.9664891520682382, 0.9393630037622912, 0.9493585007903503, 0.0803824711183268, 0.7519294313435514, 0.8639635263784498, 0.8892369569134508, 0.8939160954114933, 0.8533178603955205, 0.033455915429682404, 0.9572884963245455, 0.07200805507980618, 0.9122052148550979, 0.6941638252080027, 0.8067206937328013, 0.04451099737317943, 0.20716786148567062};
	probabilityType probabilities_1[CODE_LENGTH] = {0.9060394543513139, 0.9443416551461641, 0.8201617476012408, 0.18593819373391252, 0.12728969302415394, 0.4337826682736008, 0.7200769762531639, 0.8116585722916277, 0.21618080994932365, 0.08652856286120647, 0.14472737573482575, 0.8160899013929102, 0.1800141396222057, 0.7175494888357173, 0.12440954140443239, 0.033510847931761845, 0.060636996237708796, 0.050641499209649665, 0.9196175288816733, 0.2480705686564486, 0.13603647362155025, 0.11076304308654916, 0.1060839045885067, 0.1466821396044795, 0.9665440845703176, 0.04271150367545451, 0.9279919449201939, 0.08779478514490213, 0.3058361747919973, 0.1932793062671987, 0.9554890026268206, 0.7928321385143293};
	bitType frozen_bits[CODE_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0};
	bitType out_bits[CODE_LENGTH];
    bitType expected_out_bits[CODE_LENGTH] = {0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1};

#ifdef PYNQ_VERSION
    axis_t input_0[CODE_LENGTH];
    axis_t input_1[CODE_LENGTH];
    axis_t frozen_bits_input[CODE_LENGTH];
    axis_t output[CODE_LENGTH];

    for (int i = 0; i < CODE_LENGTH; i++)
    {
    	input_0[i].data = probabilities_0[i];
    	input_1[i].data = probabilities_1[i];
    	frozen_bits_input[i].data = (frozen_bits[i] == 1) ? (probabilityType)1.0 : probabilityType(0.0);
    }

    decodePolarData32(input_0,
    				  input_1,
				      frozen_bits_input,
					  output);

    for (int i = 0; i < CODE_LENGTH; i++)
    {
    	out_bits[i] = output[i].data == (probabilityType)1.0;
    }
#else
    decodePolarData32(probabilities_0,
    		          probabilities_1,
				      frozen_bits,
					  out_bits);
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





