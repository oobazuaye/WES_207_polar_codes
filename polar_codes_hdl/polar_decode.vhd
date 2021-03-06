
library IEEE;
   use IEEE.std_logic_1164.all;
   use IEEE.numeric_std.all;
   use IEEE.math_real.all;
   use IEEE.float_pkg.all;
--Library UNISIM;
--    use UNISIM.vcomponents.all;

library work;
	use work.polarProbabilityTypes.all;

---------- POLAR_DECODE ------------

-- this module is the main top-level decode module for a 
-- polar code successive cancellation decoder.

entity polar_decode is
    generic 
    (
	    N: positive := 4
    );
    port (
        INPUT_PROBS_0     : in probabilityArrayType(0 to N-1);
        INPUT_PROBS_1     : in probabilityArrayType(0 to N-1);

        INPUT_IS_FROZEN   : in frozenArrayType(0 to N-1);

        OUTPUT_BITS       : out std_logic_vector(0 to N-1)
    ); 
end polar_decode; -- polar_decode

architecture polar_decode_arch of polar_decode is
	constant  M: positive := positive(ceil(log2(real(N))));
	
	-- creating signals representing intermediate probabilities
	-- for use in generated modules
	signal intermediate_probabilities_0 : probabilityArrayType(0 to (M*N)-1) := (others => (others => '0'));
    signal intermediate_probabilities_1 : probabilityArrayType(0 to (M*N)-1) := (others => (others => '0'));
	
	signal final_probabilities_0 : probabilityArrayType(0 to N-1) := (others => (others => '0'));
	signal final_probabilities_1 : probabilityArrayType(0 to N-1) := (others => (others => '0'));

    function start_idx(row : integer := 0) return integer is
    begin
    	return (row * N);
    end function;

    function end_idx(row : integer := 0) return integer is
    begin
    	return ((row + 1) * N) - 1;
    end function;    

begin 

	-- assign input probabilities to the first row of intermediate probabilities,
	-- rather than making another instance of polar_decode_stage just for the beginning
	-- probabilities
	intermediate_probabilities_0(start_idx(M-1) to end_idx(M-1)) <= INPUT_PROBS_0;
	intermediate_probabilities_1(start_idx(M-1) to end_idx(M-1)) <= INPUT_PROBS_1;

	-- generator for the decode stages
	decode_stage_gen: for i in M-1 downto 1 generate
	begin
		polar_decode_stage_i : entity work.polar_decode_stage
		generic map
		(
			N => N,
			DISTANCE => 2**i
		)
		port map
		(
			INPUT_PROBS_0 => intermediate_probabilities_0(start_idx(i) to end_idx(i)),
			INPUT_PROBS_1 => intermediate_probabilities_1(start_idx(i) to end_idx(i)),

			OUTPUT_PROBS_0 => intermediate_probabilities_0(start_idx(i-1) to end_idx(i-1)),
			OUTPUT_PROBS_1 => intermediate_probabilities_1(start_idx(i-1) to end_idx(i-1))
		);
	end generate;

	-- final probability stage
	polar_decode_stage_final : entity work.polar_decode_stage
	generic map
	(
		N => N,
		DISTANCE => 1
	)
	port map
	(
		INPUT_PROBS_0 => intermediate_probabilities_0(start_idx(0) to end_idx(0)),
		INPUT_PROBS_1 => intermediate_probabilities_1(start_idx(0) to end_idx(0)),

		OUTPUT_PROBS_0 => final_probabilities_0,
		OUTPUT_PROBS_1 => final_probabilities_1
	);

	-- converting final probabilities to output bits
	polar_decode_output_bits : entity work.polar_decode_output
	generic map
	(
		N => N
	)
	port map
	(
		INPUT_PROBS_0 => final_probabilities_0,
		INPUT_PROBS_1 => final_probabilities_1,

		INPUT_IS_FROZEN => INPUT_IS_FROZEN,

		OUTPUT_BITS => OUTPUT_BITS
	);

end polar_decode_arch;


