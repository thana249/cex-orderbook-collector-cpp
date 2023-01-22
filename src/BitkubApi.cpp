//
// Created by Thanakrit Dangsuwan on 21/1/2023 AD.
//

#include "BitkubApi.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>

std::unordered_set<std::string> BitkubApi::getAllTickers() const {
    // Make an HTTP GET request
    std::string url = "https://api.bitkub.com/api/market/symbols";
    auto [statusCode, responseBody] = curl_.request(url, "GET");
    // If the request was not successful, return an empty set
    if (statusCode!=HTTP_STATUS_OK) {
        return {};
    }

    // Parse json response
    rapidjson::Document document;
    document.Parse(responseBody.c_str());
    if (document["error"].GetInt() != 0){
        return {};
    }
    auto jsonResult = document["result"].GetArray();
    std::unordered_set<std::string> result;
    for (auto it=jsonResult.begin();it!=jsonResult.end();it++) {
        result.insert((*it)["symbol"].GetString());
    }
    return result;
}

void BitkubApi::getOrderBook(const Curl &curl, const std::string &ticker, std::string &result) const {
    // Make an HTTP GET request
    std::string url = "https://api.bitkub.com/api/market/books?sym=" + ticker + "&lmt=" + std::to_string(ORDER_BOOK_DEPTH);
    auto [statusCode, responseBody] = curl.request(url, "GET", nullptr, request_timeout_);
    // If request is not successfull then print the error
    if (statusCode!=HTTP_STATUS_OK) {
        std::cout<< "[ERROR] status code = "<< statusCode<< std::endl;
        return;
    }

    // Parse json response
    rapidjson::Document document;
    document.Parse(responseBody.c_str());
    // Check for error
    if (document["error"].GetInt() != 0){
        std::cout<< "[ERROR] "<< responseBody;
        return;
    }
    // Convert json object to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document["result"].Accept(writer);
    // Append the result to the result string
    result += buffer.GetString();
    result += "\n";
}
