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
#include <atomic>

volatile bool quit_readout = false;
std::atomic<bool> dump_hv = false;
std::atomic<bool> dump_lv = false;

using PoolType = MemoryPool<Digitizer::max_event_size()>;
using QueueType = SPSCQueue<typename PoolType::BlockType, 2048>;


void decode_loop(size_t thread_id, DigitizerContext& ctx, PoolType& pool, QueueType& queue, std::atomic<bool>& dump) {
    std::cout << thread_id << ": Creating decoder\n";
    Decoder decoder(ctx.digi().serial_code());
    const std::string root_io_path = ctx.path_prefix() + "/outfile_" + ctx.name() + ".root";
    std::cout << thread_id << ": Creating root io: " << root_io_path << "\n";
    RootWriter root_io(root_io_path);
    root_io.setup(decoder.event());

    std::cout << thread_id << ": Entering decode loop\n";

    // Stopwatch<std::chrono::microseconds> stopwatch;
    // uint64_t last_read_waits = 0;

    size_t decode_count = 0;

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
        if (dump.load()) {
            std::cout << thread_id << ": writing waveform dump\n";
            root_io.dump_last_event(ctx.path_prefix() + "/dump_" + ctx.name());
            dump.store(false);
            std::cout << thread_id << ": waveform dump written\n";
        }

        pool.deallocate(block);

        decode_count++;
        if (ctx.digi().max_readout_count() && decode_count >= *ctx.digi().max_readout_count()) {
            break;
        }

        // stopwatch();
    }

    root_io.write();
}

void main_loop(size_t thread_id, DigitizerContext& ctx, std::atomic<bool>& dump) {

    PoolType pool(1024);
    QueueType queue;

    ctx.digi().set_event_callback(
        [&pool, &queue]
        (const char* data, UIntType event_size, UIntType num_events) {
            auto blocks = pool.allocate(num_events);
            for (auto* block : blocks) {
                copy_raw_buffer<uint8_t, char>(block->block().data(), data, event_size);
                queue.add(block);
                data += event_size;
            }
        });


    try {
        std::thread decode_thread(&decode_loop, thread_id, std::ref(ctx), std::ref(pool), std::ref(queue), std::ref(dump));

        ctx.log() << "Beginning readout\n";
        ctx.digi().readout([](const Digitizer& d) {
                std::cout << "Read " << d.num_events_read() << "\n";

                // Can maybe sleep for more efficient readouts?
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(20ms);
                if (d.max_readout_count() && d.num_events_read() >= *d.max_readout_count()) {
                    return false;
                }
                return !quit_readout;
            });


        ctx.log() << thread_id << ": Stopped readout\n";

        queue.close();
        decode_thread.join();

        ctx.log() << thread_id << ": Resetting digitzer...\n";
        ctx.digi().reset();

        ctx.log() << thread_id << ": Done.\n";

    } catch (const CaenError& error) {
        std::cerr << "[FATAL ERROR]: ";
        error.print_error(std::cerr);
        error.print_error(ctx.log());
    }
}

#include <termios.h>

void interrupt_listener() {
    struct termios cintty;
    struct termios cinsave;
    tcgetattr(STDIN_FILENO, &cinsave);
    cintty = cinsave;

    cintty.c_lflag &= ~(ICANON);
    cintty.c_cc[VMIN] = 0;
    cintty.c_cc[VTIME] = 0;
    cintty.c_cc[VEOF] = 4;

    tcsetattr(STDIN_FILENO, TCSANOW, &cintty);

    char buffer[4096];
    size_t current_size = 0;
    while (!quit_readout) {
        int num_read = ::read(STDIN_FILENO, buffer + current_size, 4095 - current_size);
        if (num_read < 0) {
            // An error has occured, stop the readout
            quit_readout = true;
            break;
        } else if (num_read == 0) {
            // didn't read anything
            continue;
        } else {
            current_size += num_read;
            buffer[current_size] = '\0';
            std::string message(buffer);

            if (message == "stop\n") {
                quit_readout = true;
                break;
            } else if (message == "sample plot\n") {
                dump_hv.store(true);
                dump_lv.store(true);
                current_size = 0;
            }
        }
    }
    std::cout << "Out of the loop\n";
    tcsetattr(STDIN_FILENO, TCSANOW, &cinsave);
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

    auto& hv_ctx = make_context<DigiMap::HG>(digis, argv);
    auto& lv_ctx = make_context<DigiMap::LG>(digis, argv);

    std::cout << "Beginning to make threads...\n";

    main_threads.emplace_back(&main_loop, 1, std::ref(hv_ctx), std::ref(dump_hv));
    main_threads.emplace_back(&main_loop, 2, std::ref(lv_ctx), std::ref(dump_lv));

    std::thread listener(&interrupt_listener);

    // joining threads

    for (size_t i = 0; i < main_threads.size(); i++) {
        main_threads[i].join();
    }

    quit_readout = true;

    listener.join();

    return 0;
}
