//
// Created by jg on 1/3/17.
//

#include <string>
#include "../include/Statistic.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"

unsigned int Statistic::usedStaticMemory = 0;
unsigned int Statistic::usedStaticBlocks = 0;
unsigned int Statistic::usedDynamicMemory = 0;
unsigned int Statistic::usedDynamicBlocks = 0;
unsigned int Statistic::usedDynamicMemoryWithCodeblocks = 0;

void Statistic::newDynamic(size_t size) {
    usedDynamicBlocks++;
    usedDynamicMemory += size;
    usedDynamicMemoryWithCodeblocks += size + 2*CodeBlock::getNeededCodeBlockSize(size);
    logDynamic();
}

void Statistic::newStatic(size_t size) {
    usedStaticBlocks++;
    usedStaticMemory += size;
    logStatic();
}

void Statistic::freeDynamic(size_t size) {
    usedDynamicBlocks--;
    usedDynamicMemory -= size;
    usedDynamicMemoryWithCodeblocks -= size - 2*CodeBlock::getNeededCodeBlockSize(size);
    logDynamic();
}

void Statistic::logComplete() {
    unsigned int usedBlocks = usedDynamicBlocks + usedStaticBlocks;
    unsigned int usedMemory = usedDynamicMemory + usedStaticMemory;
    unsigned int usedBruttoMemory = usedDynamicMemoryWithCodeblocks + usedStaticMemory;
    Logger::debug(("Statistic: using " + std::to_string(usedBlocks) + " blocks, taking up "
                  + std::to_string(usedMemory) + " bytes (" + std::to_string(usedBruttoMemory) + " brutto)").c_str());
}

void Statistic::logDynamic() {
    Logger::debug(("Statistic: using " + std::to_string(usedDynamicBlocks) + " dynamic blocks, taking up "
                  + std::to_string(usedDynamicMemory) + " bytes ("
                   + std::to_string(usedDynamicMemoryWithCodeblocks) + " brutto)").c_str());
}

void Statistic::logStatic() {
    Logger::debug(("Statistic: using " + std::to_string(usedStaticBlocks) + " static blocks, taking up "
                  + std::to_string(usedStaticMemory) + " bytes").c_str());
}
