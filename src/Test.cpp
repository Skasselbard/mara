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

    Logger::info("Test started with seed: ");

    unsigned int seed = 123456789;


    unsigned int amountNewVariables, minSize, averageSize, maxSize, maxIterations;
    double pDynamic;

    maxIterations = 10;
    amountNewVariables = 10000000;
    pDynamic = 0.85;
    minSize = 4;
    averageSize = 16;
    maxSize = 1000;

    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> prob_distribution(0, 1);

    std::poisson_distribution<unsigned int> size_distribution(averageSize);

    for (int iterations = 0; iterations <= maxIterations; iterations++) {
        for (int v = 0; v <= amountNewVariables; v++) {
            size_t varSize = 64;

            do {
                varSize = size_distribution(generator);
            } while (varSize < minSize || varSize > maxSize);

            char** pointer;
            if (prob_distribution(generator) <= pDynamic) {
                pointer = (char**) dynamicNew(varSize);
                //printf("%zu\n", pointer);
            } else {
                pointer = (char **) staticNew(varSize);
                printf("%p\n", pointer);
            }
            char** * pointerAtPointer = &pointer;
            *pointerAtPointer = (char**) pointerAtPointer;
        }
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
