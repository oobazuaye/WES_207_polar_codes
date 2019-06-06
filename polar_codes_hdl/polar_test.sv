

// this module is used for ModelSim testing of a polar encoder of size 16
module polar_encode_test #(parameter N = 16)
(
	input logic [0:N-1] input_bits,
	output logic [0:N-1] output_bits
);
	polar_encode #(.N(N)) test(input_bits, output_bits);
endmodule

// this module is used for ModelSim test of a polar decoder of size 4
module polar_decode_test #(parameter N = 4)
(
	output logic [0:N-1] output_bits
);
	logic [0:N-1] frozen_bits = {4'b0000}; // frozen bit selection, change any bit to 1 to freeze that bit

	// probability values representing a 0111:
	// P(0) = {0.75, 0.25, 0.25, 0.25},
	// P(1) = {0.25, 0.75, 0.75, 0.75}
	// 
	// should decode to 1001 with no frozen bits
	logic [63:0] probs_0 [0:N-1] = '{64'b0011111111101000000000000000000000000000000000000000000000000000,
								     64'b0011111111010000000000000000000000000000000000000000000000000000,
								     64'b0011111111010000000000000000000000000000000000000000000000000000,
								     64'b0011111111010000000000000000000000000000000000000000000000000000};
	logic [63:0] probs_1 [0:N-1] = '{64'b0011111111010000000000000000000000000000000000000000000000000000,
								     64'b0011111111101000000000000000000000000000000000000000000000000000,
								     64'b0011111111101000000000000000000000000000000000000000000000000000,
								     64'b0011111111101000000000000000000000000000000000000000000000000000};						     
	polar_decode #(.N(N)) test(probs_0, probs_1, frozen_bits, output_bits);
endmodule