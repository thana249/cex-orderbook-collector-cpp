//
// Created by Thanakrit Dangsuwan on 17/1/2023 AD.
//

#include "Config.h"

#include <iostream>

std::vector<std::string> Config::getStringArray(const std::string &key) {
    std::vector<std::string> array;
    for (const auto &item : pt_.get_child(key)) {
        array.push_back(item.second.get_value<std::string>());
    }
    return array;
}

void Config::reload() {
    // Open the configuration file
    std::ifstream file(file_path_);
    if (!file.is_open()) {
        std::cerr << "Unable to open config file: " << file_path_ << std::endl;
        return;
    }
    // Read the JSON data from the file into the property tree object
    boost::property_tree::read_json(file, pt_);
    file.close();
}
