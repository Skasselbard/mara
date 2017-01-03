//
// Created by jg on 1/3/17.
//

#include <string>
#include <cmath>
#include "../include/Statistic.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"
#include "../include/PageList.h"

unsigned int Statistic::usedStaticMemory = 0;
unsigned int Statistic::usedStaticBlocks = 0;
unsigned int Statistic::usedDynamicMemory = 0;
unsigned int Statistic::usedDynamicBlocks = 0;
unsigned int Statistic::usedDynamicMemoryWithCodeblocks = 0;

void Statistic::newDynamic(size_t size) {
    usedDynamicBlocks++;
    usedDynamicMemory += size;
    usedDynamicMemoryWithCodeblocks += size + 2*CodeBlock::getNeededCodeBlockSize(size);
}

void Statistic::newStatic(size_t size) {
    usedStaticBlocks++;
    usedStaticMemory += size;
}

void Statistic::freeDynamic(size_t size) {
    usedDynamicBlocks--;
    usedDynamicMemory -= size;
    usedDynamicMemoryWithCodeblocks -= size - 2*CodeBlock::getNeededCodeBlockSize(size);
}

void Statistic::logComplete() {
    unsigned int usedBlocks = usedDynamicBlocks + usedStaticBlocks;
    unsigned int usedMemory = usedDynamicMemory + usedStaticMemory;
    unsigned int usedBruttoMemory = usedDynamicMemoryWithCodeblocks + usedStaticMemory;
    Logger::debug(("Statistic: using " + std::to_string(usedBlocks) + " blocks on " + std::to_string(PageList::getPageCount()) + " pages, taking up "
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

void Statistic::logTable() {
    std::string beforeBlocks, betweenBlocksMemory, fillStatic, fillStaticBlocks, fillDynamic, fillDynamicBlocks, fillCb, separatorLine, fillTotal, fillTotalBlocks;
    unsigned int column1width = 12 + 2;
    unsigned int column2width = (unsigned int) (log10((usedDynamicBlocks >= usedStaticBlocks) ? usedDynamicBlocks : usedStaticBlocks) + 1);
    column2width = ((column2width < 6) ? 6 : column2width) + 4;
    unsigned int column3width = 2 * (unsigned int) (log10((usedDynamicMemory >= usedStaticMemory) ? usedDynamicMemory : usedStaticMemory) + 1);
    column3width = ((column3width < 6) ? 6 : column3width);

    beforeBlocks = std::string(column1width, ' ');
    betweenBlocksMemory = std::string(column2width - 6, ' ');
    fillStatic = std::string(column1width - 6, ' ');
    fillStaticBlocks = std::string(column2width - (int) log10(usedStaticBlocks) - 1, ' ');
    fillDynamic = std::string(column1width - 7, ' ');
    fillDynamicBlocks = std::string(column2width - (int) log10(usedDynamicBlocks) - 1, ' ');
    fillCb = std::string(column1width - 9 + column2width, ' ');
    separatorLine = std::string(column1width + column2width + column3width, '-');
    fillTotal = std::string(column1width - 5, ' ');
    fillTotalBlocks = std::string(column2width - (int) log10(usedDynamicBlocks+usedStaticBlocks) - 1, ' ');
    Logger::debug(("Statistic: " + std::to_string(PageList::getPageCount()) + " pages in use\n" + beforeBlocks + "Blocks" + betweenBlocksMemory + "Memory\n"
                  + "static" + fillStatic + std::to_string(usedStaticBlocks) + fillStaticBlocks + std::to_string(usedStaticMemory) + "B\n"
                  + "dynamic" + fillDynamic + std::to_string(usedDynamicBlocks) + fillDynamicBlocks + std::to_string(usedDynamicMemory) + "B\n"
                  + "codeblock" + fillCb + std::to_string(usedDynamicMemoryWithCodeblocks-usedDynamicMemory) + "B\n"
                  + separatorLine + "\n"
                  + "total" + fillTotal + std::to_string(usedDynamicBlocks+usedStaticBlocks) + fillTotalBlocks + std::to_string(usedDynamicMemory+usedStaticMemory)
                  + "B (" + std::to_string(usedDynamicMemoryWithCodeblocks+usedStaticMemory) + "B)").c_str());
}
