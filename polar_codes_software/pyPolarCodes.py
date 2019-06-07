# py3PolarCodes.py
# obosa obazuaye, 2019
# polar code utilities and simulations in python
from __future__ import print_function
import math
import turtle
import numpy
import random


MONTE_CARLO_ITERATIONS = 10**5 # the literature uses 10**7, but that takes too long to run in Python
ITERATION_COUNTER_MOD = 10**4
codeDraw = 0
fontSize = 16
writeFont = ("Arial", 16, "normal")

# generates an array of given length with a random bit in each index
def randomBits(length, print_bits=False):
    bits = [random.randint(0, 1) for num in range(length)]
    if print_bits:
        print(''.join([str(bit) for bit in bits]))
    return bits

# takes a given array of bits,
# and randomly assigns some of the 0 bits to be frozen bits.
# returns an array of booleans, used in decoder functions
def randomFrozenBitSelection(data):
    return [False if bit == 1 else random.choice([True, False]) for bit in data]

# generates an array of given length with a random bit in each index,
# but also assigns 0 to any that are designated as frozen bits
def randomDataBitsWithFrozenBits(length, frozen_bits, print_bits=False):
    bits = [0 if frozen_bits[idx] else random.randint(0, 1) for idx in range(length)]
    if print_bits:
        print(''.join([str(bit) for bit in bits]))
    return bits

# converts an array of bit error ratios into an array of frozen bits
# by ensuring the top num_frozen_bits highest error indices are frozen
# (so as to make sure they aren't used for data bits, since they have a high chance of error)
def converPErrToFrozenBits(p_errs, num_frozen_bits):
    code_length = len(p_errs)
    frozen_bit_indices = sorted(range(code_length), key = lambda x: p_errs[x])[-num_frozen_bits:]
    frozen_bits = [bit_idx in frozen_bit_indices for bit_idx in range(code_length)]
    return frozen_bits

# color gradient code adapted from https://bsou.io/posts/color-gradients-with-python
def linear_gradient(start_rgb, finish_rgb=(255,255,255), n=10):
  ''' returns a gradient list of (n) colors between
    two hex colors. start_rgb and finish_rgb
    should be RGB tuples'''

  # Initilize a list of the output colors with the starting color
  RGB_list = [start_rgb]
  # Calcuate a color at each evenly spaced value of t from 1 to n
  for t in range(1, n):
    # Interpolate RGB vector for color at the current value of t
    curr_vector = [
      int(start_rgb[j] + (float(t)/(n-1))*(finish_rgb[j]-start_rgb[j]))
      for j in range(3)
    ]
    # Add it to our list of output colors
    RGB_list.append(curr_vector)

  return RGB_list

# monte carlo simulation used to determine which bits should be frozen bits
# for a given code length and using the basic SC decoder in AWGN.
# uses the global variable MONTE_CARLO_ITERATIONS defined at the top of the file
def determineAwgnFrozenBits(code_length, num_frozen_bits, snr, draw=False):
    log2_result = math.log(code_length, 2)
    remainder = math.modf(log2_result)[0]
    
    if remainder != 0.0:
        print("Length of data is not power of 2!")
        return

    if draw:
        colors = linear_gradient((0, 255, 0), (255, 0, 0), code_length)
        turtle.mode("logo")
        turtle.colormode(255)
        drawScreen = turtle.Screen()
        turtles = []
        writingTurtle = turtle.Turtle()
        writingTurtle.speed(10)
        channelLength = drawScreen.window_width()/2
        turtle.bgcolor("black")


        #calculate ratios of the diagram based on number of elements
        turtleSize = 20
        channelLength = drawScreen.window_width()/2
        channelSpacing = drawScreen.window_height() / code_length
        initialX = turtleSize/2 - drawScreen.window_width()/2
        initialY = drawScreen.window_height()/2 - turtleSize/2 - channelSpacing/2     
        y_pos = initialY
        x_start = initialX

        writingTurtle.penup()
        writingTurtle.goto(initialX, initialY + (fontSize*2))
        writingTurtle.hideturtle()
    
    bit_errs = [0] * code_length
    print("running for " + str(MONTE_CARLO_ITERATIONS) + " iterations!")
    print("iteration number ", end=' ')
        
    for iteration in range(MONTE_CARLO_ITERATIONS):
        if iteration % ITERATION_COUNTER_MOD == 0:
            print(iteration, end=' ')
            if draw:
                y_pos = initialY
                writingTurtle.goto(initialX, initialY + (fontSize*2))
                writingTurtle.clear()
                writingTurtle.color("white")
                writingTurtle.write("iteration number " + str(iteration), font = ("Calibri", fontSize, "bold"))
                
        data = randomBits(code_length)
        encoded_data = polarEncodeSimple(data)
        decoded_data = polarDecodeWithAwgnSimulation(encoded_data, [False] * code_length, snr, True)
        for bit_idx in range(code_length):
            bit_errs[bit_idx] += decoded_data[bit_idx] != data[bit_idx]

        if draw and iteration % ITERATION_COUNTER_MOD == 0:
            p_errs = [num_errs / (1.0 * (iteration + 1)) for num_errs in bit_errs]
            sorted_bit_errs = sorted(p_errs)
            print(sorted_bit_errs, p_errs)
            for bit_idx in range(code_length):
                err_rate = p_errs[bit_idx]
                
                color_idx = sorted_bit_errs.index(err_rate)
                
                writingTurtle.penup()
                writingTurtle.goto(x_start, y_pos)
                writingTurtle.hideturtle()
                writingTurtle.color("white")
                writingTurtle.write("BIT " + str(bit_idx) + " error rate: ", font = writeFont)
                writingTurtle.color(colors[color_idx])
                writingTurtle.setx(x_start+(fontSize * 15))
                writingTurtle.write(err_rate, font = ("Arial", int(fontSize + (fontSize * err_rate)), "bold")) 
                y_pos -= channelSpacing                

                
    p_errs = [num_errs / (1.0 * MONTE_CARLO_ITERATIONS) for num_errs in bit_errs]
    print()
    print(p_errs)
    frozen_bits = converPErrToFrozenBits(p_errs, num_frozen_bits)
    
    if draw:
        y_pos = initialY
        for bit_idx in range(code_length):
            writingTurtle.penup()
            writingTurtle.goto(x_start, y_pos)
            writingTurtle.hideturtle()
            writingTurtle.setx(x_start + (1.5*channelLength))
            writingTurtle.color((255,0,0) if frozen_bits[bit_idx] else (0,255,0))
            writingTurtle.write("FROZEN!" if frozen_bits[bit_idx] else "DATA!", font = ("Impact", int(fontSize + (fontSize * err_rate)), "bold")) 
            y_pos -= channelSpacing
            
    return frozen_bits

