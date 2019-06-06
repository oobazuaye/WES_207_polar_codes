
library IEEE;
   use IEEE.std_logic_1164.all;
   use IEEE.numeric_std.all;
   use IEEE.math_real.all;
   use IEEE.float_pkg.all;
--Library UNISIM;
--    use UNISIM.vcomponents.all;
library work;
	use work.polarProbabilityTypes.all;


---------- POLAR_DECODE_UNIT ------------

-- this module is the main low-level unit of probability propagation
-- for the polar code successive cancellation decoder.
-- it calculates what the new probabilities for the top and bottom bit should be
-- after passing through the XOR unit in reverse

entity polar_decode_unit is
	port
	(
		in_top_bit_0 : in probabilityType;
		in_top_bit_1 : in probabilityType;
		in_bottom_bit_0 : in probabilityType;
		in_bottom_bit_1 : in probabilityType;
		out_top_bit_0 : out probabilityType;
		out_top_bit_1 : out probabilityType;
		out_bottom_bit_0 : out probabilityType;
		out_bottom_bit_1 : out probabilityType
	);
end polar_decode_unit;

architecture polar_decode_unit_arch of polar_decode_unit is
	signal top_0_bottom_0 : probabilityType := (others => '0');
	signal top_1_bottom_0 : probabilityType := (others => '0');
	signal top_0_bottom_1 : probabilityType := (others => '0');
	signal top_1_bottom_1 : probabilityType := (others => '0');
	signal new_top_bit_0 : probabilityType := (others => '0');
	signal new_top_bit_1 : probabilityType := (others => '0');

	signal choose_0 : boolean := false;
begin
	
	top_0_bottom_0 <= in_top_bit_0 * in_bottom_bit_0;
	top_1_bottom_0 <= in_top_bit_1 * in_bottom_bit_0;
	top_0_bottom_1 <= in_top_bit_0 * in_bottom_bit_1;
	top_1_bottom_1 <= in_top_bit_1 * in_bottom_bit_1;

	new_top_bit_0 <= top_0_bottom_0 + top_1_bottom_1;
	new_top_bit_1 <= top_1_bottom_0 + top_0_bottom_1;

	choose_0 <= new_top_bit_0 > new_top_bit_1;
	
	out_top_bit_0 <= new_top_bit_0;
	out_top_bit_1 <= new_top_bit_1;
	out_bottom_bit_0 <= top_0_bottom_0 when choose_0 else top_1_bottom_0;
	out_bottom_bit_1 <= top_1_bottom_1 when choose_0 else top_0_bottom_1;
end polar_decode_unit_arch;


