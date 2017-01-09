#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <random>
#include <map>
#include <assert.h>
#include "../include/Logger.h"
#include "../include/Test.h"
#include "../include/interface.h"
#include "../include/Page.h"
#include "../include/PageList.h"
#include "../include/CodeBlock.h"
#include "../include/MemDump.h"
#include "../include/Statistic.h"

using namespace std;


const double Test::DEFAULT_P_DYNAMIC = 0.85;
const double Test::DEFAULT_P_FREE = 0.2;


/**
 * Syntax: {@code test amountNewVariables pDynamic pFree minSize averageSize maxSize iterations seed}
 * Example arguments: test 50000 0.85 0.2 4 16 1000 1 123456789
 */

int Test::test(int argc, char** argv) {

    unsigned int seed = DEFAULT_SEED;


    unsigned int amountNewVariables, minSize, averageSize, maxSize, maxIterations;
    double pDynamic, pFree;

    // set default values
    maxIterations = DEFAULT_ITERATIONS;
    amountNewVariables = DEFAULT_VARIABLES;
    pDynamic = DEFAULT_P_DYNAMIC;
    pFree = DEFAULT_P_FREE;
    minSize = DEFAULT_MIN_SIZE;
    averageSize = DEFAULT_AVERAGE_SIZE;
    maxSize = DEFAULT_MAX_SIZE;

    Test::readArguments(argc, argv, &amountNewVariables, &pDynamic, &pFree, &minSize, &averageSize, &maxSize, &maxIterations, &seed);



    std::vector<unsigned long*> dynamicPointers;

    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> prob_distribution(0, 1);
    std::uniform_int_distribution<unsigned int> dynamicVariable_distribution(0, amountNewVariables-1);

    //std::poisson_distribution<unsigned int> size_distribution(averageSize);
    std::uniform_int_distribution<unsigned int> size_distribution(minSize, maxSize);

    for (int iterations = 1; iterations <= maxIterations; iterations++) {
        for (int v = 0; v <= amountNewVariables; v++) {
            size_t varSize = 64;

            do { // generate a random size in the given boundaries
                varSize = size_distribution(generator);
                varSize = varSize + (4-(varSize % 4));
            } while (varSize < minSize || varSize > maxSize);

            Logger::info(("iterations=" + std::to_string(iterations) + ", variables=" + std::to_string(v)).c_str());

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
            writeIntoBlock(address, varSize);

            checkPages();

            // maybe free a dynamic variable
            if (prob_distribution(generator) <= pFree) {
                unsigned long deletedIndex = (unsigned long) dynamicVariable_distribution(generator) % dynamicPointers.size();
                unsigned long* toDelete = dynamicPointers.at(deletedIndex);
                dynamicDelete(toDelete);
                dynamicPointers.erase(dynamicPointers.begin() + deletedIndex);
                char addressBuffer[50];
                std::sprintf(addressBuffer, "0x%lx", (unsigned long) toDelete);
                Logger::debug((std::string("Freed dynamic memory: ") + addressBuffer).c_str());
            }
        }
        // todo test shit
        checkPages();
        Statistic::logTable();
    }
    return 0;
}

void Test::writeIntoBlock(unsigned long * address, size_t size) {
    // todo make content configurable (write full value vs. only complete addresses)
    for(unsigned int i = 0; i < size/8; i++) {
#if FILL_REQUESTED_MEMORY == 0
        unsigned long valueAtAddress = 0;
#elif FILL_REQUESTED_MEMORY == 1
        unsigned long valueAtAddress = 1;
#elif FILL_REQUESTED_MEMORY == 2
        unsigned long valueAtAddress = (unsigned long) (address);
#endif
        *(address+i) = valueAtAddress;

    }
    char addressBuffer[50];
    std::sprintf(addressBuffer, "0x%lx", address);
    Logger::debug((std::string("Wrote values to address: ") + addressBuffer).c_str());
}

int Test::checkPages() {
    Logger::info("Checking pages");
    Page * page = PageList::getFirstPage();
    BucketList * bucketList = page->getBucketList();
    do {
        byte * startOfPage = (byte *) page->getStartOfPage();
        byte * blockPointer = startOfPage;
        byte * dynamicEnd = page->getDynamicEnd();
        while (blockPointer < dynamicEnd) {
            size_t memorySize = CodeBlock::readFromLeft(blockPointer);
            size_t codeBlockSize = CodeBlock::getBlockSize(blockPointer);
            if (CodeBlock::isFree(blockPointer) == 0) {
                unsigned long * memoryStart = (unsigned long *) (blockPointer + codeBlockSize);
                //MemDump::dumpFullBlock(memoryStart);
                for (int i = 0; i < memorySize / 8 - 1; i++) {
                    // TODO if less than 6 bytes are left, those are attached to the array. CHECK!
#if FILL_REQUESTED_MEMORY == 0
                    assert(*(memoryStart + i) == 0;
#elif FILL_REQUESTED_MEMORY == 1
                    assert(*(memoryStart + i) == 1;
#elif FILL_REQUESTED_MEMORY == 2
                    assert(*(memoryStart + i) == (unsigned long) memoryStart);
#endif
                }
            } else {
                int blIndex = BucketList::lookupBucket(memorySize);
                FreeSpace * freeSpace = bucketList->getFromBucketList(blIndex);
                FreeSpace * currentElement;
                for (currentElement = freeSpace;
                     currentElement != (FreeSpace *) blockPointer;
                     currentElement = currentElement->getNext(startOfPage)) {
                    if (currentElement == nullptr) break;
                }
                assert(currentElement != nullptr);
            }
            blockPointer = blockPointer + memorySize + 2 * codeBlockSize;
        }

    } while ((page = page->getNextPage()) != nullptr);
}

void Test::readArguments(int argc, char** argv, unsigned int * amountNewVariables,
                         double * pDynamic, double * pFree,
                         unsigned int * minSize, unsigned int * averageSize, unsigned int * maxSize,
                         unsigned int * maxIterations, unsigned int * seed){
    if (argc == 10) {
        sscanf(argv[2], "%u", amountNewVariables);
        sscanf(argv[3], "%lf", pDynamic);
        sscanf(argv[4], "%lf", pFree);
        sscanf(argv[5], "%u", minSize);
        sscanf(argv[6], "%u", averageSize);
        sscanf(argv[7], "%u", maxSize);
        sscanf(argv[8], "%u", maxIterations);
        sscanf(argv[9], "%u", seed);
    }
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