#include "Birdnet.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <sstream>
#include <algorithm>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "BirdEntity.h"
#include <regex>
#include <boost/asio.hpp>
#include "HTTPRequest.h"

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

std::string write_jsonEx(const ptree& json)
{
    std::ostringstream oss;
    boost::property_tree::write_json(oss, json);
    std::regex reg("\\\"([0-9]+\\.{0,1}[0-9]*)\\\"");
    std::regex reg1("\\\"(true)\\\"|\\\"(false)\\\"");
    std::string result = std::regex_replace(oss.str(), reg, "$1");
    std::string result1 = std::regex_replace(result, reg1, "$1");
    return result1;
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


    pt.put("tripwire", true);
    pt.put("activity", (float) 3.14);
    pt.put("lichtwaarde", 255);

    for (auto& item : resultVector) {
        ptree child;
        child.put(item.Birdname, item.PredictionScore);
        children1.push_back(std::make_pair("", child));
    }

 //   pt.add_child("sensor_values", children);
  //  pt.add_child("values", children);
    pt.add_child("birdnet", children1);
    //pt.put("values.name", "birdnet");
   // auto& array = pt.get_child("values");
    
    //array.push_back(std::make_pair("name", ptree("birdnet")));
   // std::string array_prefix = "name: birdnet";

    //array.push_back(std::make_pair("", ptree(array_prefix)));
    //array.push_back(std::make_pair("", ptree("value")));

   // array.push_back(std::make_pair("", children1));
    
    std::string json = write_jsonEx(pt); // {"foo":"bar"}
    std::cout << "JSON value: '" << json << "'" << std::endl;

    try
    {
        http::Request request("webhook.site/621b9ca7-7022-4e8f-9997-8e60b82cc4e1");
        // pass parameters as a map
        const auto response = request.send("POST", json, {
            "Content-Type: application/json"
            });
        std::cout << std::string{ response.body.begin(), response.body.end() } << '\n'; // print the result
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
    //sendJson(json, "https://webhook.site/621b9ca7-7022-4e8f-9997-8e60b82cc4e1");
    /* std::thread t1(birdnetThread);
    std::thread t2(jsonObserver);
    t1.join();
    t2.join(); */
   // std::cout << "Vector length: '" << birdnet_results.size() << "'" << std::endl;
    //example();
    getchar();
}


