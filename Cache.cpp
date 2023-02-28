//
// Created by Noah Schonhorn on 2/8/23.
//

#include "Cache.hpp"
#include "HelperFunctions.hpp"
#include <vector>
#include <cstdint>
#include <cmath>

using namespace std;

// Global Variable for Memory
std::vector<uint8_t> gMemory = vector<uint8_t>(MEMORY_SIZE);

Cache::Cache(int numSets, int pAssociativity, int cacheBlockSize)
{
    writeThrough = false;
    associativity = pAssociativity;

    // Create each of the cache sets
    for (int i = 0; i < numSets; i++)
    {
        sets.emplace_back(cacheBlockSize, associativity);
    }

    for (unsigned i = 0; i < MEMORY_SIZE; i += 4)
    {
        uint32_t new_write_val = i;
        uint8_t byte_1 = new_write_val & 0xFF;
        uint8_t byte_2 = (new_write_val >> 8) & 0xFF;
        uint8_t byte_3 = (new_write_val >> 16) & 0xFF;
        uint8_t byte_4 = (new_write_val >> 24) & 0xFF;

        gMemory[i] = byte_1;
        gMemory[i + 1] = byte_2;
        gMemory[i + 2] = byte_3;
        gMemory[i + 3] = byte_4;
    }

    memorySizeBits = logb2(MEMORY_SIZE);
    cacheSizeBits = logb2(CACHE_SIZE);
    cacheBlockSizeBits = logb2(CACHE_BLOCK_SIZE);
    indexLength = logb2(numSets);
    blockOffsetLength = logb2(CACHE_BLOCK_SIZE);
}

int Cache::getAddrTag(int address)
{
    // Next, remove the correct number of bits from the address to return the tag
    return ((address & tag_mask) >> (indexLength + blockOffsetLength));
}

int Cache::getAddrIndex(int address)
{
    // First, find the necessary number of bits to represent the associativity in an address
    bool associativityFound = false;
    int associativityBits = 0;
    while (!associativityFound) {
        if (pow(2, associativityBits) > associativity) {
            associativityFound = true;
        } else {
            associativityBits += 1;
        }
    }
    return ((address & index_mask) >> blockOffsetLength);
}

int Cache::getAddrOffset(int address)
{
    // just mask out the bottom 6 bits
    return (address & offset_mask);
}

uint32_t Cache::getWordValue(uint32_t pByteAddress, vector<uint8_t> &pMemory)
{
//    int iterator = 0;
    uint32_t returnVal = 0;
//    for (iterator = 0; iterator < 4 && pByteAddress < pMemory.size(); iterator++)
//    {
//        returnVal += (pow(256, iterator) * pMemory[pByteAddress]);
//        pByteAddress++;
//    }

    uint8_t byte_1 = pMemory[pByteAddress];
    uint8_t byte_2 = pMemory[pByteAddress + 1];
    uint8_t byte_3 = pMemory[pByteAddress + 2];
    uint8_t byte_4 = pMemory[pByteAddress + 3];

    returnVal = (byte_4 << 24) | (byte_3 << 16) | (byte_2 << 8) | byte_1;

    return returnVal;

}

/**
 *
 * @param address
 * @return
 */
int Cache::readWord(int address)
{
    // TEMPORARY COMMENT: Ignoring usage of LittleEndian function and word reading, processing whether the block is contained in the cache instead
    return this->accessMemory(address, 0, true);
}

/**
 *
 * @param address
 * @param memory
 */
void Cache::writeWord(int address, int memory)
{
    accessMemory(address, memory, false);
}

