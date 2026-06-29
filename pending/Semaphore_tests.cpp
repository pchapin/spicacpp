/*! \file    test_semaphore.cpp
 *  \brief   Program for CIS-4230, Homework #2.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 *  Simple test program to exercise the Semaphore class.
 */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <spicacpp/Semaphore.hpp>
#include <thread>

using namespace spica;

// Keep track of the number of available resource items.
const int count = 10000;
Semaphore resource_counter(count);

// Used to synchronize access to the console (to prevent interleaved output lines).
std::mutex console_lock;

void upper()
{
    // Produce COUNT resources and signal the Semaphore for each one.
    for (int i = 0; i < count; ++i) {
        resource_counter.up();
        if ((i + 1) % 1000 == 0) {
            std::lock_guard<std::mutex> guard(console_lock);
            std::cout << "Produced " << i << " items\n";
        }
    }
}

void downer()
{
    // Consume COUNT resources.
    for (int i = 0; i < count; ++i) {
        resource_counter.down();
        if ((i + 1) % 1000 == 0) {
            std::lock_guard<std::mutex> guard(console_lock);
            std::cout << "Consumed " << i << " items\n";
        }
    }

    // Try to consume one more resource (not available... I block here)
    resource_counter.down();
}

int main()
{
    std::thread thread1(upper);
    std::thread thread2(downer);

    // Delay a bit so that the loops above end and downer is blocked waiting.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Give downer what he seeks (so he'll end cleanly).
    resource_counter.up();

    // Be sure both threads have fully ended before terminating the program.
    thread1.join();
    thread2.join();
    return EXIT_SUCCESS;
}
