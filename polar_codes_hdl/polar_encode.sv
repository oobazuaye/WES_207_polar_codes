
// this module generates the XOR lattice for the entire
// polar encoder of a given code length N
module polar_encode #(parameter N = 16, M = $clog2(N))
(
	input logic [0:N-1] input_bits,
	output logic [0:N-1] output_bits
);
	logic [0:N-1] intermediate_bits [0:M-1];

	assign intermediate_bits[0] = input_bits;

	genvar i;
	generate
		for (i = 0; i < M - 1; i++)
		begin : each_stage
			polar_encode_stage #(.N(N), .DISTANCE(2**i)) polar_encode_stage_i(.input_bits(intermediate_bits[i]), .output_bits(intermediate_bits[i + 1]));
		end
	endgenerate
	polar_encode_stage #(.N(N), .DISTANCE(N >> 1)) polar_encode_final_stage(.input_bits(intermediate_bits[M-1]), .output_bits(output_bits));
endmodule

// this module generates the XOR lattice for a given stage of the polar encoder
module polar_encode_stage #(parameter N = 16, DISTANCE = 1)
(
	input logic [0:N-1] input_bits,
	output logic [0:N-1] output_bits
);
	genvar i, j;
	generate
		for (i = 0; i < N; i += DISTANCE*2)
		begin : each_bit
			for (j = i; j < i + DISTANCE; j++)
			begin : each_pair
				assign output_bits[j] = input_bits[j] ^ input_bits[j + DISTANCE];
				assign output_bits[j + DISTANCE] = input_bits[j + DISTANCE];
			end
		end
	endgenerate
endmodule
