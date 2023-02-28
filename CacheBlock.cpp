//
// Created by Noah Schonhorn on 2/15/23.
//

#include "CacheBlock.hpp"
#include <cstdint>

CacheBlock::CacheBlock(int cacheBlockSize, int tag, bool dirty, bool valid)
{
    setTag(tag);
    setDirty(dirty);
    setValid(valid);
    data.resize(cacheBlockSize);
}

bool CacheBlock::getValid()
{
    return valid;
}

bool CacheBlock::getDirty()
{
    return dirty;
}

int CacheBlock::getTag() {
    return tag;
}

void CacheBlock::setDirty(bool pDirty)
{
    dirty = pDirty;
}

void CacheBlock::setValid(bool pValid)
{
    valid = pValid;
}

void CacheBlock::setTag(int pTag)
{
    tag = pTag;
}

// NEEDS TO BE FIXED WITH HEADER FILE: void inputData(int pData, int pIndex);
