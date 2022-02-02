#pragma once

#include <string>
#include <random>

class FilenameGenerator
{
public:
    FilenameGenerator():
        _generator(_rd()),
        _dist(0, std::numeric_limits<uint64_t>::max())
    {
    }
    std::string operator() (void) { return std::to_string(_dist(_generator)) + ".~"; }
protected:
    std::random_device _rd;
    std::mt19937_64    _generator;
    std::uniform_int_distribution<uint64_t> _dist;
};
