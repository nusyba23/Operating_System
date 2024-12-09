//

//Name : Nusyba Shifa
//UCID : 30162709
// ======================================================================
// Starter code used from this file of Professor Jonathan Hudson: pi-calc/calcpi.cpp
//MODIFIED file.
// ======================================================================
//
// This code calculkates an approximate of pi using a geomatrical approach mentioned in this 
// Algorithm:
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pixels() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create


#include "calcpi.h"
#include <atomic>
#include <cmath>
#include <thread>
#include <vector>


//This function counts the pixel which are inside quarter circle 
void count_pixels_thread(int r, double rsq, int start, int end, std::vector<uint64_t>& local_counts, int thread_index) {
    uint64_t local_count = 0;

    //using nested loops to iterate over the picel grid
    for (double x = start; x <= end; x++) {
        for (double y = 0; y <= r; y++) {

            //using equation 
            if (x * x + y * y <= rsq) {
                local_count++;
            }
        }
    }

    //update count for thread that has shareing vector
    local_counts[thread_index] = local_count;
}


//MAIN function that is counting all the pixels inside quarter coircle 
uint64_t count_pixels(int r, int n_threads) {
    double rsq = static_cast<double>(r) * r;
    std::vector<uint64_t> local_counts(n_threads, 0);
    std::vector<std::thread> threads;

//Start calculkating soze of each chunk and check if any remaining wokr 
// that is not divisible by number of thraeds
    int chunk_size = r / n_threads;
    int remaining = r % n_threads;
    int start = 1;
    int end = 0;

//creating thraeds and assigning them to process
    for (int i = 0; i < n_threads; ++i) {
        end = start + chunk_size - 1;
        if (i < remaining) ++end;
        threads.emplace_back(count_pixels_thread, r, rsq, start, end, std::ref(local_counts), i);
        start = end + 1;
    }


//Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

//sum all the counts to get total count of pixels inside 
    uint64_t total_count = 0;
    for (auto count : local_counts) {
        total_count += count;
    }

//As we only counted a qyuarter circle so multiply by 4 and adding 1 for center pixel.
    return total_count * 4 + 1;
}
