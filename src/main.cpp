#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <memory>
#include "Config.h"
#include "BinanceApi.h"
#include "BitkubApi.h"
#include "OrderBookCollector.h"

#define CONFIG_WATCH_INTERVAL 5

// Convert time point to a std::time_t
template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    // cast the time point to system_clock time point
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    // convert the system_clock time point to time_t
    return system_clock::to_time_t(sctp);
}

// Monitors the config file for updates, and reloads the config and updates the collector accordingly
void watchConfigUpdate(Config &config, OrderBookCollector &collector) {
    auto configPath = config.getPath();
    std::time_t lastWriteTime = to_time_t(std::filesystem::last_write_time(configPath));
    std::time_t lastReloadTime = lastWriteTime;
    while (true) {
        // sleep for CONFIG_WATCH_INTERVAL seconds
        std::this_thread::sleep_for(std::chrono::seconds(CONFIG_WATCH_INTERVAL));

        // get the last write time of the config file
        std::ifstream file(configPath);
        lastWriteTime = to_time_t(std::filesystem::last_write_time(configPath));
        // if the last write time is greater than the last reload time
        if (lastWriteTime > lastReloadTime) {
            lastReloadTime = lastWriteTime;
            // reload the config
            config.reload();

            // update the tickers in the collector
            auto symbols = config.getStringArray("tickers");
            collector.updateTickers(symbols);
        }
    }
}

int main() {
    // Load config from file
    Config config("config/config.json");

    std::unique_ptr<CexApi> cexApi;
    // Check the cex in the config file
    if (config.getString("cex") == "binance") {
        // Create BinanceApi object
        cexApi = std::make_unique<BinanceApi>();
    } else if (config.getString("cex") == "bitkub") {
        // Create BitkubApi object
        cexApi = std::make_unique<BitkubApi>();
    } else {
        // If cex is unknown, print error message
        std::cout<< "Error: unknown CEX "<< config.getString("cex")<< std::endl;
        return 1;
    }

    // Start collecting order book
    auto outputPath = "output";
    auto tickers = config.getStringArray("tickers");
    OrderBookCollector collector(*cexApi, outputPath, tickers);
    collector.start();

    // Start monitoring the config file for updates
//    watchConfigUpdate(config, collector);

    return 0;
}
