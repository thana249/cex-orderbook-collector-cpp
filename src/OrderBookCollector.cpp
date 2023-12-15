//
// Created by Thanakrit Dangsuwan on 15/1/2023 AD.
//

#include "OrderBookCollector.h"

#include <sstream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <future>
#include <iomanip>
#include <boost/filesystem.hpp>

OrderBookCollector::OrderBookCollector(CexApi &cexApi,
                                       const std::string &outputPath,
                                       const std::vector<std::string> &tickers) : cex_api_(cexApi), output_path_(outputPath) {
    request_interval_ = cexApi.getRequestInterval();

    // Create the output directory if it does not exist
    boost::filesystem::path dir(output_path_);
    if(boost::filesystem::create_directory(dir)) {
        std::cerr<< "Directory created: "<< output_path_<< std::endl;
    }

    // Get all available tickers to verify
    updateAvailableTickers();
    for (auto ticker : tickers) {
        // Check if the ticker is available
        if (find(available_tickers_.begin(), available_tickers_.end(), ticker) != available_tickers_.end()) {
            tickers_.emplace_back(ticker);
        } else {
            std::cout << "Ticker not available: " << ticker << std::endl;
        }
    }
}

OrderBookCollector::~OrderBookCollector() {
    for (const auto &ticker : tickers_) {
        threads_[ticker].join();
    }
}

void OrderBookCollector::start() {
    for (const auto &ticker : tickers_) {
        std::cout << "Start collecting order book [" << ticker << "]" << std::endl;
        stop_[ticker] = false;
        // Create a new thread and call the collect method with the current ticker as input
        threads_[ticker] = std::thread(&OrderBookCollector::collect, this, ticker);
    }
}

void OrderBookCollector::updateTickers(const std::vector<std::string> &tickers) {
    // Update the list of available tickers
    updateAvailableTickers();

    // Stop old threads
    std::unordered_set<std::string> newTickerSet(tickers.begin(), tickers.end());
    std::vector<std::string> tickersToRemove;
    for (const auto &ticker : tickers_){
        if (newTickerSet.count(ticker)==0) {
            tickersToRemove.push_back(ticker);
        }
    }
    for (const auto &ticker : tickersToRemove){
        auto it = find(tickers_.begin(), tickers_.end(), ticker);
        tickers_.erase(it);
        stop_[ticker] = true; // Tell current running thread to stop
    }

    // Add new tickers
    for (const auto &ticker : tickers){
        // Check if the ticker is available
        if(find(available_tickers_.begin(), available_tickers_.end(), ticker) == available_tickers_.end()) {
            continue;
        }
        // Check if the thread for the ticker is already running
        auto it = stop_.find(ticker);
        if (it == stop_.end() || it->second == true) { // the thread is not running
            // If the thread is not running, add the ticker, set the stop flag to false, and start a new thread for the ticker
            std::cout << "Start collecting order book [" << ticker << "]" << std::endl;
            tickers_.emplace_back(ticker);
            stop_[ticker] = false;
            threads_[ticker] = std::thread(&OrderBookCollector::collect, this, ticker);
        }
    }
}

void OrderBookCollector::updateAvailableTickers() {
    std::cout<< "Update available tickers...\n";
    auto tickers = cex_api_.getAllTickers();
    if (tickers.size() > 0) {
        available_tickers_ = tickers;
    }
}

void OrderBookCollector::collect(const std::string &ticker) const {
    Curl curl;
    std::time_t currentTime;
    std::time_t lastRequestTime = std::time(nullptr);
    lastRequestTime &= ~1;  // round to even number
    uint currentTimeBlock = (lastRequestTime + request_interval_) / FILE_STORAGE_DURATION;

    std::string orderBookData;
    std::string *currentOrderBookData = &orderBookData;
    while (true) {
        currentTime = std::time(nullptr);
        // Check if the time difference between the current time and the last request time is less than the request interval
        if (difftime(currentTime, lastRequestTime) < request_interval_) {
            continue;
        }
        lastRequestTime = currentTime;

        if (currentTimeBlock < currentTime / FILE_STORAGE_DURATION){
            // Save order book to file asynchronously
            std::async(std::launch::async,
                       &OrderBookCollector::saveOrderBooksToFile, this, ticker, currentTimeBlock, *currentOrderBookData);

            // Check the stop flag for the current ticker
            if (stop_.at(ticker)) {
                break;
            }

            currentTimeBlock = currentTime / FILE_STORAGE_DURATION;
            std::string newOrderBookData;
            currentOrderBookData = &newOrderBookData;
        }

        // Get the order book data from the cex_api_ and append it to the current order book data
        cex_api_.getOrderBook(curl, ticker, *currentOrderBookData);

        // Convert currentTime to milliseconds and calculate the time to sleep
        unsigned long long currentTimeMillis = static_cast<unsigned long long>(currentTime) * 1000; // Convert seconds to milliseconds
        unsigned long long interval_in_milliseconds = request_interval_ * 1000; // Convert seconds to milliseconds
        unsigned long long remainder = currentTimeMillis % interval_in_milliseconds;

        if (remainder > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_in_milliseconds - remainder));
        }
    }
    std::cout << "Stop collecting order book [" + ticker << "]" << std::endl;
}

void OrderBookCollector::saveOrderBooksToFile(const std::string &ticker,
                                              uint currentTimeBlock,
                                              const std::string &data) const {
    // Format the time in the format "YYYY-MM-DD HH:MM:SS"
    std::time_t t = currentTimeBlock*FILE_STORAGE_DURATION;
    std::tm* time = std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(time, "%Y-%m-%d %H:%M:%S");
    std::string timeStr = ss.str();

    // Clear and reset the stringstream for later use
    ss.str("");
    ss.clear();

    // Create the file name with the format "ticker_currentTimeBlock.txt"
    ss << ticker << "-" << currentTimeBlock << ".txt";
    std::string filePath = output_path_ + "/" + ss.str();
    std::cout << "[" << timeStr << "] Saving " << filePath << std::endl;

    // Write the data to the file
    std::ofstream orderBookFile;
    orderBookFile.open(filePath);
    orderBookFile<< data;
    orderBookFile.close();
}
