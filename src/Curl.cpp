//
// Created by Thanakrit Dangsuwan on 15/1/2023 AD.
//

#include "Curl.h"

#include <iostream>

Curl::Curl() {
    curl_ = curl_easy_init();
}

Curl::~Curl() {
    curl_easy_cleanup(curl_);
}

std::pair<long, std::string> Curl::request(const std::string& url,
                                           const std::string& method,
                                           const std::map<std::string, std::string> *data,
                                           long timeout,
                                           bool follow_redirects) const {
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, method.c_str());

    // set post fields for POST and PUT request
    if (method == "POST" || method == "PUT") {
        std::string post_data;
        if(data) {
            for (auto &[key, value]: *data) {
                post_data += key + "=" + value + "&";
            }
            post_data.pop_back(); //remove last &
        }
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, post_data.c_str());
    }

    // set timeout option
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout);

    // set follow redirects option
    if (follow_redirects) {
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
    } else {
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 0L);
    }

    // set the callback function
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &Curl::callback);
    std::string response;
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_);

    // get HTTP response code
    long http_code = 0;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);

    /* Check for errors */
    if(res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        return {http_code, ""};
    }
    return {http_code, response};
}

size_t Curl::callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* response = reinterpret_cast<std::string*>(userdata);
    response->append(reinterpret_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}