int Cache::accessMemory(int pCacheAddress, int pMemoryData, bool pRead)
{
    std::vector<uint8_t> &memory = gMemory;
    // Initializing temporary variables given)
    uint32_t aBlockIndex, word, indexForFoundWord;
    int givenAddressBlockIndex = -1, iterator, tempBlockIndex = -1;
    vector<int> tempTagQueue;

    // Initialize block offset, index, and tag of address
    uint32_t aBlockOffset = getAddrOffset(pCacheAddress);
    uint32_t aIndex = getAddrIndex(pCacheAddress);
    uint32_t aTag = getAddrTag(pCacheAddress);

    // If the tag starts after an index which is four bits long, then perform a bitwise shift of four bits.
    // This is performed for a graph with an associativity of one, while a graph with an associativity of
    // four has the Tag move with a bitwise shift of two bits.

    // Find the necessary number of bits to represent the associativity in an address
    bool associativityFound = false;
    int associativityBits = 0;
    while (!associativityFound) {
        if (::pow(2, associativityBits) > associativity) {
            associativityFound = true;
        } else {
            associativityBits += 1;
        }
    }

    // Find the expected block index for the given associativity
    aBlockIndex = (aIndex + (aTag << associativityBits)) % associativity;

    // Find the true block index at the given address
    CacheSet &set = sets[aIndex];
    for (iterator = 0; iterator < set.tagQueue.size(); iterator++)
    {
        if (this->sets[aIndex].tagQueue[iterator] == aTag)
        {
            givenAddressBlockIndex = iterator;
        }
    }

    // Check to see if there is any data to be found at the given block, or if it is empty
    if (set.blocks[aBlockIndex].getValid())
    {
        // Check to see if the given data was found at the given block, assuming it is not empty
        if (givenAddressBlockIndex != -1)
        {
            // This condition is found to be true, therefore we record that the correct value was
            // found and that we can return the word found at the given value. We also must move it up in the tag queue
            if (pRead)
            {
                indexForFoundWord = pCacheAddress % set.blocks[aBlockIndex].data[0];
                word = getWordValue(indexForFoundWord, set.blocks[aBlockIndex].data);
                // Because the tag is already set correctly for this block, we do not need to set the tag again within the block of the cache
                printf("read hit [addr=%d index=%d block_index=%d tag=%d: word=%u (%d - %d)]\n", pCacheAddress,
                       aIndex, aBlockIndex, aTag, word, pCacheAddress - aBlockOffset,
                       pCacheAddress - aBlockOffset + 63);

                // Remove every element from the LRU queue
                while (!tempTagQueue.empty())
                {
                    tempTagQueue.pop_back();
                }
                // Create a new version of the LRU queue with the first element first (Working Incorrectly)
                tempTagQueue.push_back(aTag);
                for (int givenTag: set.tagQueue)
                {
                    if (givenTag != aTag)
                    {
                        tempTagQueue.push_back(givenTag);
                    }
                }
                // Initialize the set's tagQueue with the most recent version of the LRU elements queue
                for (iterator = 0; iterator < set.tagQueue.size(); iterator++)
                {
                    set.tagQueue[iterator] = tempTagQueue[iterator];
                }
            }
            else
            {
                // Upload data to memory for the next four bytes
                uint8_t byte_1 = pMemoryData & 0xFF;
                uint8_t byte_2 = (pMemoryData >> 8) & 0xFF;
                uint8_t byte_3 = (pMemoryData >> 16) & 0xFF;
                uint8_t byte_4 = (pMemoryData >> 24) & 0xFF;

                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset] = byte_1;
                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset + 1] = byte_2;
                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset + 2] = byte_3;
                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset + 3] = byte_4;

                if (writeThrough)
                {
                    /*
                     * if this is a write through,
                     * break the word into bytes and write to memory
                     */
                    uint8_t byte_1 = pMemoryData & 0xFF;
                    uint8_t byte_2 = (pMemoryData >> 8) & 0xFF;
                    uint8_t byte_3 = (pMemoryData >> 16) & 0xFF;
                    uint8_t byte_4 = (pMemoryData >> 24) & 0xFF;

                    gMemory[pCacheAddress] = byte_1;
                    gMemory[pCacheAddress + 1] = byte_2;
                    gMemory[pCacheAddress + 2] = byte_3;
                    gMemory[pCacheAddress + 3] = byte_4;
                }
                else
                {
                    // if this *isn't* write through, set the dirty bit
                    set.blocks[aBlockIndex].setDirty(true);
                }
            }
            // If the block is valid but the given address block is not within the cache, then one of the
            // blocks must be replaced. This must be printed out differently to the user, but will be sorted
            // out in the readWord() function.

            set.blocks[aBlockIndex].setTag(aTag);
            set.blocks[aBlockIndex].setValid(true);
        }
        else
        {
            int invalidBlockIndex = -1;
            // Search for a block without the valid boolean value set to valid.
            // If there are no blocks this way, then we must evict a block.
            for (iterator = 0; iterator < set.blocks.size(); iterator++) {
                if (set.blocks[iterator].getValid()) {
                    invalidBlockIndex = iterator;
                    iterator = set.blocks.size();
                }
            }

            if (pRead) {
                // Since the data is not empty but we need to insert the data into the block, we must find what is the least relevent data,
                // read the data that we will place into the cache block from memory, record the data that is being replaced in the cache
                // block, and replace the data. Then, we can present to the user the word of data as a single 32-bit integer, or "word"!
                word = getWordValue(pCacheAddress, gMemory);

                // Because the tag is already set correctly for this block, we do not need to set the tag again within the block of the cache
                printf("read miss + replace [addr=%d index=%d tag=%d: word=%u (%d - %d)]\n", pCacheAddress, aIndex,
                       aTag, word, pCacheAddress - aBlockOffset, pCacheAddress - aBlockOffset + 63);

                // Find block index to-be evicted
                for (iterator = 0; iterator < set.blocks.size(); iterator++) {
                    if (set.tagQueue.size() == 1) {
                        if (set.blocks[iterator].getTag() == set.tagQueue[0]) {
                            tempBlockIndex = iterator;
                        }
                    } else {
                        if (set.blocks[iterator].getTag() == set.tagQueue[-1]) {
                            tempBlockIndex = iterator;
                        }
                    }
                }

                // If the block is not found in the set, return an error
                if (tempBlockIndex == -1) {
                    printf("Invalid data is saved in either the cache block or the tagQueue!\n");
                    return 0;
                }

                // Least-relevant data is collected
                printf("evict tag %d, in block index %d\n", set.tagQueue[tempBlockIndex], tempBlockIndex);

                // Assign each of the new values of the tag, block offset, and new data for the cache block
                set.blocks[tempBlockIndex].setValid(true);
                set.blocks[tempBlockIndex].setTag(aTag);
                set.blocks[tempBlockIndex].setDirty(false);
                while (!set.blocks[tempBlockIndex].data.empty()) {
                    set.blocks[tempBlockIndex].data.pop_back();
                }
                for (iterator = 0; iterator < 64; iterator++) {
                    set.blocks[tempBlockIndex].data.push_back(gMemory[pCacheAddress - aBlockOffset + iterator]);
                }

                // Print the block range that is being written back into the cache
                printf("[write back (%d - %d)]\n", pCacheAddress - aBlockOffset, pCacheAddress - aBlockOffset + 63);

                // Remove every element from the LRU queue
                while (!tempTagQueue.empty()) {
                    tempTagQueue.pop_back();
                }

                // Create a new version of the LRU queue with the first element first
                tempTagQueue.push_back(aTag);
                for (int givenTag: set.tagQueue) {
                    if (givenTag != aTag) {
                        tempTagQueue.push_back(givenTag);
                    }
                }

                // Initialize the set's tagQueue with the most recent version of the LRU elements queue
                for (int i = 0; i < set.tagQueue.size(); i++) {
                    set.tagQueue[i] = tempTagQueue[i];
                }
            } else {
                // No data has been found, so find what is the most irrelevant block in the queue, remove it, write in this data, then write this data to memory
                uint8_t byte_1 = pMemoryData & 0xFF;
                uint8_t byte_2 = (pMemoryData >> 8) & 0xFF;
                uint8_t byte_3 = (pMemoryData >> 16) & 0xFF;
                uint8_t byte_4 = (pMemoryData >> 24) & 0xFF;

                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset] = byte_1;
                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset + 1] = byte_2;
                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset + 2] = byte_3;
                set.blocks[aBlockIndex].data[pCacheAddress - aBlockOffset + 3] = byte_4;
                if (writeThrough) {
                    /*
                    * if this is a write through,
                    * break the word into bytes and write to memory
                    */
                    uint8_t byte_1 = pMemoryData & 0xFF;
                    uint8_t byte_2 = (pMemoryData >> 8) & 0xFF;
                    uint8_t byte_3 = (pMemoryData >> 16) & 0xFF;
                    uint8_t byte_4 = (pMemoryData >> 24) & 0xFF;

                    gMemory[pCacheAddress] = byte_1;
                    gMemory[pCacheAddress + 1] = byte_2;
                    gMemory[pCacheAddress + 2] = byte_3;
                    gMemory[pCacheAddress + 3] = byte_4;
                }

                set.blocks[aBlockIndex].setDirty(true);
                set.blocks[aBlockIndex].setTag(aTag);
            }
        }
    }

    else
    {
        if (pRead)
        {
            // Since this cache block is empty, we can enter each of the values without notice of what
            // we are evicting. First we return which data is being entered into the cache, then we
            word = getWordValue(pCacheAddress, gMemory);
            // Because the tag is already set correctly for this block, we do not need to set the tag again within the block of the cache
            int cacheBlockStart = pCacheAddress - aBlockOffset, cacheBlockEnd = pCacheAddress - aBlockOffset + 63;

            printf("read miss [addr=%d index=%d block_index=%d tag=%d: word=%u (%d - %d)]\n", pCacheAddress, aIndex,
                   aBlockIndex, aTag, word, cacheBlockStart, cacheBlockEnd);

            // Assign each of the new values of the tag, block offset, and new data for the cache block
            set.blocks[aBlockIndex].setValid(true);
            set.blocks[aBlockIndex].setTag(aTag);
            set.blocks[aBlockIndex].setDirty(false);
            while (!set.blocks[aBlockIndex].data.empty())
            {
                set.blocks[aBlockIndex].data.pop_back();
            }
            for (iterator = 0; iterator < 64; iterator++)
            {
                set.blocks[aBlockIndex].data.push_back(gMemory[pCacheAddress - aBlockOffset + iterator]);
            }

            // Remove every element from the LRU queue
            while (!tempTagQueue.empty())
            {
                tempTagQueue.pop_back();
            }

            // Create a new version of the LRU queue with the first element first
            tempTagQueue.push_back(aTag);
            for (int givenTag: set.tagQueue)
            {
                if (givenTag != aTag)
                {
                    tempTagQueue.push_back(givenTag);
                }
            }

            // Initialize the set's tagQueue with the most recent version of the LRU elements queue
            for (int i = 0; i < TAG_QUEUE_LENGTH; i++)
            {
                set.tagQueue[i] = tempTagQueue[i];
            }
        }
        else
        {
            // read the correct block from memory into this block
            for (int i = 0; i < CACHE_BLOCK_SIZE; i++)
            {
                set.blocks[aBlockIndex].data[i] = gMemory[pCacheAddress - aBlockOffset + i];
            }

            // write the value to the block
            set.blocks[aBlockIndex].data[aBlockOffset] = pMemoryData;

            // if this is a write through, write the value to memory also
            if (writeThrough)
            {
                /*
                 * if this is a write through,
                 * break the word into bytes and write to memory
                 */
                uint8_t byte_1 = pMemoryData & 0xFF;
                uint8_t byte_2 = (pMemoryData >> 8) & 0xFF;
                uint8_t byte_3 = (pMemoryData >> 16) & 0xFF;
                uint8_t byte_4 = (pMemoryData >> 24) & 0xFF;

                gMemory[pCacheAddress] = byte_1;
                gMemory[pCacheAddress + 1] = byte_2;
                gMemory[pCacheAddress + 2] = byte_3;
                gMemory[pCacheAddress + 3] = byte_4;
            }
            return 0;
        }
    }

    // Printing the tag queue of the given cache set
    printf("[");
    for (int tag: this->sets[aIndex].tagQueue)
    {
        printf(" %d", tag);
    }
    printf(" ]\n");

    // Print the new address in the cache block
    printf("address = %d ", pCacheAddress);
    printf("%s", convertToBinary(pCacheAddress).c_str());
    printf("; word = %u\n\n", word);

    return word;
}

