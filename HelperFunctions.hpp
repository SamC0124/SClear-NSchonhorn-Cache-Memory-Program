//
// Created by Noah Schonhorn on 2/13/23.
//

#ifndef ASSIGNMENT2_HELPERFUNCTIONS_HPP
#define ASSIGNMENT2_HELPERFUNCTIONS_HPP

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

static const int tag_mask    = 0b1111110000000000;
static const int index_mask  = 0b0000001111000000;
static const int offset_mask = 0b0000000000111111;

using namespace std;

/**
 * moveToFront function moves the given element to the front of the LRU cache-block queue
 * @param val
 * @return TO BE USED IN PART 2
 */
bool moveToFront();

int logb2(int val);

int getAddrTag(int address);
int getAddrIndex(int address);
int getAddrOffset(int address);

string convertToBinary(int pAddress);

#endif //ASSIGNMENT2_HELPERFUNCTIONS_HPP
