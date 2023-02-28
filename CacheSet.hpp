//
// Created by Noah Schonhorn on 2/15/23.
//

#ifndef ASSIGNMENT2_CACHESET_HPP
#define ASSIGNMENT2_CACHESET_HPP

#include "CacheBlock.hpp"
#include <cstdint>

class CacheSet
{
public:
    std::vector<CacheBlock> blocks;
    std::vector<int> tagQueue;

    std::vector<CacheBlock> &getBlocks();
    CacheSet(int cacheBlockSize, int associativity);
};


#endif //ASSIGNMENT2_CACHESET_HPP
