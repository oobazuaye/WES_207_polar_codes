
library IEEE;
   use IEEE.std_logic_1164.all;
   use IEEE.numeric_std.all;
   use IEEE.math_real.all;
   use IEEE.float_pkg.all;
--Library UNISIM;
--    use UNISIM.vcomponents.all;
library work;
	use work.polarProbabilityTypes.all;
	
---------- POLAR_DECODE_STAGE ------------

entity polar_decode_stage is
    generic 
    (
	    N: positive := 16;
	    DISTANCE: positive := 4
    );
    port (
        --CLK             : in std_logic;

        INPUT_PROBS_0     : in probabilityArrayType(0 to N-1);
        INPUT_PROBS_1     : in probabilityArrayType(0 to N-1);

        OUTPUT_PROBS_0     : out probabilityArrayType(0 to N-1);
        OUTPUT_PROBS_1     : out probabilityArrayType(0 to N-1)
    ); 
end polar_decode_stage; -- polar_decode_stage

architecture polar_decode_stage_arch of polar_decode_stage is

begin 
	decode_unit_gen_outer_Loop: 
	for i in 0 to N - 1 generate
	begin
		decode_unit_gen_conditional:
		if (i mod (DISTANCE*2)) = 0 generate 
		begin
			decode_unit_gen_inner_Loop:
			for j in i to i + DISTANCE - 1 generate
			begin 
				polar_decode_unit_j : 
					entity work.polar_decode_unit
					port map
					(
						in_top_bit_0 => INPUT_PROBS_0(j),
						in_top_bit_1 => INPUT_PROBS_1(j),
						in_bottom_bit_0 => INPUT_PROBS_0(j+DISTANCE),
						in_bottom_bit_1 => INPUT_PROBS_1(j+DISTANCE),

						out_top_bit_0 => OUTPUT_PROBS_0(j),
						out_top_bit_1 => OUTPUT_PROBS_1(j),
						out_bottom_bit_0 => OUTPUT_PROBS_0(j+DISTANCE),
						out_bottom_bit_1 => OUTPUT_PROBS_1(j+DISTANCE)		
					);
			end generate;
		end generate;
	end generate;
end polar_decode_stage_arch;

