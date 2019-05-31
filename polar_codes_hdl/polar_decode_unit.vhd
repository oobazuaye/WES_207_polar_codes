
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
	signal top_0_bottom_0 : probabilityType;
	signal top_1_bottom_0 : probabilityType;
	signal top_0_bottom_1 : probabilityType;
	signal top_1_bottom_1 : probabilityType;
	signal new_top_bit_0 : probabilityType;
	signal new_top_bit_1 : probabilityType;
begin
	
	top_0_bottom_0 <= in_top_bit_0 * in_bottom_bit_0;
	top_1_bottom_0 <= in_top_bit_1 * in_bottom_bit_0;
	top_0_bottom_1 <= in_top_bit_0 * in_bottom_bit_1;
	top_1_bottom_1 <= in_top_bit_1 * in_bottom_bit_1;

	new_top_bit_0 <= top_0_bottom_0 + top_1_bottom_1;
	new_top_bit_1 <= top_1_bottom_0 + top_0_bottom_1;

	out_top_bit_0 <= new_top_bit_0;
	out_top_bit_1 <= new_top_bit_1;
	out_bottom_bit_0 <= top_0_bottom_0 when new_top_bit_0 > new_top_bit_1 else top_1_bottom_0;
	out_bottom_bit_1 <= top_1_bottom_1 when new_top_bit_0 > new_top_bit_1 else top_0_bottom_1;
end polar_decode_unit_arch;


