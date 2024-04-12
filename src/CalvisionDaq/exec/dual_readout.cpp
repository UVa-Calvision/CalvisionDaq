#include "CalvisionDaq/common/BufferedFileIO.h"
#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/digitizer/Command.h"
#include "CalvisionDaq/common/BinaryIO.h"
#include "CalvisionDaq/digitizer/Decoder.h"
#include "CalvisionDaq/root/RootIO.h"
#include "CalvisionDaq/common/Stopwatch.h"
#include "CalvisionDaq/common/MemoryPool.h"

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


class DigiContext {
public:

    DigiContext(size_t n, const std::string& config_file)
        // : root_io_("decoded_outfile_" + std::to_string(n) + ".root")
    {
        log_out_ = std::make_unique<std::ofstream>("readout_" + std::to_string(n) + ".log");
        digi_ = std::make_unique<Digitizer>(config_file, &std::cout /*log_out_.get()*/);
        
        pool_ = std::make_unique<PoolType>(1024);
        queue_ = std::make_unique<QueueType>();

        digi_->print();
        digi_->set_event_callback([this] (const char* data, UIntType count) {
                const size_t event_size = digi().event_size();
                size_t num_events = count / event_size;
                auto blocks = this->pool().allocate(num_events);
                for (auto* block : blocks) {
                    copy_raw_buffer<uint8_t, char>(block->block().data(), data, event_size);
                    this->queue().add(block);
                    data += event_size;
                }
            });
    }

    ~DigiContext()
    {
        pool_.reset();
        queue_.reset();

        digi_.reset();
        log_out_.reset();
    }

    Digitizer& digi() { return *digi_; }
    // std::ostream& log() { return *log_out_; }
    std::ostream& log() { return std::cout; }
    QueueType& queue() { return *queue_; }
    PoolType& pool() { return *pool_; }

private:
    std::unique_ptr<Digitizer> digi_;
    std::unique_ptr<std::ostream> log_out_;
    std::unique_ptr<QueueType> queue_;
    std::unique_ptr<PoolType> pool_;
};



void decode_loop(size_t thread_id, DigiContext& ctx) {
    Decoder decoder;
    RootWriter root_io("decoded_outfile_" + std::to_string(thread_id) + ".root");
    root_io.setup(decoder.event());

    while (auto block = ctx.queue().pop()) {
        BinaryInputBufferStream input((const char*) block->block().data(), ctx.digi().event_size());
        decoder.read_event(input);
        decoder.apply_corrections();
        root_io.handle_event(decoder.event());
        ctx.pool().deallocate(block);
    }

    root_io.write();
}

void main_loop(size_t thread_id, DigiContext& ctx) {

    try {
        std::thread decode_thread(&decode_loop, thread_id, std::ref(ctx));

        ctx.log() << "Beginning readout\n";
        ctx.digi().readout([](const Digitizer& d) {
                std::cout << "Read " << d.num_events_read() << "\n";
                return !quit_readout;
            });


        ctx.log() << "Stopped readout\n";

        ctx.queue().close();
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

constexpr static size_t N_DIGITIZERS = 2;

constexpr static size_t HV_SERIAL_NUMBER = 29622;
constexpr static size_t LV_SERIAL_NUMBER = 21333;

#include <TROOT.h>

int main(int argc, char** argv) {

    if (argc != 1 + N_DIGITIZERS) {
        std::cout << "Usage: " << argv[0];

        for (size_t i = 0; i < N_DIGITIZERS; i++) {
            std::cout << " [config " << (i+1) << "]";
        }
        std::cout << "\n";

        return 1;
    }

    ROOT::EnableThreadSafety();

    std::vector<std::thread> main_threads;
    std::vector<std::unique_ptr<DigiContext> > digis;
    for (size_t i = 1; i <= N_DIGITIZERS; i++) {
        digis.push_back(std::make_unique<DigiContext>(i, std::string(argv[i])));

    }
    
    for (size_t i = 0; i < N_DIGITIZERS; i++) {
        main_threads.emplace_back(&main_loop, i+1, std::ref(*digis[i]));
    }

    std::thread listener(&interrupt_listener);

    for (size_t i = 0; i < main_threads.size(); i++) {
        main_threads[i].join();
    }

    listener.join();

    return 0;
}
