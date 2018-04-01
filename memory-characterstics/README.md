
# Overview
  - Compute the size of RAM
    - A plaform-independent C program that approximates the size of main memory (RAM). See [here](calc_ram_size.c)
  - Measure the characteristics of Cache
    -  A C program to measure the cache behavior using Saavadra and Smith's methodology. See [here](cache_charac.c). There are two graphs presented, [ArraySize(N) vs Avg Iteration Time](NvsAvgIterTime_Machine1.png), [Stride vs Avg Iteration Time](SvsAvgIterTime_Machine1.png) to describe the inferred cache parameters. The tests are performed on two different machines. The graphs are generated using a python program. See [here](plot.py).
 
# Compute the size of RAM
This program gives you an approximate size of your RAM. I have followed below strategy to get as close to correct RAM size.

In first iteration, I keep mapping the memory region using MMAP starting from 1024 bytes and increasing by power of 2 until I get MMAP failure. Meanwhile, I also store the last allocated space after every successfull MMAP call. Now, when I get the first MMAP failure, I go back to the last successfully allocated space and start mapping memory region starting from that and incrementing by 1024 bytes, which is the default increment counter. From then on, whenever there is a MMAP failure as previous I go back to last successfully allocated space but also reduce the increment counter by half. I repeat this process upto 10 failurs or until increment counter is positive.

The above approach allows the mapping of smallest of region if within the main memory, hence my last allocated space at the end becomes the RAM size of the machine.

# Measure the characteristics of Cache
