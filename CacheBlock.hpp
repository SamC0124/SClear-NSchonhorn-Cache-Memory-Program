//
// Created by Noah Schonhorn on 2/15/23.
//

#ifndef ASSIGNMENT2_CACHEBLOCK_HPP
#define ASSIGNMENT2_CACHEBLOCK_HPP

#include <vector>
#include <cstdint>

class CacheBlock
{
private:
    int tag;
    bool dirty;
    bool valid;
public:
    explicit CacheBlock(int cacheBlockSize, int tag, bool dirty, bool valid);
    bool getValid();
    void setValid(bool pValid);
    bool getDirty();
    void setDirty(bool pDirty);
    int getTag();
    void setTag(int pTag);
    void inputData(int pData, int pIndex) {
        if (pIndex > 0 && pIndex < data.size()) {
            data[pIndex] = pData;
        }
    };
    std::vector<uint8_t> data;

};


#endif //ASSIGNMENT2_CACHEBLOCK_HPP