# encodes a given array of bits using the polar encoding algorithm,
# and draws the XOR lattice using Turtle graphics
def polarEncodeDraw(data, draw=False):
    turtle.mode("logo")
    global codeDraw
    codeDraw = turtle.Turtle()
    codeDraw.speed(3)
    drawScreen = turtle.Screen()
    turtleSize = 20
    channelLength = drawScreen.window_width()/2



    #place the turtle near the top-left corner of the screen
    codeDraw.penup()
    codeDraw.goto(turtleSize/2 - drawScreen.window_width()/2, drawScreen.window_height()/2 - turtleSize/2)

    #calculate number of elements and number of branches
    n = len(data)
    log2_result = math.log(n, 2)
    remainder = math.modf(log2_result)[0]
    
    if remainder != 0.0:
        print("Length of data is not power of 2!")
        return

    #create variables that will be referenced whether or not we draw
    initialX = 0
    initialY = 0
    channelSpacing = 0
    branchSpacing = 0
    branchAdjustment = 0
    xorRadius = 0
    if draw == False:
        #close screen if we're not drawing
        try:
            drawScreen.bye()
        except:
            print("turtle window already closed!")
    else:
        #calculate ratios of the diagram based on number of elements
        channelSpacing = drawScreen.window_height() / n
        branchSpacing = channelLength / (2 * log2_result)
        branchAdjustment = branchSpacing / (n / 2)
        xorRadius = branchSpacing / (2 * log2_result)
        initialX = codeDraw.pos()[0] + branchSpacing
        initialY = codeDraw.pos()[1] - channelSpacing/2        

    y_pos = initialY
    x_start = initialX
    branches = range(int(log2_result))
    encoded_bits = [bit for bit in data]

    if draw:
        #draw all the channel lines first
        for idx in range(n):
            codeDraw.goto(x_start, y_pos)
            codeDraw.color("green")
            codeDraw.write(data[idx])
            codeDraw.color("black")
            codeDraw.setheading(90)
            codeDraw.pendown()
            codeDraw.forward(channelLength)
            codeDraw.penup()
            y_pos -= channelSpacing

    x_start = initialX
    branch_dists = [2**x for x in branches]
    for pair_dist in branch_dists:
        y_pos = initialY
        x_start += branchSpacing #increment x starting point by which branch we're on
        orig_x_start = x_start #save off original x starting point for this branch in case we need to nudge stuff over for overlapping XORs
        is_paired = [False] * n #create a pairing boolean array for easy determination of which channels need to be paired
        for bit_idx in range(0, n):
            if draw:
                #if we're drawing, go to the starting position for this branch XOR
                codeDraw.penup()
                codeDraw.goto(x_start, y_pos)
                codeDraw.pendown()
            if is_paired[bit_idx] is False:
                #if this one hasnt been paired yet, pair it with the channel pair_dist below it
                pair_idx = bit_idx + pair_dist
                encoded_bits[bit_idx] ^= encoded_bits[pair_idx] 
                if draw:
                    if True in is_paired[bit_idx:pair_idx]:
                        #slice the array to see if any have already been paired below this one. if they have,
                        # then we should nudge the x start over so that the XORs dont overlap entirely
                        x_start += branchAdjustment
                        codeDraw.penup()
                        codeDraw.goto(x_start, y_pos)
                        codeDraw.pendown()
                    else:
                        #if there arent any overlapping, use the original starting X for this branch
                        x_start = orig_x_start
                        codeDraw.penup()
                        codeDraw.goto(orig_x_start, y_pos)
                        codeDraw.pendown()
                    #draw an XOR using a function
                    drawXor(pair_dist, channelSpacing, xorRadius, encoded_bits[bit_idx], encoded_bits[pair_idx], branch_dists[-1] == pair_dist)
    
                is_paired[bit_idx] = True
                is_paired[pair_idx] = True
            y_pos -= channelSpacing #jump down some space for each channel

    codeDraw.penup
    return encoded_bits

