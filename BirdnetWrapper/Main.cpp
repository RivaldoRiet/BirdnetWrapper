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
#include "BirdEntity.h"

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

Birdnet* birdnet = new Birdnet();

void birdnetThread() {
    birdnet->get_birdnet_array();
}

void jsonObserver()
{
    while (true) {
        if (birdnet->shouldUpdate) {
            std::cout << "Printing json now" << std::endl;
            birdnet->shouldUpdate = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}


int main(int argc, char* argv[]) {
    ptree pt;
    ptree children;
    ptree children1;
    const auto p1 = std::chrono::system_clock::now();
    std::string unix = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
        p1.time_since_epoch()).count());
    pt.put("timestamp", unix);
    pt.put("id", "urn:shertogenbosch.faunatoren.1.hok.1");


    std::vector<BirdEntity> resultVector = {};
    BirdEntity birdEntity("merel", "0.15");
    BirdEntity birdEntity1("spreeuw", "0.35");
    BirdEntity birdEntity2("koolmees", "0.65");
    BirdEntity birdEntity3("zwartkop", "0.46");
    resultVector.push_back(birdEntity);
    resultVector.push_back(birdEntity1);
    resultVector.push_back(birdEntity2);
    resultVector.push_back(birdEntity3);

    ptree sensorChild;
    sensorChild.put("name", "tripwire");
    sensorChild.put("value", "true");
    children.push_back(std::make_pair("", sensorChild));
    sensorChild.put("name", "activity");
    sensorChild.put("value", "3.14");
    children.push_back(std::make_pair("", sensorChild));
    sensorChild.put("name", "lichtwaarde");
    sensorChild.put("value", "255");
    children.push_back(std::make_pair("", sensorChild));

    for (auto& item : resultVector) {
        ptree child;
        child.put("name", item.Birdname);
        child.put("value", item.PredictionScore);
        children1.push_back(std::make_pair("", child));
    }

    pt.add_child("values", children);
    pt.put("name", "birdnet");
    pt.add_child("value", children1);

    std::ostringstream buf;
    write_json(buf, pt, false);
    std::string json = buf.str(); // {"foo":"bar"}
    std::cout << "JSON value: '" << json << "'" << std::endl;
    /* std::thread t1(birdnetThread);
    std::thread t2(jsonObserver);
    t1.join();
    t2.join(); */
   // std::cout << "Vector length: '" << birdnet_results.size() << "'" << std::endl;
    //example();
    getchar();
}


