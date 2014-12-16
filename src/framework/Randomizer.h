#pragma once

#include <random>

class Randomizer {
public:

    Randomizer();

    virtual ~Randomizer();

    /**
    * @return [0..1]
    */
    double randomUnit();

    /**
    * @return [-1..1]
    */
    double randomSignedUnit();

private:
    std::mt19937 rng;
};
