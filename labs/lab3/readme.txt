ENSE 481 - Lab 3 - Timing sin/cos Implementations

Run the "trigtest" command in the console.

The goal of this lab is to utilize some of the codes we’ve encountered when implementing fast integer-based
sine and cosine. You will investigate the performance and accuracy of these:
1. Integer Taylor series implementation with 4 terms, as described in Crenshaw, ch5, Listing 5.6, p118
2. The built-in sine and cosine functions from the standard math library (which might be extremely slow
on the nucleo-f103rb, as they are software-only, only measurement will tell!)

Overview
Using nucleo-f103rb even though it sucks and doesn't have a FPU which will soundly beat any software tricks
for accuracy and speed.

Requirements
1. You shall compare the implementations with respect to speed.
    Using timer clock cycles.  I tested the average, max, and min.

2. You shall compare the implementations with respect to accuracy. The built-in math library’s sine and
cosine functions can be regarded as the “gold-standard” against which integer-based methods can be
judged.
    Compared and got error results in parts per million (PPM) as an average, max and min.

3. Your system shall be well documented; well enough so that anyone could repeat your measurements
and get the same results. Describe the conditions under which you made your measurements. Describe
any necessary configuration of the hardware or software environment. Draw schematics when necessary.
Document the toolchain switches, etc.  Try to automate the gathering of measurements as much as possible.
    Using the 14.2.1 toolchain and the internal oscillator with a 64MHz main clock in CubeIDE.

4 What To Submit
Push all your code to your git repository. Provide a readme file which talks about what you did, any issues
you encountered, and any extensions you made or special techniques you used.
    The Taylor series code from the reference text is in fixed point so I had to convert the input to
    fixed point.  Sometimes I have to flash the code 2 or 3 times before it takes it correctly.
    Maybe some problem with CubeIDE or a failing cable.