// Temporary guidelines / unused code

//      either read the specified word or write the specified word
//// on a write: for a write-through cache, write the word also to memory
//// on a write: for a write-back cache, mark the block as dirty
//      set the tag for this block
//      set the valid flag for this block
//      update the tag queue for this set
//    else
//// cache miss
// ^^^ Sam

// vvv Noah
//// try to find an unused block in set i
//    if the valid flag for any block is set i is false
//// use that block
//    update the tag queue
//    set the valid flag for this block to true
//    if this is a read
//    read a cache block from memory into that block
//    else
//    write to that cache block
//    if a write-through cache then write to memory also
//    set the dirty flag for this block
//    set the tag for this block
//    else
//// must evict a cache block
//    find the least-recently used block, by checking the tag queue for this set
//    update the tag queue
//    set the tag for the target block
//    if this is a write-back cache
//    3
//    if the target block is dirty
//    write back the block
//    if this is a read
//    read the correct block from memory into this block
//    else
//// this is a write-allocate cache, so do the following
//    read the correct block from memory into this block
//    write the value to the block
//    if this is a write-through cache
//    write the value to memory also
//    int unused_index = -1;
//    for (int i = 0; i < ASSOCIATIVITY; ++i)
//    {
//        if (!set.getBlocks()[i].isValid())
//        {
//            unused_index = i;
//            break;
//        }
//    }
//    // use that block
//    if (unused_index != -1)
//    {
//        CacheBlock &block = set.getBlocks()[unused_index];
//        // update the tag queue
//        // set the valid flag for this block to true
//        block.setValid(true);
//        // if this is a read
//        if (isRead)
//        {
//            // read a cache block from memory into that block
//            int block_addr = ((tag << 10) | (index << 6));
//
//        }
//        else
//            // else
//        {
//            // write to that cache block
//
//            // if a write-through cache then write to memory also
//
//            // set the dirty flag and the tag for this block
//            block.setDirty(false);
//            block.setTag(tag);
//        }
//    }
//    else
//        // else
//    {
//
//    }