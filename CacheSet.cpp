//
// Created by Noah Schonhorn on 2/15/23.
//

#include "CacheSet.hpp"
#include <cstdint>

CacheSet::CacheSet(int cacheBlockSize, int associativity)
{
    for (int i = 0; i < associativity; i++)
    {
        blocks.push_back(CacheBlock(cacheBlockSize, -(i), false, false));
        tagQueue.push_back(-(i));
    }
}

std::vector<CacheBlock> &CacheSet::getBlocks()
{
    return blocks;
}
