#pragma once

// Objective:
// 3. Data aggregator
//         Calculates average of averages. Prints out result after finishing work.


#include <thread>

#include "processer.hpp"

class Aggregator {
public:
    typedef Processer::output_type input_type;
    typedef input_type output_type;

    Aggregator(Queue<input_type>& inputQueue) : inputQueue(inputQueue) {}

    ~Aggregator() { join(); }

    // join thread
    void join() {
        if (thread.joinable())
            thread.join();
    }

    void start() {
        thread = std::thread(&Aggregator::run, this);
    }

    // can be used directly for same thread operation
    output_type run()
    {
        std::cout << "aggregator: started" << std::endl;

        size_t count = 0;
        output_type result = 0;

        try {
            while (true /*inputQueue*/) {
                input_type input = inputQueue.pop();  // will wait for value, will throw if closed
                // std::cout << "aggregator: got input value: " << input << std::endl;

                result += input;
                count++;
            }
        } catch(const std::out_of_range& e) { // alternative approach to checking inputQueue state
            // std::cout << "aggregator: input queue is closed" << std::endl;
            std::cout << "aggregator: input queue: " << e.what() << std::endl;
        } catch(const std::exception& e) {
            std::cerr << "aggregator: unexpected error: " << e.what() << std::endl;
        } catch(...) {
            std::cerr << "aggregator: unexpected error" << std::endl;
        }

        // std::cout << "aggregator: got final sum: " << result << "  and count: " << count << std::endl;

        result /= (output_type)count;

        std::cout << "aggregator: done, got final result: " << result << std::endl;

        {
            std::lock_guard<std::mutex> lock(mtx);
            output = result;
        }

        return result;
    }

    output_type result() {
        std::lock_guard<std::mutex> lock(mtx);
        return output;
    }

private:

    std::thread thread;
    std::mutex mtx;

    Queue<input_type>& inputQueue;
    output_type output;
};
