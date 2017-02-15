//
// Created by jg on 1/3/17.
//

#ifndef MARA_STATISTIC_H
#define MARA_STATISTIC_H

#include <cstddef>
#include <map>

class Statistic {
public:
    static void newDynamic(size_t size, void * address);
    static void newStatic(size_t size, void * address);

    static void freeDynamic(size_t size, void * address);

    static void logDynamic();
    static void logStatic();
    static void logComplete();
    static void logTable();

    static unsigned int getUsedStaticMemory();

    static unsigned int getUsedStaticBlocks();

    static unsigned int getUsedDynamicMemory();

    static unsigned int getUsedDynamicBlocks();

    static unsigned int getUsedDynamicMemoryWithCodeblocks();

    static unsigned int getDynamicMemoryPeak();

    static unsigned int getDynamicBlocksPeak();

private:
    static unsigned int usedStaticMemory;
    static unsigned int usedStaticBlocks;

    static unsigned int usedDynamicMemory;
    static unsigned int usedDynamicBlocks;
    static unsigned int usedDynamicMemoryWithCodeblocks;

    static unsigned int usedDynamicMemoryPeak;
    static unsigned int usedDynamicBlocksPeak;


    static std::map<void *, size_t> dynamicMemoryMap;
    static std::map<void *, size_t> staticMemoryMap;
};
#endif //MARA_STATISTIC_H
