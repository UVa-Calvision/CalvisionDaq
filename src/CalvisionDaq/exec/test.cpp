#include "CalvisionDaq/common/MemoryPool.h"
#include "CalvisionDaq/common/Stopwatch.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

using DataType = unsigned long;

using PoolType = MemoryPool<sizeof(DataType)>;
using QueueType = SPSCQueue<MemoryBlock<sizeof(DataType)>, 20>;

using namespace std::chrono_literals;

std::vector<DataType> output;
std::array<DataType, 200> data;

std::chrono::microseconds producer_time, consumer_time;

void producer(PoolType& pool, QueueType& queue) {

    Stopwatch<std::chrono::microseconds> stopwatch;

    const size_t n = 10;

    for (size_t i = 0; i < data.size(); i += n) {
        std::this_thread::sleep_for(10us);
        std::vector<MemoryBlock<sizeof(DataType)>*> blocks(n, nullptr);
        for (size_t j = 0; j < n; j++) {
            blocks[j] = pool.allocate();
            ((DataType*) blocks[j]->block().data())[0] = data[i+j];
        }

        for (auto* block : blocks)
            queue.add(block);
    }

    queue.close();

    producer_time = stopwatch();
}

void consumer(PoolType& pool, QueueType& queue) {
    Stopwatch<std::chrono::microseconds> stopwatch;

    while (auto block = queue.pop()) {
        output.push_back(((DataType*)block->block().data())[0]);
        pool.deallocate(block);
    }

    consumer_time = stopwatch();
}



int main(void) {
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = i;
    }

    PoolType char_pool(8);
    QueueType queue;

    Stopwatch<std::chrono::microseconds> stopwatch;

    std::thread producer_thread(&producer, std::ref(char_pool), std::ref(queue));
    std::thread consumer_thread(&consumer, std::ref(char_pool), std::ref(queue));

    producer_thread.join();
    consumer_thread.join();

    std::cout
        << "Wall     time: " << stopwatch() << "\n"
        << "Producer time: " << producer_time << "\n"
        << "Consumer time: " << consumer_time << "\n"
        << "\n"
        << "Total allocated size: " << char_pool.total_count() << "\n"
        << "Total write waits: " << queue.write_waits() << "\n"
        << "Total read  waits: " << queue.read_waits() << "\n";


    for (size_t i = 0; i < data.size(); i++) {
        if (i >= output.size()) {
            std::cout << "Output-data size mismatch: " << output.size() << " vs " << data.size() << "\n";
            break;
        }

        if (data[i] != output[i]) {
            std::cout << "mismatch @ " << i << ": " << data[i] << " != " << output[i] << "\n";
        }
    }

    return 0;
}
