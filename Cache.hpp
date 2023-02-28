//
// Created by Noah Schonhorn on 2/8/23.
//

#ifndef ASSIGNMENT2_CACHE_HPP
#define ASSIGNMENT2_CACHE_HPP

static const int MEMORY_SIZE = 65536;
static const int CACHE_SIZE = 1024;
static const int CACHE_BLOCK_SIZE = 64;
static const int ASSOCIATIVITY = 1;
static const int TAG_QUEUE_LENGTH = 4;

#include <vector>
#include <cstdint>

#include "HelperFunctions.hpp"
#include "CacheBlock.hpp"
#include "CacheSet.hpp"

//using u32 = int;

class Cache
{
private:
public:
    Cache(int numSets, int pAssociativity, int cacheBlockSize);
    int accessMemory(int pCacheAddress, int pMemoryAddress, bool pRead);
    int readWord(int address);
    void writeWord(int address, int memory);
    uint32_t getWordValue(uint32_t pByteAddress, vector<uint8_t> &pMemory);
    int getAddrOffset(int address);
    int getAddrIndex(int address);
    int getAddrTag(int address);
    uint32_t getTagMask(int pAssociativity);
    uint32_t getIndexMask(int pAssociativity);
    uint32_t getOffsetMask(int pAssociativity);

    bool writeThrough;
    std::vector<CacheSet> sets;
    int associativity;

    int memorySizeBits;
    int cacheSizeBits;
    int cacheBlockSizeBits;
    int indexLength;
    int blockOffsetLength;
};

#endif //ASSIGNMENT2_CACHE_HPP
