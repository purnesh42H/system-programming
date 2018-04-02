
# Overview
  - Compute the size of RAM
    - A plaform-independent C program that approximates the size of main memory (RAM). See [here](calc_ram_size.c)
  - Measure the characteristics of Cache
    -  A C program to measure the cache behavior using Saavadra and Smith's methodology. See [here](cache_charac.c). There are two graphs presented, [ArraySize(N) vs Avg Iteration Time](NvsAvgIterTime_Machine1.png), [Stride vs Avg Iteration Time](SvsAvgIterTime_Machine1.png) to describe the inferred cache parameters. The tests are performed on two different machines. The graphs are generated using a python program. See [here](plot.py).
    
# How to run
 - Run "make" to compile
 - Run "make check" to calculate the ram size
 - Run "make check1" to fetch the cache characterstics
    - Run plot.py to plot the graphs for values generated from "make check1"
 
# Compute the size of RAM
This program gives you an approximate size of your RAM. I have followed below strategy to get as close to correct RAM size.

In first iteration, I keep mapping the memory region using MMAP starting from 1024 bytes and increasing by power of 2 until I get MMAP failure. Meanwhile, I also store the last allocated space after every successfull MMAP call. Now, when I get the first MMAP failure, I go back to the last successfully allocated space and start mapping memory region starting from that and incrementing by 1024 bytes, which is the default increment counter. From then on, whenever there is a MMAP failure as previous I go back to last successfully allocated space but also reduce the increment counter by half. I repeat this process upto 10 failurs or until increment counter is positive.

The above approach allows the mapping of smallest of region if within the main memory, hence my last allocated space at the end becomes the RAM size of the machine.

# Measure the characteristics of Cache
The characterstics of the cache can be inferred easily from the two presented graph
- [ArraySize(N) vs Avg Iteration Time](NvsAvgIterTime_Machine1.png)
- [Stride vs Avg Iteration Time](SvsAvgIterTime_Machine1.png)

While the former help us in calculating the approximate size of the cache, the latter gives the information about cache line size, associativity and avg miss delay panalty.

## Cache Size
- Characterstics from graph [ArraySize(N) vs Avg Iteration Time](NvsAvgIterTime_Machine1.png)
  - It can be seen when the array size is small, the average iteration time is huge. It is because time to bring the array to cache dominates the overall iteration time. However, as the size increases, after the first cache miss, the susequent reads are fast as they are from the cache and hence the average iteration time decreases and becomes constant after a while as you keep increasing the array size. The reason for the constant low average iteration time is the fact that the cache is able to accomodate the entire array in the cache after the first miss. Again when the array size increases even more just big to not be accomadated in the cache at once, there would be a sudden increase in the average iteration time(till the one you see for array size 2 and 4) for subsequent reads after the cache limit is reached. For example for machine 1, the cache is able to accomodate the array size of 2^19 bytes.
 - Technique Used
  To plot the N vs Avg Iteration Time graph, I have kept the stride(=1) as constant and increased the array size by power of 2 upto 19. For each of the iteration I create a float array of given array size and assign random float numbers to it. Then I traverse that array and do following in every iteration:
    - Read the consecutive elements
    - Add the elements and store to the smaller index element
    - Print the sum
 
 - The duration of for the above operation time is recorded and added to elapsed time.
 - Finally I calculate the average elapsed time for the particular array size.
 
This technique helps to get the efficient result with sufficient accuracy as it gives the average iteration time for 20 array sizes for a constant stride. Also, I use the consecutive elements to add assuming the spatial locality for caching mechanism. Hence, there is no effect of stride in the iteration elapsed time. To avoid the loop overhead and compiler optimization, it's better to not only compute the sum of values read from the array, but also to print the final sum.

## Average Miss Delay and Cache Line Size
- Characterstics from graph [Stride vs Avg Iteration Time](SvsAvgIterTime_Machine1.png)
  - It can be seen that when the stride is small the average iteration time is low and keep on increasing with increase in stride. It means the the average iteration time is proportional to stride length. This is due to the fact that when the stride is low the number of cache misses are less but increases when the stride is high. This gives us two characterstics
    - Average Miss Delay - time difference between where there is no miss and when miss starts to occur in each iteration. This can be seen when the stride is 2^14
    - Cache line size - The rate of misses can be easily identified from the graph. The transition that occurs from few iterations having cache misses to every iteration having cache miss gives us the cache line size(See difference when strid is 2^13 and 2^14).
    
- Technique Used
  - I have kept the array size constant to 2^19 and increased the stride from 2 to 2^14. This has made sure that for the highest array size(which won't fit in cache line or atleast in first level cache) the average iteration time is totally dependent on stride length which affects the no. of cache misses or cache line misses. This has helped to see the above mentioned transitions and also made calculation for Average Miss Delay easy.
  
  Please note that I am doing the same operation in each iteration as mentioned in "Cache Size" section above.
  
## Associativity
- Characterstics from graph [Stride vs Avg Iteration Time](SvsAvgIterTime_Machine1.png)
  - Once the stride crosses the C/a boundry, where a is the number of sets in a cache, only a single set is referenced, and the number of elements mapping to this set starts to decrease. When the stride is maximum N(2^19)/2 only two elements are being map to the surviving set. Therefore, if at some point the effect of cache misses disappears, then the point where this occurs using the formula a = N/s gives us the associativity. Otherwise, it is obvious that the cache has to be
direct mapped. 

## Parameters of TLB
The phenomena observed when we consider the TLB is the same as for the cache; the only difference is in the particular values of N and s where the changes in behavior occur.
  
