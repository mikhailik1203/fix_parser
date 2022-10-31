
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <set>
#include <sched.h>
#include <cassert>
#include "../src/FIXParser.hpp"
#include "../src/FIXMessage.hpp"
#include "../src/dictionary/FIX44Builder.hpp"

using namespace fix;

namespace {
#ifdef DEBUG
    const uint64_t BENCHMARK_DATA_COUNT = 100000;
#else
    const uint64_t BENCHMARK_DATA_COUNT = 1000000; //10000000
#endif

    const int CPU_CORE = 2; ///Please check that you have this CPU core!!

    cpu_set_t g_mask;

    inline void assign_to_cpu(int core_id) {
        CPU_ZERO(&g_mask);
        CPU_SET(core_id, &g_mask);
        sched_setaffinity(0, sizeof(g_mask), &g_mask);
    }

    uint64_t benchmark_get_timer() {
        uint64_t avg_nsec = 0;        
        { /// measure throughput
            uint64_t thoughput_nsec = 0, dur = 0;
            {
                auto start_tm = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                    auto mid_tm = std::chrono::high_resolution_clock::now();
                    dur += std::chrono::duration_cast<std::chrono::nanoseconds>(mid_tm - start_tm).count();
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                thoughput_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count();
            }
            avg_nsec = thoughput_nsec/BENCHMARK_DATA_COUNT;
            std::cout << " Throughput of get_timer " << BENCHMARK_DATA_COUNT << " time is " << thoughput_nsec
                      << " nsec, avg="<< avg_nsec<< "nsec per call"<< std::endl;
            std::cout << " \t\t Calculated duration " << dur << std::endl;
        }

        { /// measure latency
            using LatencyT = std::vector<uint64_t>;
            LatencyT lat_data;
            lat_data.reserve(BENCHMARK_DATA_COUNT);

            auto start_tm = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                auto finish_tm = std::chrono::high_resolution_clock::now();
                lat_data.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count());
                start_tm = finish_tm;
            }
            std::sort(lat_data.begin(), lat_data.end());

