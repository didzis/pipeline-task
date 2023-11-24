#pragma once

// Objective:
// 1. Data generator
//     Generates random size vectors of ints for random time (max 10s) and passes to processer.
//     Use some little sleep between each yield.
//
// For data passing between threads, create "universal (templated) queue" which you can use in all (current two) cases.


#include <limits>
#include <vector>
#include <chrono>
#include <thread>
#include <utility>
#include <iostream>

#include "random.hpp"
#include "queue.hpp"


using namespace std::chrono_literals; // C++14

class Generator {
public:
    // optionally can be converted to template
    typedef int value_type;
    typedef std::vector<value_type> output_type;
    typedef output_type::size_type size_type;
    using seconds = std::chrono::duration<double, std::ratio<1>>;

    // for verification output
    // typedef long int target_type;
    typedef double target_type;

    Generator(Queue<output_type>& outputQueue,
            size_type minSize = 10,
            size_type maxSize = 1000,
            value_type minValue = std::numeric_limits<value_type>::min(),
            value_type maxValue = std::numeric_limits<value_type>::max(),
            const seconds& minTime = 5.0s,              // C++14
            const seconds& maxTime = 10.0s,             // C++14
            const seconds& minDelay = 0.01s,            // C++14
            const seconds& maxDelay = seconds(0.1)) :   // C++11
        outputQueue(outputQueue),
        valueGenerator(rd, minValue, maxValue),
        sizeGenerator(rd, minSize, maxSize),
        timeGenerator(rd, minTime.count(), maxTime.count()),
        delayGenerator(rd, minDelay.count(), maxDelay.count()) { }

    ~Generator() { join(); }

    // join thread
    void join() {
        if (thread.joinable())
            thread.join();
    }

    void start() {
        thread = std::thread(&Generator::run, this);
    }

    // can be used directly for same thread operation
    void run()
    {
        std::cout << "generator: started" << std::endl;

        const seconds duration(timeGenerator());
        // const seconds duration = 0.07s;  // for debug
        const auto start = std::chrono::steady_clock::now();
        size_t count = 0;
        size_t sum = 0;
        target_type average_sum = 0;
        size_t average_count = 0;

        std::cout << "will run for " << duration.count() << " seconds" << std::endl;

        try {
            while((std::chrono::steady_clock::now() - start) < duration) {

                size_type size = sizeGenerator();

                output_type item(size); // pre-allocate
                // item.reserve(size);  // alternative pre-allocate approach

                target_type average = 0;

                for (size_type i = 0; i < size; i++) {
                    item[i] = valueGenerator();
                    // item.push_back(valueGenerator());  // alternative approach with reserve()

                    average += item[i]; // for verification
                }

                // for debug output
                count++;
                sum += item.size();

                // for verificaton
                // std::cout << "generator: sum: " << average << "  size: " << item.size() << std::endl;
                average = (target_type)average / (target_type)item.size();
                // std::cout << "generator: got average: " << average << std::endl;
                average_sum += average;
                average_count++;

                outputQueue.push(std::move(item));

                // sleep
                std::this_thread::sleep_for(seconds(delayGenerator()));
            }
        } catch(const std::exception& e) {
            std::cerr << "generator: unexpected error: " << e.what() << std::endl;
        } catch(...) {
            std::cerr << "generator: unexpected error" << std::endl;
        }

        std::cout << "generator: time limit of " << duration.count() << " seconds reached, no more data will be generated, " << count
            << " vectors generated, average vector size is " << (sum / count) << std::endl;

        std::cout << "generator: (for verification) average of averages: " << ((target_type)average_sum / (target_type)average_count) << std::endl;

        std::cout << "generator: closing output queue" << std::endl;
        outputQueue.close();
    }

private:

    std::thread thread;

    std::random_device rd;
    RandomGenerator<std::uniform_int_distribution<value_type>> valueGenerator;
    RandomGenerator<std::uniform_int_distribution<value_type>> sizeGenerator;
    RandomGenerator<std::uniform_real_distribution<double>> timeGenerator;
    RandomGenerator<std::uniform_real_distribution<double>> delayGenerator;

    Queue<output_type>& outputQueue;
};
