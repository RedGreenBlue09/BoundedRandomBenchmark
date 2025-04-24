
# What is this?

This benchmarks various bounded random integer methods.

# Background

I needed to implement a bounded random function for use in the array shuffle function. 
I went across [an article from PCG Random](https://www.pcg-random.org/posts/bounded-rands.html) 
that showcase various algorithms to do this along with benchmarks of them.

Recently, I found [another algorithm](https://github.com/swiftlang/swift/pull/39143) that claims to be "optimal", 
so I carried out this benchmark to see if that is true in term of performance and RNG call count.

# Bounded random algorithms

+ Bitmask
+ Short Product (the Swift PR)
+ Multiply
+ Multiply 2 (Optimized Multiply)
+ Modulo
+ Modulo 2 (Optimized Modulo)

# Benchmark method

The bounded random algorithms are run 100 million times in different situations.  
The input range is random but using a different seed.  
The result is discarded as I don't plan to test the quality of the output.

The following cases are covered:

+ Large (full) range & small range (0 - 1023)
+ Fast RNG and slow RNG (fast RNG with extra useless instructions)
+ 32-bit and 64-bit RNG
+ CPU: IA-32, AMD64, ARMv7

The RNGs are the xoshiro family.

Due to abstractions to make the code shorter, the 64-bit algorithms are not inlined when compiled to 32-bit code. 
However, the overhead of 64-bit math dominates the overhead of function pointers, 
so the results still show a clear trend among all algorithms.

# Results

The graphs and statistics are in the Result foler.

#### IA-32 & AMD64

CPU: Intel Core i7-7700HQ  
Compiled using [Clang-Cl 19.1.1](https://releases.llvm.org/19.1.0/tools/clang/docs/MSVCCompatibility.html)

```
clang-cl /O2 /MD /Zi -fuse-ld=lld -flto Main.c
```

#### ARMv7

CPU: Qualcomm Snapdragon 800 @ 2.15 Ghz  
Compiled using [Clang 19.1.3](https://github.com/mstorsjo/llvm-mingw/releases/tag/20241030)

```
gcc -O3 -g -mcpu=cortex-a15 -mfpu=neon-vfpv4 -fvectorize -munaligned-access -flto -Wl,--pdb= Main.c
```

## Short Product

This algorithm does quite well, but it isn't "optimal" at all.

#### Speed

In the large range test, it performs much worse than Bitmask algorithm (40% slower), 
and is only slightly faster than the Optimized Multiply algorithm. 
For the small range test, it ties with the Optimized Multiply method, being the fastest algorithm.

It is usually the slowest algorithm when the RNG is slow.

Basically, there are algorithms that are on par with or faster than this one in every situation, 
and they are much easier to understand.

#### RNG call count

The most disappointing result is that it has the highest RNG call count of all algorithms, in the large range test.
In the small range test, it doesn't make any extra calls just like the other algorithms (except Bitmask).

This essentially disputes the authors' claim that it 
"achieves a theoretically optimal bound on the amount of randomness consumed to generate a sample".

#### Implementation

The logic is hard to understand and it does not completely eliminate bias (the chance is low enough for you to ignore it).

For 64-bit RNG, it requires the high result of 64-bit by 64-bit multiplication. 
You can use `__int128` with GCC, `_umul128` with MSVC or implement this yourself with performance penalty.

## Bitmask

#### Speed

This algorithm beats every other algorithm by a wide margin in the large range test. 
However, it cannot scale down in the small range test like the other algorithms. 
Short Product or Optimized Multiply is much faster in this case.

#### RNG call count

On average, this one calls the RNG the most (still less than Short Product in case of large range). 
In the small range test, while other algorithms don't discard anything from the RNG, 
this one still discards the same amount that it does in the large range test (about 28% of calls).

#### Implementation

The logic is simple, it just removes bits.

You need to implement the count leading zero function either by yourself (with performance penalty) or use compiler intrinsics. 

## Optimized Multiply

This algorithm is well balanced. It performs decently in the large range test and is super fast in the small range test.

#### Implementation

Simple logic. The optimized version is complex but understandable.

For 64-bit RNG, it requires the high result of 64-bit by 64-bit multiplication. 
You can use `__int128` with GCC, `_umul128` with MSVC or implement this yourself with performance penalty.

## Modulo

It is the slowest one. The optimized version is faster for 64-bit RNG but is slower for 32-bit RNG.

#### Implementation

Simple logic. The optimized version is complex but understandable.

It does not require any special math function.

# Sources & references

1. https://www.pcg-random.org/posts/bounded-rands.html
2. https://github.com/imneme/bounded-rands/blob/master/bounded64.cpp
3. https://github.com/swiftlang/swift/pull/39143
4. https://github.com/openssl/openssl/blob/openssl-3.5.0/crypto/rand/rand_uniform.c
5. https://prng.di.unimi.it/
