#include "config.h"
#include "nlohmann/json.hpp"
#include "fstream"

Config::Config() {}

Config Config::loadJson(const QString &path){
    using json = nlohmann::json;

    std::ifstream in("Config.json");
    json j;
    in>>j;
};
