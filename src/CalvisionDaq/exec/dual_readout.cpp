#include "CalvisionDaq/common/BufferedFileIO.h"
#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/digitizer/Command.h"
#include "CalvisionDaq/common/BinaryIO.h"
#include "CalvisionDaq/digitizer/Decoder.h"
#include "CalvisionDaq/root/RootIO.h"
#include "CalvisionDaq/common/Stopwatch.h"
#include "CalvisionDaq/common/MemoryPool.h"
#include "CalvisionDaq/digitizer/Staging.h"

#include "CppUtils/c_util/CUtil.h"

#include <iostream>
#include <fstream>
#include <memory>

volatile bool quit_readout = false;

bool stop_readout(const Digitizer&) {
    return !quit_readout;
}

using PoolType = MemoryPool<Digitizer::max_event_size()>;
using QueueType = SPSCQueue<typename PoolType::BlockType, 2048>;


void decode_loop(size_t thread_id, DigitizerContext& ctx, PoolType& pool, QueueType& queue) {
    std::cout << thread_id << ": Creating decoder\n";
    Decoder decoder(ctx.digi().serial_code());
    std::cout << thread_id << ": Creating root io\n";
    RootWriter root_io(ctx.path_prefix() + "/outfile_" + ctx.name() + ".root");
    root_io.setup(decoder.event());

    std::cout << thread_id << ": Entering decode loop\n";

    // Stopwatch<std::chrono::microseconds> stopwatch;
    // uint64_t last_read_waits = 0;

    while (auto block = queue.pop()) {
        // const auto wait_duration = stopwatch();
        // if (ctx.queue().read_waits() > last_read_waits) {
        //     std::cout << thread_id << ": read waited " << wait_duration << "\n";
        //     last_read_waits = ctx.queue().read_waits();
        // }

        BinaryInputBufferStream input((const char*) block->block().data(), ctx.digi().event_size());
        decoder.read_event(input);
        decoder.apply_corrections();
        root_io.handle_event(decoder.event());
        pool.deallocate(block);

        // stopwatch();
    }

    root_io.write();
}

void main_loop(size_t thread_id, DigitizerContext& ctx) {

    PoolType pool(1024);
    QueueType queue;

    ctx.digi().set_event_callback(
        [&pool, &queue, event_size = ctx.digi().event_size()]
        (const char* data, UIntType count) {
            size_t num_events = count / event_size;
            auto blocks = pool.allocate(num_events);
            for (auto* block : blocks) {
                copy_raw_buffer<uint8_t, char>(block->block().data(), data, event_size);
                queue.add(block);
                data += event_size;
            }
        });


    try {
        std::thread decode_thread(&decode_loop, thread_id, std::ref(ctx), std::ref(pool), std::ref(queue));

        ctx.log() << "Beginning readout\n";
        ctx.digi().readout([](const Digitizer& d) {
                std::cout << "Read " << d.num_events_read() << "\n";

                // Can maybe sleep for more efficient readouts?
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(20ms);
                return !quit_readout;
            });


        ctx.log() << "Stopped readout\n";

        queue.close();
        decode_thread.join();

        ctx.log() << "Resetting digitzer...\n";
        ctx.digi().reset();

        ctx.log() << "Done.\n";

    } catch (const CaenError& error) {
        std::cerr << "[FATAL ERROR]: ";
        error.print_error(std::cerr);
        error.print_error(ctx.log());
    }
}


void interrupt_listener() {
    std::string message;
    while (std::getline(std::cin, message)) {
        std::cout << "Got the message: [" << message << "]\n";
        if (message == "stop") {
            quit_readout = true;
            return;
        }
    }
}

template <DigiMap d>
DigitizerContext& make_context(AllDigitizers& digis, char** argv) {
    auto* ctx = digis.get<d>();
    if (!ctx) {
        std::cerr << "Couldn't find "
                  << DigiMapTable.get<d, DigiMapValue::Name>()
                  << " digitizer (serial number "
                  << DigiMapTable.get<d, DigiMapValue::Serial>()
                  << ")\n";
        std::exit(1);
    }

    ctx->make_log(std::string(DigiMapTable.get<d, DigiMapValue::Name>()));
    ctx->digi().load_config(std::string(argv[DigiMapTable.get<d, DigiMapValue::ArgvNum>()]));
    ctx->digi().print();
    return *ctx;
}

#include <TROOT.h>

int main(int argc, char** argv) {

    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " [run name] [config hv] [config lv]\n";
        return 1;
    }

    ROOT::EnableThreadSafety();

    std::vector<std::thread> main_threads;
    AllDigitizers digis{std::string(argv[1])};

    auto& hv_ctx = make_context<DigiMap::HV>(digis, argv);
    auto& lv_ctx = make_context<DigiMap::LV>(digis, argv);

    std::cout << "Beginning to make threads...\n";

    main_threads.emplace_back(&main_loop, 1, std::ref(hv_ctx));
    main_threads.emplace_back(&main_loop, 2, std::ref(lv_ctx));

    std::thread listener(&interrupt_listener);

    // joining threads

    for (size_t i = 0; i < main_threads.size(); i++) {
        main_threads[i].join();
    }

    listener.join();

    return 0;
}
