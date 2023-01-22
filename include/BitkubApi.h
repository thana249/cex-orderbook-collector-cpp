//
// Created by Thanakrit Dangsuwan on 21/1/2023 AD.
//

#ifndef ORDERBOOK_COLLECTOR_BITKUBAPI_H
#define ORDERBOOK_COLLECTOR_BITKUBAPI_H

#include "CexApi.h"

class BitkubApi : public CexApi {
public:
    inline BitkubApi() {
        request_timeout_ = getRequestInterval()*1000-10;  // ms
    }

    inline std::string getName() const {
        return "bitkub";
    }

    inline uint getRequestInterval() const {
        return 2;
    }

    // Retrieves all ticker symbols from the Binance API
    std::unordered_set<std::string> getAllTickers() const;

    // Retrieves the order book (bids and asks) for a given ticker symbol
    void getOrderBook(const Curl &curl, const std::string &ticker, std::string &result) const;

private:
    Curl curl_;
    uint request_timeout_;  // ms

};

#endif //ORDERBOOK_COLLECTOR_BITKUBAPI_H
