//
// Created by Thanakrit Dangsuwan on 15/1/2023 AD.
//

#ifndef ORDERBOOK_COLLECTOR_CURL_H
#define ORDERBOOK_COLLECTOR_CURL_H

#include <curl/curl.h>
#include <string>
#include <map>

class Curl {
public:
    Curl();
    ~Curl();

    std::pair<long, std::string> request(const std::string& url,
                                         const std::string& method,
                                         const std::map<std::string, std::string> *data=nullptr,
                                         long timeout=0,  // ms // Default timeout is 0 (zero) which means it never times out.
                                         bool follow_redirects=false) const;

private:
    static size_t callback(void* ptr, size_t size, size_t nmemb, void* userdata);

    CURL* curl_;
};

#endif //ORDERBOOK_COLLECTOR_CURL_H
