#include "BufferedFileIO.h"

#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
    
using namespace std::chrono_literals;

// std::mutex done_mutex;
// volatile bool done = false;

constexpr UIntType transfer_size = 25;

void run_test() {
    BufferedFileWriter out;

    std::this_thread::sleep_for(2000ms);

    // std::thread worker_thread(write_loop, std::ref(out));

    for (UIntType i = 0; i < 10000; i++) {
        if (i % 1 == 0) std::cout << "Step " << i << "\n";
        UIntType x[transfer_size];
        for (int j = 0; j < transfer_size; j++) {
            x[j] = transfer_size * i + j;
        }

        out.write((BufferedType*) x, EventSize * transfer_size);
    }

    out.close();

    // done_mutex.lock();
    // done = true;
    // done_mutex.unlock();

    // worker_thread.join();
}

void validate_test() {
    std::ifstream input("output.dat", std::fstream::binary);
    UIntType t;
    std::cout << "Starting validation\n";
    for (UIntType i = 0; true; i++) {
        input.read((char*) &t, sizeof(UIntType));
        if (!input) break;

        auto expected = i;
        if (expected != t) {
            std::cout << "Expected: " << expected << ",   Actual: " << t << "\n";
        }
    }
    std::cout << "Done with validation.\n";
}

int main(void) {

    run_test();
    validate_test();

    return 0;
}
