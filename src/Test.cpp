#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <random>
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

    std::vector<char**> dynamicPointers;

    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> prob_distribution(0, 1);
    std::uniform_int_distribution<unsigned int> dynamicVariable_distribution(0, amountNewVariables-1);

    std::poisson_distribution<unsigned int> size_distribution(averageSize);

    for (int iterations = 1; iterations <= maxIterations; iterations++) {
        for (int v = 0; v <= amountNewVariables; v++) {
            size_t varSize = 64;

            do { // generate a random size in the given boundaries
                varSize = size_distribution(generator);
                varSize = varSize + (4-(varSize % 4));
            } while (varSize < minSize || varSize > maxSize);

            char** pointer;
            if (prob_distribution(generator) <= pDynamic) {
                // request pointer to dynamic memory and save the pointer for later deletion
                pointer = (char**) dynamicNew(varSize);
                dynamicPointers.push_back(pointer);
            } else {
                // request pointer to static memory
                pointer = (char **) staticNew(varSize);
            }

            // write address to address
            char** * pointerAtPointer = &pointer;
            //*pointerAtPointer = (char**) pointerAtPointer;
            *pointerAtPointer = (char**) pointerAtPointer;

            printf("iterations=%u, variables=%u\n", iterations, v);

            // maybe free a dynamic variable
            if (prob_distribution(generator) <= pFree) {
                unsigned long deletedIndex = (unsigned long) dynamicVariable_distribution(generator) % dynamicPointers.size();
                char** toDelete = dynamicPointers.at(deletedIndex);
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
