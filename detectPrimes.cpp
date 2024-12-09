

/// ============================================================================
/// Copyright (C) 2023 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// ============================================================================
///
/// Modified by Nusyba Shifa
/// UCID : 30162709
///
/// Starter code has been take from this file :https://csgit.ucalgary.ca/jwhudson/cpsc457w24/-/tree/main/detect-primes?ref_type=heads
///

#include "detectPrimes.h"
#include <cmath>
#include <condition_variable>
#include <mutex> //for thread managemnet
#include <thread> // for dynamic arrays
#include <vector>
#include <atomic>
#include <iostream>


//this simple barrier class synchronize specific number of threads 
// that making every thread wait until they have reached barrier point
class simple_barrier {

    //using mutex here to protect shared data
    std::mutex mutex_;  

    std::condition_variable cv_;
    int count_;
    int waiting_ = 0;
    int generation_ = 0;

public:

//initializing barrier with thread counts
    explicit simple_barrier(int count) : count_(count) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;

        if (++waiting_ == count_) {
            generation_++;
            waiting_ = 0;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }
};

//Here atomic flag is using to signal when the prime checking process should cancel
//across threads
std::atomic<bool> cancel(false);


//is_prime_worker() function
//prime worker function using to determine whethere the number is prime or not 
//by dividing work between multiple threads and uses a sinmple barrier as well.
//it utilizes early return whether 'n' is not prime detected by any thread 
// with shared state that is manging by atomic variables and barriers for synchroniztaion

void is_prime_worker(int64_t n, int thread_id, int n_threads, std::atomic<bool>& prime, simple_barrier& barrier) {
    if (n <= 1 || (n % 2 == 0 && n != 2) || (n % 3 == 0 && n != 3)) {
        if (thread_id == 0) prime.store(false);  // Only the first thread sets the prime to false to avoid race conditions
        return; //return for non-prime numbers
    }

    int64_t sqrt_n = std::sqrt(n) + 1;
    int64_t start = 5 + 6 * thread_id;
    int64_t end = sqrt_n;

    for (int64_t i = start; i <= end && !cancel.load(std::memory_order_relaxed); i += 6 * n_threads) {
        if (n % i == 0 || n % (i + 2) == 0) {
            prime.store(false);
            cancel.store(true);
            break;
        }
    }

    barrier.wait();

    // Reset for next number, only by one thread to avoid race condition
    if (thread_id == 0) {
        cancel.store(false);
    }

 // Ensure all threads see the reset cancel flag before proceeding to next number
    barrier.wait();
}



//detect_primes() function
//this function is taking list of ingers along with number of threads to be used
//it is also checking subset of numbers for primality using the is_prime_worker function
//this function uses a simple_abrrier to synchronize teh threads AND ensures the aromicity of shared variables 
//returning a list of prime numbers found in input list

std::vector<int64_t> detect_primes(const std::vector<int64_t>& nums, int n_threads) {
    std::vector<int64_t> primes;

    //iterate each number in input list
    for (int64_t n : nums) {
        std::atomic<bool> prime(true); // atomic flag to check if the current number is prime

        simple_barrier barrier(n_threads); // using barrier here that synchronizing threads 

        std::vector<std::thread> threads(n_threads);//using bvector to store threads

        //create threads
        for (int i = 0; i < n_threads; ++i) {
            threads[i] = std::thread(is_prime_worker, n, i, n_threads, std::ref(prime), std::ref(barrier));
        }

        //joining threads once their execution is complete.
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        //if the number is prime then add it to the list
        if (prime.load()) {
            primes.push_back(n);
        }
    }

    return primes;
}