            size_t index_95perc = static_cast<double>(lat_data.size()) * 0.95;
            size_t index_99perc = static_cast<double>(lat_data.size()) * 0.99;
            std::cout << " Latency of get_timer: min=" << *lat_data.begin() << " nsec; 50%="
                      << lat_data[lat_data.size() / 2] << " nsec; 95%=" << lat_data[index_95perc]
                      << " nsec; 99%=" << lat_data[index_99perc] << " nsec; max=" << *lat_data.rbegin() << " nsec"
                      << std::endl
                      << "\t" << *lat_data.begin()<< " | "<< lat_data[lat_data.size() / 2]<< " | "<< lat_data[index_95perc] << " | "
                      << lat_data[index_99perc] << " | "<< *lat_data.rbegin()<< std::endl;
        }
        return avg_nsec;
    }

    fix::FIXMessage create_MDSubscription(const FIXParser &parser){
        fix::FIXMessage md_msg = parser.create("V");

        {// fill tags
            md_msg.set_string(fix44_gen::SenderCompID, {"BenchSenderId"});
            md_msg.set_string(fix44_gen::TargetCompID, {"BenchTargetId"});
            md_msg.set_string(fix44_gen::MdReqId, {"123456"});
            md_msg.set_int(fix44_gen::MsgSeqNum, 777);
            md_msg.set_int(fix44_gen::SubscriptionRequestType, 1);
            md_msg.set_int(fix44_gen::MarketDepth, 0);
            md_msg.set_int(fix44_gen::MDUpdateType, 1);
            md_msg.set_datetime(fix44_gen::SendingTime, {"20221005 12:44:55.123456"});
            {
                auto &md_grp = md_msg.get_group(fix44_gen::NoMdEntryTypes);
                {
                    auto &md_entry = md_grp.add_entry();
                    md_entry.set_int(fix44_gen::MDEntryType, 0);
                }
                {
                    auto &md_entry = md_grp.add_entry();
                    md_entry.set_int(fix44_gen::MDEntryType, 1);
                }
                {
                    auto &md_entry = md_grp.add_entry();
                    md_entry.set_int(fix44_gen::MDEntryType, 2);
                }
            }
            {
                auto &md_grp = md_msg.get_group(fix44_gen::NoRelatedSym);
                {
                    auto &md_entry = md_grp.add_entry();
                    md_entry.set_string(fix44_gen::Symbol, {"MSFT"});
                }
                {
                    auto &md_entry = md_grp.add_entry();
                    md_entry.set_string(fix44_gen::Symbol, {"NVDA"});
                }
                {
                    auto &md_entry = md_grp.add_entry();
                    md_entry.set_string(fix44_gen::Symbol, {"AMZN"});
                }
            }
        }
        return md_msg;
    }

    fix::FIXMessage create_NewOrderSingle(const FIXParser &parser){
        fix::FIXMessage ord_msg = parser.create("D");
        {
            ord_msg.set_string(fix44_gen::SenderCompID, {"BenchSenderId"});
            ord_msg.set_string(fix44_gen::TargetCompID, {"BenchTargetId"});
            ord_msg.set_int(fix44_gen::MsgSeqNum, 777);
            ord_msg.set_datetime(fix44_gen::SendingTime, {"20221005 12:44:55.123456"});
            ord_msg.set_string(fix44_gen::ClOrdID, {"987654321"});
            ord_msg.set_char(fix44_gen::Side, '1');
            ord_msg.set_double(fix44_gen::OrderQty, {"10.0"});
            ord_msg.set_double(fix44_gen::Price, {"100.0"});
            ord_msg.set_string(fix44_gen::Symbol, {"MSFT"});
            ord_msg.set_string(fix44_gen::ExecInst, {"6"});
            ord_msg.set_char(fix44_gen::OrdType, '2');
            ord_msg.set_char(fix44_gen::TimeInForce, '1');
        }
        return ord_msg;
    }

    void benchmark_serialise_MDSubscription() {
        FIXDictionary dict_fix44 = FIX44Builder::build();
        FIXParser parser(dict_fix44);
        uint64_t avg_nsec = 0;        
        { /// measure throughput
            auto msg = create_MDSubscription(parser);        
            uint64_t thoughput_nsec = 0, dur = 0;
            {
                auto start_tm = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                    auto res = FIXParser::serialize(msg, true, true);
                    dur += res.length();
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                thoughput_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count();
            }
            avg_nsec = thoughput_nsec/BENCHMARK_DATA_COUNT;
            std::cout << " Throughput of MDSubscr[V] serialization " << BENCHMARK_DATA_COUNT << " time is " << thoughput_nsec
                      << " nsec, avg="<< avg_nsec<< "nsec per call"<< std::endl;
            {
                auto res = FIXParser::serialize(msg, true, true);
                std::cout << " Message is ["<< res.buffer()<< "], length "<< res.length() << ", total="<< dur<< std::endl;
            }
        }

        { /// measure latency
            using LatencyT = std::vector<uint64_t>;
            LatencyT lat_data;
            lat_data.reserve(BENCHMARK_DATA_COUNT);
            auto msg = create_MDSubscription(parser);
            uint64_t dur = 0;
            auto start_tm = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                {
                    auto res = FIXParser::serialize(msg, true, true);
                    dur += res.length();
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                lat_data.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count());
                start_tm = finish_tm;
            }
            std::sort(lat_data.begin(), lat_data.end());

            size_t index_95perc = static_cast<double>(lat_data.size()) * 0.95;
            size_t index_99perc = static_cast<double>(lat_data.size()) * 0.99;
            std::cout << " Latency of of MDSubscr[V] serialization: min=" << *lat_data.begin() << " nsec; 50%="
                      << lat_data[lat_data.size() / 2] << " nsec; 95%=" << lat_data[index_95perc]
                      << " nsec; 99%=" << lat_data[index_99perc] << " nsec; max=" << *lat_data.rbegin() << " nsec"
                      << std::endl
                      << "\t" << *lat_data.begin()<< " | "<< lat_data[lat_data.size() / 2]<< " | "<< lat_data[index_95perc] << " | "
                      << lat_data[index_99perc] << " | "<< *lat_data.rbegin()<< std::endl;
        }
    }

    void benchmark_parse_MDSubscription() {
        FIXDictionary dict_fix44 = FIX44Builder::build();
        FIXParser parser(dict_fix44);
        uint64_t avg_nsec = 0;        
        const char raw_msg[] = "8=FIX.4.4\0019=157\00135=V\00149=BenchSenderId\00156=BenchTargetId\00134=777\00152=20221005 12:44:55.123456\001"
            "262=123456\001263=1\001264=0\001265=1\001267=3\001269=0\001269=1\001269=2\001146=3\00155=MSFT\00155=NVDA\00155=AMZN\00110=024\001";
        size_t raw_msg_size = sizeof(raw_msg)/sizeof(char);
        { /// measure throughput
            uint64_t thoughput_nsec = 0, dur = 0;
            {
                auto start_tm = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                    auto res = parser.parse(raw_msg, raw_msg_size);
                    dur += static_cast<int>(res.protocol());
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                thoughput_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count();
            }
            avg_nsec = thoughput_nsec/BENCHMARK_DATA_COUNT;
            std::cout << " Throughput of MDSubscr[V] parse " << BENCHMARK_DATA_COUNT << " time is " << thoughput_nsec
                      << " nsec, avg="<< avg_nsec<< "nsec per call"<< std::endl;
            std::cout << " Message is ["<< raw_msg<< "], length "<< raw_msg_size << ", total="<< dur<< std::endl;
        }

        { /// measure latency
            using LatencyT = std::vector<uint64_t>;
            LatencyT lat_data;
            lat_data.reserve(BENCHMARK_DATA_COUNT);
            uint64_t dur = 0;
            auto start_tm = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                {
                    auto res = parser.parse(raw_msg, raw_msg_size);
                    dur += static_cast<int>(res.protocol());
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                lat_data.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count());
                start_tm = finish_tm;
            }
            std::sort(lat_data.begin(), lat_data.end());

            size_t index_95perc = static_cast<double>(lat_data.size()) * 0.95;
            size_t index_99perc = static_cast<double>(lat_data.size()) * 0.99;
            std::cout << " Latency of of MDSubscr[V] parse: min=" << *lat_data.begin() << " nsec; 50%="
                      << lat_data[lat_data.size() / 2] << " nsec; 95%=" << lat_data[index_95perc]
                      << " nsec; 99%=" << lat_data[index_99perc] << " nsec; max=" << *lat_data.rbegin() << " nsec"
                      << std::endl
                      << "\t" << *lat_data.begin()<< " | "<< lat_data[lat_data.size() / 2]<< " | "<< lat_data[index_95perc] << " | "
                      << lat_data[index_99perc] << " | "<< *lat_data.rbegin()<< std::endl;
        }
    }

    void benchmark_serialise_NewOrderSingle() {
        FIXDictionary dict_fix44 = FIX44Builder::build();
        FIXParser parser(dict_fix44);
        uint64_t avg_nsec = 0;        
        { /// measure throughput
            auto msg = create_NewOrderSingle(parser);        
            uint64_t thoughput_nsec = 0, dur = 0;
            {
                auto start_tm = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                    auto res = FIXParser::serialize(msg, true, true);
                    dur += res.length();
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                thoughput_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count();
            }
            avg_nsec = thoughput_nsec/BENCHMARK_DATA_COUNT;
            std::cout << " Throughput of NewOrderSngl[D] serialization " << BENCHMARK_DATA_COUNT << " time is " << thoughput_nsec
                      << " nsec, avg="<< avg_nsec<< "nsec per call"<< std::endl;
            {
                auto res = FIXParser::serialize(msg, true, true);
                std::cout << " Message is ["<< res.buffer()<< "], length "<< res.length() << ", total="<< dur<< std::endl;
            }
        }

        { /// measure latency
            using LatencyT = std::vector<uint64_t>;
            LatencyT lat_data;
            lat_data.reserve(BENCHMARK_DATA_COUNT);
            auto msg = create_NewOrderSingle(parser);
            uint64_t dur = 0;
            auto start_tm = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                {
                    auto res = FIXParser::serialize(msg, true, true);
                    dur += res.length();
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                lat_data.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count());
                start_tm = finish_tm;
            }
            std::sort(lat_data.begin(), lat_data.end());

            size_t index_95perc = static_cast<double>(lat_data.size()) * 0.95;
            size_t index_99perc = static_cast<double>(lat_data.size()) * 0.99;
            std::cout << " Latency of of NewOrderSngl[D] serialization: min=" << *lat_data.begin() << " nsec; 50%="
                      << lat_data[lat_data.size() / 2] << " nsec; 95%=" << lat_data[index_95perc]
                      << " nsec; 99%=" << lat_data[index_99perc] << " nsec; max=" << *lat_data.rbegin() << " nsec"
                      << std::endl
                      << "\t" << *lat_data.begin()<< " | "<< lat_data[lat_data.size() / 2]<< " | "<< lat_data[index_95perc] << " | "
                      << lat_data[index_99perc] << " | "<< *lat_data.rbegin()<< std::endl;
        }
    }

    void benchmark_parse_NewOrderSingle() {
        FIXDictionary dict_fix44 = FIX44Builder::build();
        FIXParser parser(dict_fix44);
        uint64_t avg_nsec = 0;        
        const char raw_msg[] = "8=FIX.4.4\0019=132\00135=D\00149=BenchSenderId\00156=BenchTargetId\00134=777\00152=20221005 12:44:55.123456\001"
                "11=987654321\00154=1\00138=10.0\00144=100.0\00155=MSFT\00118=6\00140=2\00159=1\00110=183\001";
        size_t raw_msg_size = sizeof(raw_msg)/sizeof(char);
        { /// measure throughput
            uint64_t thoughput_nsec = 0, dur = 0;
            {
                auto start_tm = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                    auto res = parser.parse(raw_msg, raw_msg_size);
                    dur += static_cast<int>(res.protocol());
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                thoughput_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count();
            }
            avg_nsec = thoughput_nsec/BENCHMARK_DATA_COUNT;
            std::cout << " Throughput of NewOrderSngl[D] parse " << BENCHMARK_DATA_COUNT << " time is " << thoughput_nsec
                      << " nsec, avg="<< avg_nsec<< "nsec per call"<< std::endl;
            std::cout << " Message is ["<< raw_msg<< "], length "<< raw_msg_size << ", total="<< dur<< std::endl;
        }

        { /// measure latency
            using LatencyT = std::vector<uint64_t>;
            LatencyT lat_data;
            lat_data.reserve(BENCHMARK_DATA_COUNT);
            uint64_t dur = 0;
            auto start_tm = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                {
                    auto res = parser.parse(raw_msg, raw_msg_size);
                    dur += static_cast<int>(res.protocol());
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                lat_data.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count());
                start_tm = finish_tm;
            }
            std::sort(lat_data.begin(), lat_data.end());

            size_t index_95perc = static_cast<double>(lat_data.size()) * 0.95;
            size_t index_99perc = static_cast<double>(lat_data.size()) * 0.99;
            std::cout << " Latency of of NewOrderSngl[D] parse: min=" << *lat_data.begin() << " nsec; 50%="
                      << lat_data[lat_data.size() / 2] << " nsec; 95%=" << lat_data[index_95perc]
                      << " nsec; 99%=" << lat_data[index_99perc] << " nsec; max=" << *lat_data.rbegin() << " nsec"
                      << std::endl
                      << "\t" << *lat_data.begin()<< " | "<< lat_data[lat_data.size() / 2]<< " | "<< lat_data[index_95perc] << " | "
                      << lat_data[index_99perc] << " | "<< *lat_data.rbegin()<< std::endl;
        }
    }


    void benchmark_create_empty_NewOrderSingle() {
        FIXDictionary dict_fix44 = FIX44Builder::build();
        FIXParser parser(dict_fix44);
        uint64_t avg_nsec = 0;        
        { /// measure throughput
            uint64_t thoughput_nsec = 0, dur = 0;
            {
                auto start_tm = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                    fix::FIXMessage ord_msg = parser.create("D");
                    dur += static_cast<int>(ord_msg.protocol());
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                thoughput_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count();
            }
            avg_nsec = thoughput_nsec/BENCHMARK_DATA_COUNT;
            std::cout << " Throughput of NewOrderSngl[D] create empty " << BENCHMARK_DATA_COUNT << " time is " << thoughput_nsec
                      << " nsec, avg="<< avg_nsec<< "nsec per call"<< std::endl;
        }

        { /// measure latency
            using LatencyT = std::vector<uint64_t>;
            LatencyT lat_data;
            uint64_t dur = 0;
            auto start_tm = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < BENCHMARK_DATA_COUNT; ++i) {
                {
                    fix::FIXMessage ord_msg = parser.create("D");
                    dur += static_cast<int>(ord_msg.protocol());
                }
                auto finish_tm = std::chrono::high_resolution_clock::now();
                lat_data.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(finish_tm - start_tm).count());
                start_tm = finish_tm;
            }
            std::sort(lat_data.begin(), lat_data.end());

            size_t index_95perc = static_cast<double>(lat_data.size()) * 0.95;
            size_t index_99perc = static_cast<double>(lat_data.size()) * 0.99;
            std::cout << " Latency of of NewOrderSngl[D] create empty: min=" << *lat_data.begin() << " nsec; 50%="
                      << lat_data[lat_data.size() / 2] << " nsec; 95%=" << lat_data[index_95perc]
                      << " nsec; 99%=" << lat_data[index_99perc] << " nsec; max=" << *lat_data.rbegin() << " nsec"
                      << std::endl
                      << "\t" << *lat_data.begin()<< " | "<< lat_data[lat_data.size() / 2]<< " | "<< lat_data[index_95perc] << " | "
                      << lat_data[index_99perc] << " | "<< *lat_data.rbegin()<< std::endl;
        }
    }


}

int main(int argc, char **argv) {
    assign_to_cpu(CPU_CORE);

    int random_seed = 123456789; /// use predefined generator to test same sequence on each run
    std::srand(random_seed);

    std::cout<< "======================================================================================="<< std::endl;
    //auto avg_timer_latency = benchmark_get_timer();
    benchmark_get_timer();
    // benchmark for plain message
    benchmark_create_empty_NewOrderSingle();
    benchmark_serialise_NewOrderSingle();
    benchmark_parse_NewOrderSingle();
    std::cout<< "----------------------------------------------------------------------------------------"<< std::endl;    
    // benchmark for message with groups 
    benchmark_serialise_MDSubscription();
    benchmark_parse_MDSubscription();
    std::cout<< "======================================================================================="<< std::endl;
    return 1;
}