// config.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <fstream>
#include <string>

#include "config.h"

Config::Config() {
    std::ifstream cf("../config/settings.json");
    cf >> this->conf;
    cf.close();
};

double Config::get_num_setting(const std::string &name) const {
    return this->conf.at(name);
}

std::string Config::get_str_setting(const std::string &name) const {
    return this->conf.at(name);
}