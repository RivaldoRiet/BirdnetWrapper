#pragma once
#include <iostream>
#include <string>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <vector>
#include <chrono>
#include <thread>
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class Birdnet
{
public:
    std::vector<std::string> resultVector = {};
    bool shouldUpdate = false;
    std::string handle_pyerror();
    void get_birdnet_array();
    std::vector<std::string> get_random();
};