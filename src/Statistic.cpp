//
// Created by jg on 1/3/17.
//

#include <string>
#include <cmath>
#include <list>
#include <vector>
#include <assert.h>
#include "../include/Statistic.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"
#include "../include/PageList.h"

unsigned int Statistic::usedStaticMemory = 0;
unsigned int Statistic::usedStaticBlocks = 0;
unsigned int Statistic::usedDynamicMemory = 0;
unsigned int Statistic::usedDynamicBlocks = 0;
unsigned int Statistic::usedDynamicMemoryWithCodeblocks = 0;

#ifdef STATISTIC_VERBOSE
std::map<void *, size_t> Statistic::dynamicMemoryMap;
std::map<void *, size_t> Statistic::staticMemoryMap;
#endif

void Statistic::newDynamic(size_t size, void * address) {
    usedDynamicBlocks++;
    usedDynamicMemory += size;
    usedDynamicMemoryWithCodeblocks += size + 2*CodeBlock::getNeededCodeBlockSize(size);
#ifdef STATISTIC_VERBOSE
    dynamicMemoryMap.insert(std::pair<void *, size_t>(address, size));
#endif
}

void Statistic::newStatic(size_t size, void * address) {
    usedStaticBlocks++;
    usedStaticMemory += size;
#ifdef STATISTIC_VERBOSE
    staticMemoryMap.insert(std::pair<void *, size_t>(address, size));
#endif
}

void Statistic::freeDynamic(size_t size, void * address) {
    usedDynamicBlocks--;
    usedDynamicMemory -= size;
    usedDynamicMemoryWithCodeblocks -= size - 2*CodeBlock::getNeededCodeBlockSize(size);
#ifdef STATISTIC_VERBOSE
    dynamicMemoryMap.erase(address);
#endif
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


#ifdef STATISTIC_VERBOSE
    std::list<void *> addressList;
    for (std::map<void *, size_t>::iterator it = dynamicMemoryMap.begin(); it != dynamicMemoryMap.end(); ++it) {
        addressList.push_back(it->first);
    }

    for (std::map<void *, size_t>::iterator it = staticMemoryMap.begin(); it != staticMemoryMap.end(); ++it) {
        addressList.push_back(it->first);
    }

    addressList.sort();

    std::string output = "\n#Cb    Address       Size        End            #Cb  Type\n";
    while (addressList.size() != 0) {
        unsigned long * address = (unsigned long *) addressList.front();
        addressList.pop_front();
        byte * codeBlockStart;
        size_t codeBlockSize = ((byte*)address) - codeBlockStart;

        //assert(calculatedSize == dynamicMemoryMap.find(address)->first || calculatedSize == staticMemoryMap.find(address));

        char addressBuffer[50];
        std::sprintf(addressBuffer, "0x%lx", (unsigned long) address);
        std::string addressString(addressBuffer);


        std::string memoryType = "error";
        size_t actualSize = 0;
        std::string codeBlockString = "-";
        if (dynamicMemoryMap.count(address)) {
            memoryType = "dynamic";
            actualSize = dynamicMemoryMap[address];
            codeBlockString = std::to_string(codeBlockSize);
        } else if (staticMemoryMap.count(address)) {
            memoryType = "static";
            actualSize = staticMemoryMap[address];
        }

        std::sprintf(addressBuffer, "0x%lx", (unsigned long) (((byte *)address) + actualSize));
        std::string endAddressString(addressBuffer);

        std::string fillAfterSize(12 - (int) log10((actualSize == 0) ? 1 : actualSize) - 1, ' ');

        output = output + " " + codeBlockString + "  "
                 + addressString + "   " + std::to_string(actualSize) + fillAfterSize
                 + endAddressString + "  " + codeBlockString + "   " + memoryType + "\n";
    }
    Logger::info(output.c_str());
#endif
}
