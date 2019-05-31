library IEEE;
   use IEEE.std_logic_1164.all;
   use IEEE.numeric_std.all;
   use IEEE.math_real.all;
   use IEEE.float_pkg.all;
   use IEEE.fixed_pkg.all;

package polarProbabilityTypes is
	subtype probabilityType is float64;

	type probabilityArrayType is array(integer range <>) of probabilityType;

	type probabilityMatrixType is array(integer range <>, integer range <>) of probabilityType;

	type frozenArrayType is array(integer range <>) of boolean;

end polarProbabilityTypes;