//
// Created by Thanakrit Dangsuwan on 21/1/2023 AD.
//

#ifndef ORDERBOOK_COLLECTOR_CEXAPI_H
#define ORDERBOOK_COLLECTOR_CEXAPI_H

#include <unordered_set>
#include "Curl.h"

#define ORDER_BOOK_DEPTH 10
#define HTTP_STATUS_OK 200

class CexApi {
public:
    virtual ~CexApi() = default;
    virtual std::string getName() const = 0;
    virtual uint getRequestInterval() const = 0;    // seconds

    // Retrieves all ticker symbols from the Binance API
    virtual std::unordered_set<std::string> getAllTickers() const = 0;

    // Retrieves the order book (bids and asks) for a given ticker symbol
    virtual void getOrderBook(const Curl &curl, const std::string &ticker, std::string &result) const = 0;
};

#endif //ORDERBOOK_COLLECTOR_CEXAPI_H