# helper function for the above, draws an XOR
def drawXor(pair_dist, channelSpacing, xorRadius, a_val, b_val, isLast):
    global codeDraw
    #draw XOR gate
    codeDraw.color("red" if isLast else "blue")
    codeDraw.write(a_val)
    codeDraw.setheading(270)
    codeDraw.pendown()
    codeDraw.forward(xorRadius)
    codeDraw.setheading(0)
    codeDraw.color("purple")
    codeDraw.circle(xorRadius)
    codeDraw.setheading(270)
    codeDraw.forward(xorRadius)
    codeDraw.setheading(0)
    codeDraw.forward(xorRadius)
    codeDraw.backward(xorRadius * 2)
    codeDraw.forward(xorRadius)

    #draw connector
    codeDraw.setheading(180)
    codeDraw.forward(pair_dist * channelSpacing)
    codeDraw.setheading(90)
    codeDraw.forward(xorRadius)
    codeDraw.color("red" if isLast else "blue")
    codeDraw.write(b_val)
    codeDraw.color("purple")
    codeDraw.backward(xorRadius)
    codeDraw.setheading(180)
    codeDraw.backward(pair_dist * channelSpacing)
    codeDraw.penup()

# encodes the given array of bits using the polar encoding algorithm
def polarEncodeSimple(data):

    #calculate number of elements and number of branches
    n = len(data)
    log2_result = math.log(n, 2)
    remainder = math.modf(log2_result)[0]
    
    if remainder != 0.0:
        print("Length of data is not power of 2!")
        return

    branches = range(int(log2_result))
    encoded_bits = [bit for bit in data]

    branch_dists = [2**x for x in branches]
    for pair_dist in branch_dists:
        is_paired = [False] * n #create a pairing boolean array for easy determination of which channels need to be paired
        for bit_idx in range(0, n):
            if is_paired[bit_idx] is False:
                #if this one hasnt been paired yet, pair it with the channel pair_dist below it
                pair_idx = bit_idx + pair_dist
                encoded_bits[bit_idx] ^= encoded_bits[pair_idx]
                
                is_paired[bit_idx] = True
                is_paired[pair_idx] = True

    return encoded_bits

# calculates the p(0) and p(1) for a given value, based on the sigma value of the distribution
def probCalc(sigma, y):
    p_x = (0.5 * math.erfc(y / math.sqrt(2.0)))
    return (p_x, 1-(p_x))

# calculates sigma (standard deviation) for a given SNR
def snrToSigma(snr_db):
    return math.sqrt(10**(-snr_db/10.0))

# converts an array of data bits into an array of values post-channel
# (values centerered around BPSK -1s and 1s)
def awgnChannelSimulation(snr_db, data):
    sigma = snrToSigma(snr_db)
    bpsk = [-1 if x == 0 else 1 for x in data]
    noise = [numpy.random.randn() * sigma for bit in data]
    return [bpsk[idx] + noise[idx] for idx in range(len(bpsk))]

# converts an array of data bits into two arrays of likelihood values post-channel
# (i.e., the probabilities calculated after receiving the post-channel bits)
# output[0] is the probability of 0 for each bit,
# output[1] is the probability of 1 for each bit
def awgnChannelProbabilities(snr_db, data):
    data_through_channel = awgnChannelSimulation(snr_db, data)
    sigma = snrToSigma(snr_db)
    p0 = []
    p1 = []
    for bit_idx in range(len(data)):
        probs = probCalc(sigma, data_through_channel[bit_idx])
        p0.append(probs[0])
        p1.append(probs[1])
    return p0, p1
		
# draws the decoder of the polar code
# (currently not completely implemented)
def polarDecodeDraw(data, frozen_bits, snr, draw=False):
    n = len(data)
    log2_result = math.log(n, 2)
    remainder = math.modf(log2_result)[0]
    
    if remainder != 0.0:
        print("Length of data is not power of 2!")
        return
    
    branches = list(reversed(range(int(log2_result))))
    
    probs = awgnChannelProbabilities(snr, data)
    decodeArray = [{'p0': probs[0][bit_idx], 'p1': probs[1][bit_idx], 'is_frozen': frozen_bits[bit_idx]} for bit_idx in range(n)]


    #create variables that will be referenced whether or not we draw
    initialX = 0
    initialY = 0
    channelSpacing = 0
    branchSpacing = 0
    branchAdjustment = 0
    xorRadius = 0
    if draw == False:
        #close screen if we're not drawing
        #drawScreen.bye()
        pass
    else:
        #calculate ratios of the diagram based on number of elements
        channelSpacing = drawScreen.window_height() / n
        branchSpacing = channelLength / (2 * log2_result)
        branchAdjustment = branchSpacing / (n / 2)
        xorRadius = branchSpacing / (2 * log2_result)
        initialX = codeDraw.pos()[0] + branchSpacing + channelLength
        initialY = codeDraw.pos()[1] - channelSpacing/2        

    y_pos = initialY
    x_start = initialX

    
    for pair_dist in [2**x for x in branches]:
        print(pair_dist)
        y_pos = initialY
        x_start += branchSpacing #increment x starting point by which branch we're on
        orig_x_start = x_start #save off original x starting point for this branch in case we need to nudge stuff over for overlapping XORs
        is_paired = [False] * n #create a pairing boolean array for easy determination of which channels need to be paired
        for bit_idx in range(0, n):
            if draw:
                #if we're drawing, go to the starting position for this branch XOR
                codeDraw.penup()
                codeDraw.goto(x_start, y_pos)
                codeDraw.pendown()
            if is_paired[bit_idx] is False:
                #if this one hasnt been paired yet, pair it with the channel pair_dist below it
                pair_idx = bit_idx + pair_dist
                a = decodeArray[bit_idx]
                b = decodeArray[pair_idx]
                a0 = (a['p0'] * b['p0']) + (a['p1'] * b['p1'])
                a1 = (a['p1'] * b['p0']) + (a['p0'] * b['p1'])
                print(a0)
                print(a1)
                if a0 > a1:
                    normalizer = a0
                    b0 = a['p0'] * b['p0']
                    b1 = a['p1'] * b['p1']
                else:
                    normalizer = a1
                    b0 = a['p1'] * b['p0']
                    b1 = a['p0'] * b['p1']
                #print(normalizer)
                a['p0'] = a0 / normalizer
                a['p1'] = a1 / normalizer
                b['p0'] = b0 / normalizer
                b['p1'] = b1 / normalizer
                
                if draw:
                    if True in is_paired[bit_idx:pair_idx]:
                        #slice the array to see if any have already been paired below this one. if they have,
                        # then we should nudge the x start over so that the XORs dont overlap entirely
                        x_start += branchAdjustment
                        codeDraw.penup()
                        codeDraw.goto(x_start, y_pos)
                        codeDraw.pendown()
                    else:
                        #if there arent any overlapping, use the original starting X for this branch
                        x_start = orig_x_start
                        codeDraw.penup()
                        codeDraw.goto(orig_x_start, y_pos)
                        codeDraw.pendown()
                    #draw an XOR using a function
                    drawXor(pair_dist, channelSpacing, xorRadius, encoded_bits[bit_idx], encoded_bits[pair_idx])
    
                is_paired[bit_idx] = True
                is_paired[pair_idx] = True
            y_pos -= channelSpacing #jump down some space for each channel

    result = []
    for bit in decodeArray:
        if bit['is_frozen'] or bit['p0'] > bit['p1']:
            result.append(0)
        else:
            result.append(1)
    codeDraw.penup
    return result

