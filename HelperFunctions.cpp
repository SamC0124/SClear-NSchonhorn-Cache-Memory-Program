//
// Created by Noah Schonhorn on 2/13/23.
//

#include "HelperFunctions.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstdint>

const int FIFTEEN_BIT_MAXIMUM = 32768;

using namespace std;

using Word = int;
/**
 *convertToBinary Function takes the address of the data and returns the address in binary format
 * @param pAddress
 * @return
 */
string convertToBinary(int pAddress) {
    string returnString = "";
    int currentMaximumValue = FIFTEEN_BIT_MAXIMUM;
    while (currentMaximumValue > 0) {
        if ((pAddress - currentMaximumValue) > -1) {
            returnString.push_back('1');
            pAddress -= currentMaximumValue;
        } else {
            returnString.push_back('0');
        }
        if (currentMaximumValue != 1) {
            currentMaximumValue /= 2;
        } else {
            currentMaximumValue = 0;
        }
    }
    return returnString;
}

int getMemoryLocation(int byteAddress, vector<int> tempMemory) {
    int iterator = 0, returnVal = 0;
    for (iterator = 0; iterator < 4 && byteAddress < tempMemory.size(); iterator++) {
        returnVal += (pow(256, iterator) * tempMemory[byteAddress]);
        byteAddress++;
    }
    return returnVal;

    /*
     *  byte_one   = memory[byteAddress]
     *  byte_two   = memory[byteAddress + 1]
     *  byte_three = memory[byteAddress + 2]
     *  byte_four  = memory[byteAddress + 3]
     *
     *  if byte_one is 0b00000000,
     *  byte_two is 0b11111111,
     *  byte_three is 0b22222222,
     *  and byte_four is 0b33333333, then
     *
     *  data will be: 0b33333333 22222222 11111111 00000000
     *  which is the same as...
     *  data = byte_one | (byte_two << 8) | (byte_three << 16) | (byte_four << 24)
     */
}

int logb2(int val)
{
    int i = 0;
    while (val > 0)
    {
        i = i + 1;
        val >>= 1;
    }
    return i;
}

int readWord(int address)
{
    return 0;
}

//void writeWord(int address, int memory)
//{
//
//    if (address % 4 != 0)
//    {
//        std::cout << "Error: address is not word-aligned" << std::endl;
//    }
//    else
//    {
//        /*
//         * data: 0b33333333 22222222 11111111 00000000
//         *
//         * byte_one: 0b00000000
//         * byte_two: 0b11111111
//         * byte_three: 0b22222222
//         * byte_four: 0b33333333
//         *
//         * these are then written to memory in little-endian order
//         */
//
//        int byte_one   =  data        & 0xFF;
//        int byte_two   = (data >>  8) & 0xFF;
//        int byte_three = (data >> 16) & 0xFF;
//        int byte_four  = (data >> 24) & 0xFF;
//
//        memory.at(address) = byte_one;
//        memory.at(address + 1) = byte_two;
//        memory.at(address + 2) = byte_three;
//        memory.at(address + 3) = byte_four;
//    }
//}

int getAddrTag(int address)
{
    // mask out the bottom 10 bits, and shift right
    return ((address & tag_mask) >> 10);
}

int getAddrIndex(int address)
{
    // mask out top and bottom 6 bits, and shift right
    return ((address & index_mask) >> 6);
}

int getAddrOffset(int address)
{
    // just mask out the bottom 6 bits
    return (address & offset_mask);
}