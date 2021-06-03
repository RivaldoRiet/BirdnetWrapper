#include <iostream>
#include <string>

#pragma once
class BirdEntity {
public:
    std::string Birdname;
    std::string PredictionScore;
    BirdEntity(std::string x, std::string y) { // Constructor with parameters
        Birdname = x;
        PredictionScore = y;
    }
};