# basic successive cancellation decoder for polar codes,
# using an array of probabilities of 0 and 1, and
# an array indicating which bits are frozen
def polarDecodeSimple(probabilities_0, probabilities_1, frozen_bits, normalize=False):
    n = len(probabilities_0)
    log2_result = math.log(n, 2)
    remainder = math.modf(log2_result)[0]
    
    if remainder != 0.0:
        print("Length of data is not power of 2!")
        return
    
    try:
        drawScreen.bye()
    except:
        print("turtle window already closed!"  )
    branches = list(reversed(range(int(log2_result))))
    for pair_dist in [2**x for x in branches]:
        #print(pair_dist)
        is_paired = [False] * n #create a pairing boolean array for easy determination of which channels need to be paired
        for bit_idx in range(0, n):
            if is_paired[bit_idx] is False:            
                pair_idx = bit_idx + pair_dist
                p0 = probabilities_0[bit_idx]
                p1 = probabilities_1[bit_idx]
                q0 = probabilities_0[pair_idx]
                q1 = probabilities_1[pair_idx]
                a0 = (p0 * q0) + (p1 * q1)
                a1 = (p0 * q1) + (p1 * q0)
                #print(a0)
                #print(a1)
                if a0 > a1:
                    normalizer = a0 if normalize else 1.0
                    b0 = (p0 * q0)
                    b1 = (p1 * q1)
                else:
                    normalizer = a1 if normalize else 1.0
                    b0 = (p1 * q0)
                    b1 = (p0 * q1)

                probabilities_0[bit_idx] = a0 / normalizer
                probabilities_1[bit_idx] = a1 / normalizer
                probabilities_0[pair_idx] = b0 / normalizer
                probabilities_1[pair_idx] = b1 / normalizer

                print("new p(bit_idx):", probabilities_0[bit_idx], probabilities_1[bit_idx])
                print("new p(pair_idx):", probabilities_0[pair_idx], probabilities_1[pair_idx])
                is_paired[bit_idx] = True
                is_paired[pair_idx] = True
                
    result = []
    for bit_idx in range(n):
        print(probabilities_0[bit_idx], probabilities_1[bit_idx])
        if frozen_bits[bit_idx] or probabilities_0[bit_idx] > probabilities_1[bit_idx]:
            result.append(0)
        else:
            result.append(1)

    return result

# decodes a given polar encoded array of bits
# by simulating an AWGN channel of given SNR.
# if normalize is off, the probabilities will get smaller at each branch,
# so it may not work well for large code sizes
def polarDecodeWithAwgnSimulation(data, frozen_bits, snr, normalize=False):
    n = len(data)
    log2_result = math.log(n, 2)
    remainder = math.modf(log2_result)[0]
    
    if remainder != 0.0:
        print("Length of data is not power of 2!")
        return
        
    branches = list(reversed(range(int(log2_result))))
    
    sigma = snrToSigma(snr)
    decodeArray = []
    data_through_channel = awgnChannelSimulation(snr, data)
    for bit_idx in range(n):
        probs = probCalc(sigma, data_through_channel[bit_idx])
        decodeArray.append({'p0': probs[0], 'p1': probs[1], 'is_frozen': frozen_bits[bit_idx]})    

    #print [bit['p0'] for bit in decodeArray]
    #print [bit['p1'] for bit in decodeArray]
    for pair_dist in [2**x for x in branches]:
        is_paired = [False] * n #create a pairing boolean array for easy determination of which channels need to be paired
        for bit_idx in range(0, n):
            if is_paired[bit_idx] is False:
                #if this one hasnt been paired yet, pair it with the channel pair_dist below it
                pair_idx = bit_idx + pair_dist
                a = decodeArray[bit_idx]
                b = decodeArray[pair_idx]
                a0 = (a['p0'] * b['p0']) + (a['p1'] * b['p1'])
                a1 = (a['p1'] * b['p0']) + (a['p0'] * b['p1'])
                #print(a0)
                #print(a1)
                if a0 > a1:
                    normalizer = a0 if normalize else 1.0
                    b0 = a['p0'] * b['p0']
                    b1 = a['p1'] * b['p1']
                else:
                    normalizer = a1 if normalize else 1.0
                    b0 = a['p1'] * b['p0']
                    b1 = a['p0'] * b['p1']
                #print(normalizer)
                a['p0'] = a0 / normalizer
                a['p1'] = a1 / normalizer
                b['p0'] = b0 / normalizer
                b['p1'] = b1 / normalizer
                
                is_paired[bit_idx] = True
                is_paired[pair_idx] = True
    #print([bit['p0'] for bit in decodeArray])
    #print([bit['p1'] for bit in decodeArray])
    result = []
    for bit in decodeArray:
        if bit['is_frozen'] or bit['p0'] > bit['p1']:
            result.append(0)
        else:
            result.append(1)

    return result


