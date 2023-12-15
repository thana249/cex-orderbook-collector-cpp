//
// Created by Thanakrit Dangsuwan on 22/1/2023 AD.
//

#include "BinanceApi.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>
#include <chrono>

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

    // Parse json response
    rapidjson::Document document;
    document.Parse(responseBody.c_str());

    // Check if parsing succeeds
    if (document.HasParseError()) {
        std::cout << "[ERROR] Failed to parse response body as JSON" << std::endl;
        return;
    }

    // Create a new JSON object for the result
    rapidjson::Document resultJson;
    resultJson.SetObject();
    rapidjson::Document::AllocatorType& allocator = resultJson.GetAllocator();

    // Add current time to the JSON object
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    resultJson.AddMember("time", rapidjson::Value().SetInt64(static_cast<int64_t>(currentTime)), allocator);

    // Add the original response to the JSON object
    resultJson.AddMember("response", document, allocator);

    // Convert the result JSON object to string
    rapidjson::StringBuffer resultBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(resultBuffer);
    resultJson.Accept(writer);

    // Append the result to the result string
    result += resultBuffer.GetString();
    result += "\n";
}
