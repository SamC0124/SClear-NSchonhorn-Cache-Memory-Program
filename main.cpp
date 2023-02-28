#include <iostream>
#include <cstdint>
#include "Cache.hpp"
#include "HelperFunctions.hpp"
#include "fstream"
#include "string"

int main()
{
    // Open the given test file for the system, create the Cache and Memory vectors, input the filename here!
    ifstream TestFileRead;
    TestFileRead.open("../exampleAddresses.txt");

    // Initialize cache and individual cache blocks
    Cache testCache(16, 1, 64);

    // Initializing placeholder values
    string nextCommand, addressToReadString;
    int addressToRead, memoryToWrite;

    // Print Cache Statistics
    cout << "-----------------------------------------" << endl;
    cout << "cache size = 1024" << endl;
    cout << "block size = 64" << endl;
    cout << "#blocks = 16" << endl;
    cout << "#sets = 4" << endl;
    cout << "associativity = 4" << endl;
    cout << "tag length = 6" << endl;
    cout << "-----------------------------------------" << endl << endl;

    // Read in each of the lines of the file in a for-loop on the csv/dat/txt file
    while (TestFileRead && TestFileRead.peek() != EOF) {
        getline(TestFileRead, nextCommand);
        if (nextCommand.substr(0, 9) == "read_word") {

            // Find the address given in the file, then enter the address into the function call
            addressToReadString = nextCommand.substr(10, nextCommand.length() - 1);
            addressToRead = (int)stoi(addressToReadString);
            testCache.readWord(addressToRead);
        } else if (nextCommand.substr(0, 10) == "write_word") {
            addressToReadString = nextCommand.substr(9, -2);
            addressToRead = (int)stoi(addressToReadString);
            testCache.writeWord(addressToRead, 0);
        }
    }
    return 0;
}
