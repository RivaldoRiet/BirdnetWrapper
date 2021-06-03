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
    std::string handle_pyerror();
    std::vector<std::string> get_birdnet_array();
    std::vector<std::string> get_random();
};