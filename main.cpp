#include <iostream>

#include "generator.hpp"
#include "processer.hpp"
#include "aggregator.hpp"
#include "queue.hpp"


int main(int argc, char* argv[])
{
    using std::cout;
    using std::endl;

    cout << std::fixed;

    cout << "starting" << endl;

    Queue<Generator::output_type> generatorQueue; // a.k.a Queue<std::vector<int>>
    Queue<Processer::output_type> processerQueue; // a.k.a Queue<long int> or Queue<double>

    {
        Generator generator(generatorQueue /* NOTE: see declaration for configurable options */);
        Processer processer(generatorQueue, processerQueue);
        Aggregator aggregator(processerQueue);


        aggregator.start();  // run aggregator in separate thread, or see below for running in main thread

        processer.start();

        generator.start();

        // aggregator.run(); // runs in main thread
        // or if ran in separate thread
        aggregator.join();  // wait for result

        cout << "result: average of averages: " << aggregator.result() << std::endl;

        // generator.join(); // optional
        // processer.join(); // optional
    } // automatically join all threads here

    cout << "done" << endl;

    return 0;
}
