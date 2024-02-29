#include "BufferedFileIO.h"

#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
    
using namespace std::chrono_literals;

// std::mutex done_mutex;
// volatile bool done = false;

constexpr UIntType transfer_size = 20;
using EventType = UIntType[EventSize / sizeof(UIntType)];

void run_test() {
    static_assert(sizeof(EventType) == EventSize);

    BufferedFileWriter out;


    // std::thread worker_thread(write_loop, std::ref(out));

    UIntType a = 0;
    for (UIntType i = 0; i < 100; i++) {
        EventType x[transfer_size];
        for (int j = 0; j < transfer_size; j++) {
            for (int k = 0; k < EventSize / sizeof(UIntType); k++) {
                x[j][k] = a++;
            }
        }

        std::this_thread::sleep_for(500ms);

        std::cout << "[buffered] writing " << transfer_size << " events\n";
        out.write((BufferedType*) x, EventSize * transfer_size / sizeof(BufferedType));
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
    // validate_test();

    return 0;
}
