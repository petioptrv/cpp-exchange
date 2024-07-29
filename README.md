# CPP Exchange

This repo uses [ModernCppStarter](MODERN_CPP_STARTER_README.md).

This repo is my non-soy attempt to implement an exchange in C++. The goal is to gain practical experience
with low-latency applications and optimization. 

I will rely heavily on the [Building Low Latency Applications with C++](https://www.oreilly.com/library/view/building-low-latency/9781837639359/)
(BLLA) book. However, in order to maximize learning, I will be taking the approach of first coding the components
myself, commiting my initial implementation, then going through the material of the book, applying the techniques
exposed in it, and finally making a new commit that contains all the updates and the reasons behind them.

## Techniques and Optimization Considerations

### Macros

Extensive use of the `UNLIKELY` macro to optimize **branch predictions**, and the `ASSERT` macro to make the code safer.

### Memory Pool

My initial implementation used `deque` to dynamically allocate and never deallocate memory for the order objects.
This approach is memory efficient as it will allocate up to the maximum amount of memory used at peak times and
reuse memory after that. However, low-latency applications are not concerned with memory efficiency nearly as
much as they are with read/write performance. This means that an approach of pre-allocating a large chunk of contiguous
memory and using [placement new](https://en.cppreference.com/w/cpp/language/new) will optimize the initial memory
allocations (effectively remove them), and will ensure better **cache performance** through memory spatial locality.

## Orders and Order Book

As suggested in the book, I performed an analysis on the [memory alignment](/standalone/investigation/alignment.cpp)
of my order objects, and the initial implementation passes the checks.
