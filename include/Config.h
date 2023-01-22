//
// Created by Thanakrit Dangsuwan on 17/1/2023 AD.
//

#ifndef ORDERBOOK_COLLECTOR_CONFIG_H
#define ORDERBOOK_COLLECTOR_CONFIG_H

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

class Config {
public:
    inline explicit Config(const std::string &file_path) : file_path_(file_path) {
        reload();
    }

    inline const std::string &getPath() const {
        return file_path_;
    }

    inline std::string getString(const std::string &key) {
        return pt_.get<std::string>(key);
    }

    inline int getInt(const std::string &key) {
        return pt_.get<int>(key);
    }

    std::vector<std::string> getStringArray(const std::string &key);
    void reload();

private:
    std::string file_path_;
    boost::property_tree::ptree pt_;

};

#endif //ORDERBOOK_COLLECTOR_CONFIG_H
