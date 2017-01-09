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
private:
    static unsigned int usedStaticMemory;
    static unsigned int usedStaticBlocks;

    static unsigned int usedDynamicMemory;
    static unsigned int usedDynamicBlocks;
    static unsigned int usedDynamicMemoryWithCodeblocks;

    static std::map<void *, size_t> dynamicMemoryMap;
    static std::map<void *, size_t> staticMemoryMap;
};
#endif //MARA_STATISTIC_H
