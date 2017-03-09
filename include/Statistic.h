//
// Created by jg on 1/3/17.
//

#ifndef MARA_STATISTIC_H
#define MARA_STATISTIC_H

#include <cstddef>
#include <map>

class Statistic {
public:
    /**
     * Registers a new dynamic memory array into the statistic.
     * @param size the array's size
     * @param address the array's address
     */
    static void newDynamic(size_t size, void * address);

    /**
     * Registers a new static memory array into the statistic.
     * @param size the array's size
     * @param address the array's address
     */
    static void newStatic(size_t size, void * address);

    /**
     * "Unregisters" a dynamic memory array from the statistic.
     * @param size the array's size
     * @param address the array's address
     */
    static void freeDynamic(size_t size, void * address);

    /**
     * Prints information about the used dynamic memory to stdout.
     */
    static void logDynamic();

    /**
     * Prints information about the used static memory to stdout.
     */
    static void logStatic();

    /**
     * Prints combined information about the used static and dynamic memory to stdout.
     */
    static void logComplete();

    /**
     * Prints complete information over dynamic and static memory and codeblock size as a table.
     * If STATISTIC_VERBOSE is set, also prints all used blocks (NOT RECOMMENDED FOR LARGE TESTS!!!)
     */
    static void logTable();

    static unsigned int getUsedStaticMemory();

    static unsigned int getUsedStaticBlocks();

    static unsigned int getUsedDynamicMemory();

    static unsigned int getUsedDynamicBlocks();

    static unsigned int getUsedDynamicMemoryWithCodeblocks();

    static unsigned int getDynamicMemoryPeak();

    static unsigned int getDynamicBlocksPeak();

private:
    /**
     * Static memory (in bytes) currently in use.
     */
    static unsigned int usedStaticMemory;

    /**
     * Number of static blocks currently in use.
     */
    static unsigned int usedStaticBlocks;

    /**
     * Dynamic memory (in bytes) currently in use, excluding codeblocks.
     */
    static unsigned int usedDynamicMemory;

    /**
     * Number of dynamic blocks currently in use.
     */
    static unsigned int usedDynamicBlocks;

    /**
     * Dynamic memory (in bytes) currently in use, including codeblocks.
     */
    static unsigned int usedDynamicMemoryWithCodeblocks;

    /**
     * Maximum used dynamic memory since initialization in bytes.
     */
    static unsigned int usedDynamicMemoryPeak;

    /**
     * Maximum used dynamic blocks since initialization.
     */
    static unsigned int usedDynamicBlocksPeak;

    /**
     * Map of all dynamic blocks and their size. Only used for STATISTIC_VERBOSE.
     */
    static std::map<void *, size_t> dynamicMemoryMap;

    /**
     * Map of all static blocks and their size. Only used for STATISTIC_VERBOSE.
     */
    static std::map<void *, size_t> staticMemoryMap;
};
#endif //MARA_STATISTIC_H
