//
// Created by Thanakrit Dangsuwan on 22/1/2023 AD.
//

#include "BinanceApi.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>

std::unordered_set<std::string> BinanceApi::getAllTickers() const {
    // Make an HTTP GET request
    std::string url = "https://api.binance.com/api/v3/ticker/price";
    auto [statusCode, responseBody] = curl_.request(url, "GET");
    // If the request was not successful, return an empty set
    if (statusCode!=HTTP_STATUS_OK) {
        return {};
    }

    // Parse json response
    rapidjson::Document document;
    document.Parse(responseBody.c_str());
    auto jsonResult = document.GetArray();
    std::unordered_set<std::string> result;
    for (auto it=jsonResult.begin();it!=jsonResult.end();it++) {
        result.insert((*it)["symbol"].GetString());
    }
    return result;
}

void BinanceApi::getOrderBook(const Curl &curl, const std::string &ticker, std::string &result) const {
    // Make an HTTP GET request
    std::string url = "https://api.binance.com/api/v3/depth?symbol=" + ticker + "&limit=" + std::to_string(ORDER_BOOK_DEPTH);
    auto [statusCode, responseBody] = curl.request(url, "GET", nullptr, request_timeout_);
    // If request is not successfull then print the error
    if (statusCode!=HTTP_STATUS_OK) {
        std::cout<< "[ERROR] status code = "<< statusCode<< std::endl;
        return;
    }
    // append the response body to the result string
    result += responseBody;
    result += "\n";
}
