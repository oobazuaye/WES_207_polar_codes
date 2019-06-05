# WES_207_polar_codes
polar codes implementations for UCSD WES 207 capstone

## Purpose 
this repository contains several different implementations of the basic encoder and successive cancellation decoder for polar codes, as well as utilities for generating bits and also simulating an AWGN channel through which to use these codes and observe their performance. 

this is all done as a means of easily learning and understanding how polar codes work, after having spent many weeks myself trying to understand the myriad of abstrusely written research papers on polar codes. these implementations include one in Python, C, HDL (SystemVerilog for the encoder, VHDL for the decoder), and Vivado HLS C, and each one has its own unique features and nuances.

in other words, the goal is that if you want to learn polar codes, and you have a reasonable grasp on software, you can use this repository to be able to quickly understand polar codes and implement them yourself!

## Directory Structure
 - polar_codes_software/ contains my C and Python libraries/implementations of polar encoders and decoders
   - the python library includes code for monte carlo frozen bit selection, and a function for drawing the XOR lattice in encoders
   - the C code contains a first pass at an implementation of the list successive cancellation decoder decoder. it will be updated and optimized in the future.
   
 - polar_codes_hdl/ contains my SystemVerilog and VHDL implementations of the polar encoder and SC decoder, respectively

 - polar_codes_hls/ contains my HLS implementation of an encoder and SC decoder for a 32-bit long polar code

## Other Resources
 - the lecture given by Arikan himself, the creator of polar codes: 
   - youtube video playlist: https://www.youtube.com/playlist?list=PLhAAV-jkgAdxWxAhFLxrcKJFXOzR6zqzb
   - slides: http://www2.egr.uh.edu/~zhan2/ECE6332/slidesarikan.pdf
   
 - the UCSD polar codes research group led by Alex Vardy has a fantastic paper on how to implement polar codes. it's pretty much the only paper i can recommend at an entry level (it also includes discussion of how to use a CRC to further optimize the list decoder): https://arxiv.org/pdf/1206.0050.pdf

 - for an advanced look at decoders, i found this paper particularly useful for introducing various algorithms for further optimizing the list decoder (without the use of a CRC): https://arxiv.org/abs/1208.3598
