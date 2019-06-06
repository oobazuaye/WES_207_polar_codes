
library IEEE;
   use IEEE.std_logic_1164.all;
   use IEEE.numeric_std.all;
   use IEEE.math_real.all;
   use IEEE.float_pkg.all;
--Library UNISIM;
--    use UNISIM.vcomponents.all;

library work;
	use work.polarProbabilityTypes.all;

---------- POLAR_DECODE_OUTPUT ------------

-- this module performs the determination of the output bits 
-- according to the given input probabilities and frozen bit selections.
-- this is the final stage of a successive cancellation decoder.

entity polar_decode_output is
    generic 
    (
	    N: positive := 16
    );
    port (
        INPUT_PROBS_0     : in probabilityArrayType(0 to N-1);
        INPUT_PROBS_1     : in probabilityArrayType(0 to N-1);

        INPUT_IS_FROZEN   : in frozenArrayType(0 to N-1);

        OUTPUT_BITS       : out std_logic_vector(0 to N-1)
    ); 
end polar_decode_output;

architecture polar_decode_output_arch of polar_decode_output is
begin
	generate_output:
	for i in 0 to N-1 generate
	begin
		OUTPUT_BITS(i) <= '0' when ((INPUT_PROBS_0(i) > INPUT_PROBS_1(i)) or (INPUT_IS_FROZEN(i) = true)) else '1';
	end generate;
end polar_decode_output_arch;