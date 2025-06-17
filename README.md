# CPP Exchange

## ToDo's

- [ ] Check where is the order priority attribute used.
  - I suspect it's to return the information to the user.
- [ ] Currently, the price levels iterate over the orders linked-list to find the order to cancel.
  - This needs to be made faster using a hash map at some point in the process.
  - The book uses a map of client-ID to client-order-ids map (i.e. `client_map[cid][coid]`).
    - This approach puts the onus on the client to keep track of the IDs they have used in the range of allotted IDs
      - This is actually quite common, and I wonder if this speeds up the process for real exchanges too....
- [x] Check how are price levels made non-hackable.
  - The book's implementation is hackable (see [below](#price-hack)).
- [x] Verify why is the pointer to previous order important.
  - It's used to be able to easily add an order to the end of the queue
    (i.e. using the pointer to previous order of the top order).

## To Investigate / Profile

- [ ] Currently, message passing requires a copy of the message in the `Communication::LFQueue::push` method.
  - Would restricting the usage to pre-allocated memory only have an effect on speed?

## Description

This repo is based on [ModernCppStarter](MODERN_CPP_STARTER_README.md).

This repo is my low-level, no-LLM, non-soy attempt to implement an exchange in C++. The goal is to gain practical
experience with low-latency applications and optimization. 

I will rely heavily on the [Building Low Latency Applications with C++](https://www.oreilly.com/library/view/building-low-latency/9781837639359/)
(BLLA) book. However, in order to maximize learning, I will be taking the approach of first coding the components
myself, commiting my initial implementation, then going through the material of the book, applying the techniques
exposed in it, and finally making a new commit that contains all the updates and the reasons behind them.

## Techniques and Optimization Considerations

### Macros

Extensive use of the `UNLIKELY` macro to optimize **branch predictions**, and the `ASSERT` macro to make the code safer.

### Memory Management

#### Pre-Allocating Memory

My initial implementation of the `ObjectPool` used `deque` to dynamically allocate and never deallocate memory for the
order objects. This approach is memory efficient as it will allocate up to the maximum amount of memory used at peak
times and reuse memory after that. However, low-latency applications are not concerned with memory efficiency nearly as
much as they are with read/write performance. This means that an approach of pre-allocating a large chunk of contiguous
memory and using [placement new](https://en.cppreference.com/w/cpp/language/new) will optimize the initial memory
allocations (effectively remove them), and will ensure better **cache performance** through memory spatial locality.

This approach is applied to all containers of the system by pre-allocating sufficient space for the maximum number
of elements each one is supposed to be able to handle. We want to run at maximum memory capacity in order to optimize
latency.

### Memory Alignment

As suggested in the book, I performed a [memory alignment analysis](/standalone/investigation/alignment.cpp)
of my order objects. Some of them had to be adapted.

## Architecture Considerations

### OrderBook and Matching Engine

~~I have decided to separate the concerns of the order book (an efficient ledger for orders) and the matching engine
(the logic for matching orders that cross each other).~~

Upon revision, it makes much more sense to contain the entire order management logic inside the order book itself.
This makes the exchange object a light-weight manager of order books.

### Price Hack

The BLLA book's implementation contains a bug where, starting with an empty order book, if two sell orders of respective
prices `i + ME_MAX_PRICE_LEVELS` (my code renames `ME_MAX_PRICE_LEVELS` to `MAX_PRICE_LEVELS`) and `i` are placed in
succession, the second order will be placed on the same price level as the first, but will have lower priority. If then
a buy order for price lower than `i + ME_MAX_PRICE_LEVELS` is placed, no orders will be filled.

Problem fixed by keeping the price levels in a `std::unordered_map` instead of `std::array`, eliminating the modulo
operation on the price and instead indexing by the price proper. This approach is slightly less efficient, but remains
$O(1)$ amortized.

This approach is still hackable via overflowing the object pool, but I will simulate "good-actor" traders only.

## Dependencies

- `libevent`
- `pkg-config`
