#pragma once
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
/* #include <iostream>
#include <sstream>
using boost::property_tree::ptree;

namespace Entities {

    struct Birdnet {
        std::string Birdname;
        std::string Predictionscore;
    };

    std::string to_json(Birdnet const& o) {
        ptree out;
        out.put("Birdnet.Birdname", o.Birdname);
        out.put("Birdnet.Predictionscore", o.Predictionscore);

        std::ostringstream oss;
        boost::property_tree::write_json(oss, out);
        return oss.str();
    }
}

// ADL trigger; `using Entities::to_json` would be roughly equivalent, but not
// make it clear that ADL is happening
void to_json(); */