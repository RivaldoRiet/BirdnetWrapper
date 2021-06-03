#include "Birdnet.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

void example() {
    // Write json.
    ptree pt;
    pt.put("foo", "bar");
    std::ostringstream buf;
    write_json(buf, pt, false);
    std::string json = buf.str(); // {"foo":"bar"}
    std::cout << "JSON value: '" << json << "'" << std::endl;
}
int main(int argc, char* argv[]) {
    Birdnet birdnet;
    std::vector<std::string> birdnet_results = birdnet.get_birdnet_array();
    std::cout << "Vector length: '" << birdnet_results.size() << "'" << std::endl;
    //example();
    getchar();
}