# calculated bit error values and consequent polar code selections,
# using a monte carlo simulation of 10**6 iterations
FROZEN_BITS_8_P_ERR = [0.422735, 0.303542, 0.303483, 0.186775, 0.303059, 0.186728, 0.185962, 0.103885]
FROZEN_BITS_8 = [True, True, True, False, True, False, False, False]

FROZEN_BITS_16_P_ERR = [0.488525, 0.423311, 0.423384, 0.303654, 0.422837, 0.303666, 0.303726, 0.186091, 0.422407, 0.303859, 0.303713, 0.187075, 0.303421, 0.186785, 0.187094, 0.104235]
FROZEN_BITS_16 = [True, True, True, False, True, False, True, False, True, True, True, False, False, False, False, False]

FROZEN_BITS_32_P_ERR = [0.499346, 0.489272, 0.488065, 0.422128, 0.487312, 0.423283, 0.422818, 0.303813, 0.488894, 0.423109, 0.422943, 0.303427, 0.422951, 0.304058, 0.303298, 0.186733, 0.487901, 0.423169, 0.422068, 0.303577, 0.42241, 0.304413, 0.303633, 0.186656, 0.422874, 0.303323, 0.302576, 0.185929, 0.303294, 0.186484, 0.186466, 0.103778]
FROZEN_BITS_32 = [True, True, True, True, True, True, True, False, True, True, True, False, True, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False]

