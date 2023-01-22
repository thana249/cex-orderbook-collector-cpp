//
// Created by Thanakrit Dangsuwan on 15/1/2023 AD.
//

#ifndef ORDERBOOK_COLLECTOR_ORDERBOOKCOLLECTOR_H
#define ORDERBOOK_COLLECTOR_ORDERBOOKCOLLECTOR_H

#include <unordered_set>
#include <vector>
#include <thread>
#include <iostream>
#include "CexApi.h"
#include "Curl.h"

//static uint FILE_STORAGE_DURATION = 3600; // 1 hour
static uint FILE_STORAGE_DURATION = 60; // 1 minute (for testing)

class OrderBookCollector {
public:
    OrderBookCollector(CexApi &cexApi,
                       const std::string &outputPath,
                       const std::vector<std::string> &tickers);

    ~OrderBookCollector();

    void start();
    void updateTickers(const std::vector<std::string> &tickers);

private:
    void updateAvailableTickers();
    void collect(const std::string &ticker) const;
    void saveOrderBooksToFile(const std::string &ticker,
                              uint currentTimeBlock,
                              const std::string &data) const;

    CexApi &cex_api_;
    uint request_interval_; // seconds
    std::string output_path_;
    std::unordered_set<std::string> available_tickers_;
    std::vector<std::string> tickers_;
    std::map<std::string, std::thread> threads_;
    std::map<std::string, bool> stop_;

};

#endif //ORDERBOOK_COLLECTOR_ORDERBOOKCOLLECTOR_H
