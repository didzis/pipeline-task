#pragma once

// Objective:
// 2. Data processer
//         Calculates average for each of vectors. Passes results to aggregator.

#include <thread>
#include <iostream>

#include "generator.hpp"
#include "queue.hpp"


class Processer {
public:
    typedef Generator::output_type input_type;
    // typedef Generator::value_type internal_type;
    typedef long int internal_type;       // define manually as long int to avoid (theoretical) overflow during calculations

    // use either integer type or floating point type for average calculation
    typedef double output_type;           // define manually as double to have fractions
    // typedef internal_type output_type;         // for whole numbers

    Processer(Queue<input_type>& inputQueue, Queue<output_type>& outputQueue) : inputQueue(inputQueue), outputQueue(outputQueue) {}

    ~Processer() { join(); }

    // join thread
    void join() {
        if (thread.joinable())
            thread.join();
    }

    void start() {
        thread = std::thread(&Processer::run, this);
    }

    // can be used directly for same thread operation
    void run()
    {
        std::cout << "processer: started" << std::endl;

        try {
            while (true /*inputQueue*/) {
                input_type input = inputQueue.pop();  // will wait for value, will throw if closed

                internal_type sum = 0;

                for (const auto v : input) {
                    sum += v;
                }

                // std::cout << "processer: sum: " << sum << "  size: " << input.size() << std::endl;

                output_type average = (output_type)sum / (output_type)input.size();

                // std::cout << "processer: got average: " << average << std::endl;

                // std::cout << "sum: " << sum << " count: " << input.size() << " avg: " << average << std::endl;

                // std::cout << "processer: push average" << std::endl;
                outputQueue.push(average);
            }
        } catch(const std::out_of_range& e) { // alternative approach to checking inputQueue state
            // std::cout << "processer: input queue is closed" << std::endl;
            std::cout << "processer: input queue: " << e.what() << std::endl;
        } catch(const std::exception& e) {
            std::cerr << "processer: unexpected error: " << e.what() << std::endl;
        } catch(...) {
            std::cerr << "processer: unexpected error" << std::endl;
        }

        std::cout << "processer: closing output queue" << std::endl;
        outputQueue.close();

        std::cout << "processer: completed, no more work will be done" << std::endl;
    }

private:

    std::thread thread;

    Queue<input_type>& inputQueue;
    Queue<output_type>& outputQueue;
};