FROZEN_BITS_1024_P_ERR = [0.500509, 0.499669, 0.499, 0.499701, 0.500511, 0.49976, 0.500016, 0.500015, 0.500221, 0.500155, 0.500729, 0.499059, 0.499321, 0.49979, 0.499749, 0.500167, 0.500438, 0.499452, 0.499912, 0.500505, 0.499794, 0.49983, 0.499716, 0.500158, 0.500105, 0.500512, 0.499471, 0.500938, 0.499953, 0.500087, 0.500533, 0.499143, 0.500418, 0.499605, 0.500017, 0.499623, 0.500269, 0.498898, 0.498887, 0.500209, 0.499048, 0.500249, 0.499678, 0.501095, 0.499075, 0.501317, 0.500008, 0.499206, 0.500918, 0.500394, 0.500783, 0.500501, 0.499907, 0.500716, 0.499592, 0.499125, 0.499519, 0.500388, 0.499743, 0.500188, 0.500436, 0.499192, 0.500622, 0.488066, 0.499656, 0.500209, 0.49983, 0.49924, 0.49911, 0.499848, 0.500608, 0.500527, 0.499468, 0.499959, 0.499654, 0.499675, 0.500469, 0.499888, 0.500144, 0.499598, 0.499564, 0.498957, 0.500128, 0.500165, 0.499731, 0.50053, 0.499976, 0.500145, 0.500577, 0.500752, 0.499886, 0.499906, 0.499017, 0.498779, 0.498638, 0.488894, 0.500949, 0.499364, 0.500515, 0.49958, 0.500301, 0.499631, 0.499879, 0.49983, 0.500026, 0.5, 0.499999, 0.500237, 0.500303, 0.499442, 0.500078, 0.488233, 0.500831, 0.500377, 0.500373, 0.499717, 0.499631, 0.499672, 0.500257, 0.488258, 0.500338, 0.49987, 0.499847, 0.488093, 0.500725, 0.487264, 0.488494, 0.422523, 0.500225, 0.499789, 0.50175, 0.500969, 0.500373, 0.500308, 0.50044, 0.499708, 0.499648, 0.500047, 0.499798, 0.500024, 0.499524, 0.500427, 0.499408, 0.499813, 0.500362, 0.500599, 0.500302, 0.499931, 0.500574, 0.500424, 0.500533, 0.5003, 0.499941, 0.499291, 0.50002, 0.498659, 0.5001, 0.499769, 0.499931, 0.487371, 0.500355, 0.500501, 0.499012, 0.499735, 0.500521, 0.500549, 0.500354, 0.499599, 0.500032, 0.500518, 0.499222, 0.500643, 0.500361, 0.500718, 0.50004, 0.487265, 0.499155, 0.499745, 0.500776, 0.499659, 0.500457, 0.499806, 0.499956, 0.487773, 0.500079, 0.5004, 0.50026, 0.487999, 0.499526, 0.488343, 0.488421, 0.422485, 0.500095, 0.500752, 0.499937, 0.500539, 0.501441, 0.499676, 0.499603, 0.499221, 0.500876, 0.49978, 0.499129, 0.499593, 0.500034, 0.499866, 0.499972, 0.488551, 0.500656, 0.500338, 0.500792, 0.500452, 0.500293, 0.500312, 0.50015, 0.489032, 0.499785, 0.499677, 0.498559, 0.488527, 0.499457, 0.487545, 0.48854, 0.42279, 0.499338, 0.500476, 0.499896, 0.499534, 0.498868, 0.499429, 0.499775, 0.4887, 0.499677, 0.499367, 0.498983, 0.488332, 0.499547, 0.488279, 0.488182, 0.423428, 0.499404, 0.499234, 0.4987, 0.487203, 0.500482, 0.487971, 0.487658, 0.422857, 0.499729, 0.487299, 0.487809, 0.422327, 0.487893, 0.422178, 0.422245, 0.303315, 0.500215, 0.499724, 0.500275, 0.499997, 0.499793, 0.498913, 0.500534, 0.498988, 0.500059, 0.49978, 0.500221, 0.49983, 0.500031, 0.499206, 0.500544, 0.500342, 0.500028, 0.500148, 0.500275, 0.500662, 0.500248, 0.500605, 0.500256, 0.500486, 0.499014, 0.499612, 0.499591, 0.500056, 0.498496, 0.49986, 0.499729, 0.487061, 0.499135, 0.499724, 0.499326, 0.50061, 0.499734, 0.49902, 0.499853, 0.49914, 0.499538, 0.500374, 0.500224, 0.499949, 0.499665, 0.499619, 0.499537, 0.48816, 0.499904, 0.50005, 0.500001, 0.499531, 0.499327, 0.499318, 0.499729, 0.488927, 0.499734, 0.499666, 0.499873, 0.487569, 0.499529, 0.486744, 0.487952, 0.422292, 0.500522, 0.499731, 0.499736, 0.49985, 0.499118, 0.499558, 0.499879, 0.499631, 0.500737, 0.501046, 0.499788, 0.499844, 0.50029, 0.499157, 0.499673, 0.488265, 0.499806, 0.499812, 0.500454, 0.500202, 0.499324, 0.499716, 0.499979, 0.487278, 0.49996, 0.500498, 0.499116, 0.488031, 0.500255, 0.488417, 0.487813, 0.422868, 0.500311, 0.500248, 0.500078, 0.500376, 0.499944, 0.500385, 0.499566, 0.487782, 0.500784, 0.499194, 0.49948, 0.488136, 0.499262, 0.488044, 0.488464, 0.422459, 0.499311, 0.500498, 0.498986, 0.487859, 0.500083, 0.488628, 0.487759, 0.422605, 0.500276, 0.488081, 0.488139, 0.421776, 0.487847, 0.421954, 0.423035, 0.303143, 0.500186, 0.499805, 0.499827, 0.499129, 0.50039, 0.499453, 0.49985, 0.499699, 0.499942, 0.500595, 0.500184, 0.49863, 0.500693, 0.499605, 0.499055, 0.488419, 0.50041, 0.50044, 0.499444, 0.500436, 0.500609, 0.500435, 0.500486, 0.488322, 0.499891, 0.498647, 0.499652, 0.488382, 0.499445, 0.487466, 0.487927, 0.422248, 0.498741, 0.499716, 0.500211, 0.499306, 0.500307, 0.498809, 0.499484, 0.48887, 0.499713, 0.498588, 0.499943, 0.488376, 0.499333, 0.488088, 0.487831, 0.422041, 0.499816, 0.500143, 0.499962, 0.489063, 0.500008, 0.487192, 0.487578, 0.422642, 0.500553, 0.488144, 0.488223, 0.422983, 0.487981, 0.421808, 0.422669, 0.302675, 0.500286, 0.499406, 0.500078, 0.498795, 0.49923, 0.499439, 0.499296, 0.487932, 0.500654, 0.499665, 0.500292, 0.488044, 0.49922, 0.486979, 0.488597, 0.422894, 0.500072, 0.499552, 0.50007, 0.486294, 0.499905, 0.487709, 0.488308, 0.422618, 0.500466, 0.488373, 0.488035, 0.422666, 0.488818, 0.422505, 0.423286, 0.303009, 0.499395, 0.499544, 0.500135, 0.488125, 0.499356, 0.48807, 0.488507, 0.423333, 0.50027, 0.488034, 0.488217, 0.42299, 0.487688, 0.422252, 0.422957, 0.303772, 0.49926, 0.487602, 0.487893, 0.422378, 0.488064, 0.42207, 0.421964, 0.303339, 0.487957, 0.422137, 0.422895, 0.303162, 0.422402, 0.302315, 0.303561, 0.18639, 0.499629, 0.500356, 0.499488, 0.499124, 0.50058, 0.49952, 0.499813, 0.500402, 0.500815, 0.50019, 0.50027, 0.499509, 0.500087, 0.50025, 0.499821, 0.501064, 0.500033, 0.499982, 0.49939, 0.500185, 0.500517, 0.499561, 0.500096, 0.499746, 0.499667, 0.50018, 0.49941, 0.49944, 0.500271, 0.499131, 0.500632, 0.487649, 0.500191, 0.499686, 0.500324, 0.499704, 0.500686, 0.499688, 0.499271, 0.500497, 0.500227, 0.499587, 0.499932, 0.500169, 0.499649, 0.499512, 0.500059, 0.487468, 0.499263, 0.5002, 0.500524, 0.50081, 0.499804, 0.499774, 0.499787, 0.487762, 0.49933, 0.499503, 0.49906, 0.488103, 0.499569, 0.487558, 0.48799, 0.421608, 0.500103, 0.499495, 0.501056, 0.499738, 0.500276, 0.50005, 0.50018, 0.499273, 0.499777, 0.500204, 0.499879, 0.49958, 0.499901, 0.499275, 0.500074, 0.488136, 0.500155, 0.500207, 0.499917, 0.499436, 0.499751, 0.500102, 0.50038, 0.48782, 0.500071, 0.49989, 0.499486, 0.488045, 0.49999, 0.487425, 0.48762, 0.42295, 0.500089, 0.500614, 0.499667, 0.500349, 0.500137, 0.500477, 0.499975, 0.487477, 0.500467, 0.499792, 0.500024, 0.488643, 0.49969, 0.488496, 0.487329, 0.422333, 0.499764, 0.499597, 0.500396, 0.487385, 0.500364, 0.488041, 0.48771, 0.422951, 0.499707, 0.488425, 0.488178, 0.422363, 0.488063, 0.422509, 0.422579, 0.303324, 0.500226, 0.499441, 0.500693, 0.499967, 0.500742, 0.500037, 0.499307, 0.499501, 0.499615, 0.499617, 0.500553, 0.499761, 0.499839, 0.499462, 0.498743, 0.487223, 0.500435, 0.499376, 0.499756, 0.50012, 0.499818, 0.500451, 0.49971, 0.488654, 0.499967, 0.500001, 0.499838, 0.488502, 0.500034, 0.488216, 0.488081, 0.423342, 0.500002, 0.499714, 0.500066, 0.498787, 0.49898, 0.500017, 0.500051, 0.48838, 0.500669, 0.500547, 0.49937, 0.488473, 0.498968, 0.487597, 0.488031, 0.422561, 0.499699, 0.500152, 0.499865, 0.488381, 0.49964, 0.488344, 0.487721, 0.4229, 0.499886, 0.488782, 0.488434, 0.422533, 0.487577, 0.422261, 0.422802, 0.303287, 0.501148, 0.500404, 0.500543, 0.499264, 0.499577, 0.50036, 0.499633, 0.487877, 0.50057, 0.500092, 0.499746, 0.487003, 0.498887, 0.487954, 0.487737, 0.42296, 0.499412, 0.499804, 0.498787, 0.487816, 0.5003, 0.488126, 0.487643, 0.423137, 0.499893, 0.487939, 0.487899, 0.422575, 0.488619, 0.423112, 0.423419, 0.303936, 0.500143, 0.499707, 0.49938, 0.488716, 0.498692, 0.48789, 0.488085, 0.422496, 0.50048, 0.487995, 0.488353, 0.423159, 0.48843, 0.423666, 0.422352, 0.303503, 0.500231, 0.487772, 0.487256, 0.423329, 0.487179, 0.422311, 0.423556, 0.303977, 0.487845, 0.422369, 0.421789, 0.302619, 0.422364, 0.303116, 0.303032, 0.186512, 0.500195, 0.499827, 0.500094, 0.500001, 0.499814, 0.500225, 0.499798, 0.500067, 0.501263, 0.500522, 0.500252, 0.4996, 0.500007, 0.500108, 0.499439, 0.488049, 0.5003, 0.500204, 0.499287, 0.499665, 0.49963, 0.499592, 0.498985, 0.488144, 0.499767, 0.499952, 0.499728, 0.488664, 0.498955, 0.488033, 0.487924, 0.421976, 0.498543, 0.4994, 0.499797, 0.499818, 0.499304, 0.499975, 0.500033, 0.488158, 0.500688, 0.498878, 0.499155, 0.488796, 0.500267, 0.488578, 0.487821, 0.423066, 0.49933, 0.500031, 0.499694, 0.488222, 0.499548, 0.488555, 0.486736, 0.422732, 0.500074, 0.487641, 0.487914, 0.422917, 0.488161, 0.422614, 0.422295, 0.30294, 0.49991, 0.500599, 0.50049, 0.499613, 0.499465, 0.500425, 0.499591, 0.488428, 0.499375, 0.500023, 0.499416, 0.488033, 0.499376, 0.487696, 0.487967, 0.422148, 0.500221, 0.499551, 0.499912, 0.487595, 0.500585, 0.487319, 0.487421, 0.422349, 0.499529, 0.487909, 0.488419, 0.422258, 0.487541, 0.422568, 0.422391, 0.302659, 0.500556, 0.500294, 0.499759, 0.4875, 0.499712, 0.48836, 0.487534, 0.422866, 0.499231, 0.488814, 0.488413, 0.423121, 0.488293, 0.423718, 0.422973, 0.303798, 0.498989, 0.488652, 0.487653, 0.423347, 0.48881, 0.422935, 0.423047, 0.303526, 0.487222, 0.422411, 0.421635, 0.303475, 0.423006, 0.302977, 0.303784, 0.186276, 0.500121, 0.499483, 0.499855, 0.500812, 0.500177, 0.5004, 0.499879, 0.487543, 0.499582, 0.499954, 0.499859, 0.488058, 0.499341, 0.488159, 0.488625, 0.422929, 0.499678, 0.499853, 0.500342, 0.487487, 0.499552, 0.489201, 0.488248, 0.422844, 0.500016, 0.487899, 0.488997, 0.422417, 0.488098, 0.422152, 0.422612, 0.303061, 0.49996, 0.499757, 0.499633, 0.487545, 0.500122, 0.487793, 0.488221, 0.423055, 0.499702, 0.489136, 0.487991, 0.423488, 0.487572, 0.422659, 0.42248, 0.303591, 0.500068, 0.487899, 0.487686, 0.422944, 0.488146, 0.422961, 0.422563, 0.303462, 0.487653, 0.422506, 0.422696, 0.303105, 0.423525, 0.302928, 0.303564, 0.186312, 0.499959, 0.500967, 0.499388, 0.487607, 0.500178, 0.48766, 0.488248, 0.422593, 0.499564, 0.488085, 0.488535, 0.422444, 0.487667, 0.422553, 0.422431, 0.303552, 0.500542, 0.488015, 0.488526, 0.422305, 0.48853, 0.422788, 0.422463, 0.303874, 0.488226, 0.422934, 0.422569, 0.30228, 0.423094, 0.303257, 0.303354, 0.186097, 0.499867, 0.487776, 0.487735, 0.422687, 0.488222, 0.421697, 0.423122, 0.303372, 0.487888, 0.422817, 0.42259, 0.303274, 0.422217, 0.303609, 0.303825, 0.186644, 0.487568, 0.422726, 0.422721, 0.303431, 0.422122, 0.303302, 0.303649, 0.18664, 0.422007, 0.303105, 0.302881, 0.185911, 0.302848, 0.18591, 0.186456, 0.103874]
FROZEN_BITS_1024 = [True, True, False, True, True, True, True, True, True, True, True, False, False, True, True, True, True, False, True, True, True, True, True, True, True, True, False, True, True, True, True, False, True, True, True, True, True, False, False, True, False, True, True, True, False, True, True, False, True, True, True, True, True, True, True, False, True, True, True, True, True, False, True, False, True, True, True, False, False, True, True, True, False, True, True, True, True, True, True, True, True, False, True, True, True, True, True, True, True, True, True, True, False, False, False, False, True, False, True, True, True, True, True, True, True, True, True, True, True, False, True, False, True, True, True, True, True, True, True, False, True, True, True, False, True, False, False, False, True, True, True, True, True, True, True, True, True, True, True, True, True, True, False, True, True, True, True, True, True, True, True, True, True, False, True, False, True, True, True, False, True, True, False, True, True, True, True, True, True, True, False, True, True, True, True, False, False, True, True, True, True, True, True, False, True, True, True, False, True, False, False, False, True, True, True, True, True, True, True, False, True, True, False, True, True, True, True, False, True, True, True, True, True, True, True, False, True, True, False, False, False, False, False, False, False, True, True, True, False, False, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, False, False, False, True, False, False, False, False, False, False, False, True, True, True, True, True, False, True, False, True, True, True, True, True, False, True, True, True, True, True, True, True, True, True, True, False, True, True, True, False, True, True, False, False, True, False, True, True, False, True, False, True, True, True, True, True, True, True, False, True, True, True, True, False, False, True, False, True, True, True, False, True, False, False, False, True, True, True, True, False, True, True, True, True, True, True, True, True, False, True, False, True, True, True, True, False, True, True, False, True, True, False, False, True, False, False, False, True, True, True, True, True, True, True, False, True, False, False, False, False, False, False, False, False, True, False, False, True, False, False, False, True, False, False, False, False, False, False, False, True, True, True, False, True, False, True, True, True, True, True, False, True, True, False, False, True, True, False, True, True, True, True, False, True, False, True, False, False, False, False, False, False, True, True, False, True, False, True, False, True, False, True, False, False, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, False, True, False, False, False, False, False, True, True, True, False, False, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, False, True, True, False, False, False, False, False, True, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, True, True, True, False, True, True, True, True, True, True, True, True, True, True, True, True, True, True, False, True, True, True, True, True, True, True, False, False, True, False, True, False, True, True, True, True, True, True, False, True, True, True, True, True, True, True, True, False, False, True, True, True, True, True, True, False, False, True, False, False, True, False, False, False, True, True, True, True, True, True, True, False, True, True, True, True, True, False, True, False, True, True, True, False, True, True, True, False, True, True, True, False, True, False, False, False, True, True, True, True, True, True, True, False, True, True, True, False, True, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, False, True, True, True, True, False, True, True, True, True, True, True, False, False, False, True, False, True, True, True, True, True, False, True, True, True, False, True, False, False, False, True, True, True, False, False, True, True, False, True, True, False, False, False, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, True, True, False, True, True, True, False, True, True, True, False, False, False, False, False, False, True, False, False, True, False, False, False, True, False, False, False, False, False, False, False, True, True, False, False, False, False, False, False, True, False, False, False, False, False, False, False, True, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, True, True, True, True, True, True, True, True, True, True, True, True, True, True, False, False, True, True, False, True, True, True, False, False, True, True, True, False, False, False, False, False, False, False, True, True, False, True, True, False, True, False, False, False, True, False, False, False, False, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, True, True, True, False, True, True, False, False, True, False, False, False, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, True, True, False, True, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, True, True, True, True, True, True, True, False, True, True, True, False, False, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, True, True, False, True, False, False, False, True, False, False, False, False, False, False, False, True, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, True, True, False, False, True, False, False, False, True, False, False, False, False, False, False, False, True, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, True, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False]
