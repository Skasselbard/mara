#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <random>
#include <map>
#include "../include/Logger.h"
#include "../include/Test.h"
#include "../include/interface.h"

using namespace std;



/**
 * Syntax: {@code test amountNewVariables pStatic pNewDynamic pFreeDynamic minSize averageSize maxSize iterations}
 */

int Test::test(int argc, char** argv) {

#define DEBUG

    unsigned int seed = 123456789;


    unsigned int amountNewVariables, minSize, averageSize, maxSize, maxIterations;
    double pDynamic, pFree;

    // TODO read parameters from arguments

    maxIterations = 1;
    amountNewVariables = 50000;
    pDynamic = 0.85;
    pFree = 0.2;
    minSize = 4;
    averageSize = 16;
    maxSize = 1000;

    std::vector<unsigned long*> dynamicPointers;

    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> prob_distribution(0, 1);
    std::uniform_int_distribution<unsigned int> dynamicVariable_distribution(0, amountNewVariables-1);

    //std::poisson_distribution<unsigned int> size_distribution(averageSize);
    std::uniform_int_distribution<unsigned int> size_distribution(averageSize);

    for (int iterations = 1; iterations <= maxIterations; iterations++) {
        for (int v = 0; v <= amountNewVariables; v++) {
            size_t varSize = 64;

            do { // generate a random size in the given boundaries
                varSize = size_distribution(generator);
                varSize = varSize + (4-(varSize % 4));
            } while (varSize < minSize || varSize > maxSize);

            unsigned long* address;
            if (prob_distribution(generator) <= pDynamic) {
                // request address to dynamic memory and save the address for later deletion
                address = (unsigned long*) dynamicNew(varSize);
                dynamicPointers.push_back(address);
            } else {
                // request address to static memory
                address = (unsigned long *) staticNew(varSize);
            }

            // write address to address
            unsigned long valueAtAddress = (unsigned long)address;
            *address = valueAtAddress;

            printf("iterations=%u, variables=%u\n", iterations, v);

            // maybe free a dynamic variable
            if (prob_distribution(generator) <= pFree) {
                unsigned long deletedIndex = (unsigned long) dynamicVariable_distribution(generator) % dynamicPointers.size();
                unsigned long* toDelete = dynamicPointers.at(deletedIndex);
                dynamicDelete(toDelete);
                dynamicPointers.erase(dynamicPointers.begin() + deletedIndex);
                Logger::debug("Freed dynamic memory");
            }
        }
    }

    printf("dynamic pointers in use: %u\n", (unsigned int) dynamicPointers.size());

    for (auto pointer : dynamicPointers) {
        cout << "pointer: " << pointer << "\n"
             << "&pointer: " << &pointer << "\n"
             //<< "*pointer: " << *pointer << "\n"
                ;
    }

    return 0;
}

int testBlock() {
    uint64_t zeiger = (uint64_t) dynamicNew(sizeof(uint64_t));
    uint64_t* pointerAufZeiger = &zeiger;

    *pointerAufZeiger = (uint64_t) pointerAufZeiger;


    cout << "Zeiger: " << zeiger << "\n"
         << "pointerAufZeiger: " << (uint64_t) pointerAufZeiger << "\n"
         << "*pointerAufZeiger: " << *pointerAufZeiger << "\n";

    return 0;
}

int Test::distributionTest() {

    unsigned int seed = 123456789;

    unsigned int amountNewVariables, minSize, averageSize, maxSize, maxIterations;
    double pDynamic, pFree;

    maxIterations = 1;
    amountNewVariables = 50000;
    pDynamic = 0.85;
    pFree = 0.2;
    minSize = 4;
    averageSize = 16;
    maxSize = 1000;

    std::vector<unsigned long*> dynamicPointers;

    std::mt19937 generator(seed);

    float stddev = (float) 5*(min(averageSize - maxSize, averageSize - minSize));
    std::normal_distribution<> d(averageSize, stddev);

    std::map<int, int> hist;
    for (int n = 0; n < 10000; ++n) {
        //cout << d(generator) << endl;
        int r;
        do {
            r = (int) d(generator)/4;
            cout << r << endl;
        } while (r < minSize/4 || r > maxSize/4);
        ++hist[r];
    }
    for (auto p : hist) {
        std::cout << p.first*4 << ' '
                  << '(' << p.second << ") "
                  << std::string((unsigned long) (p.second / 100), '*') << '\n';
    }
}



/*int Test::testBucketList() {



    cout << lastLinear4Scaling/4 << endl
         << (lastLinear16Scaling-lastLinear4Scaling+4)/16 << endl
         << (int) log2(largestBucketSize)-log2(lastLinear16Scaling) << endl
         << blSize << endl;

    int * bucketList[blSize];

    for (int i = 1; i <= 1100; i++) {
        cout << i << ": index=" << lookupBucket(i) << endl;
    }
}*/

