//
// Created by tom on 20.11.16.
//

#ifndef MARA_SPACE_H
#define MARA_SPACE_H


#include <glob.h>
#include "predefined.h"

class OccupiedSpace;


/**
 * Basic Structure:<br/>
 * <code>
 * Standard Free Space:<br/>
 * ------------------------------------------------------------------------------------<br/>
 * |.CodeBlock.|.nextPointer.|.........Free Space...........|.nextPointer.|.CodeBlock.|<br/>
 * |.min 1byte.|....4byte....|.max PAGE_SIZE - 10 byte byte.|....4byte....|.min 1byte.|<br/>
 * ------------------------------------------------------------------------------------<br/>
 *<br/>
 * 6byte Free Space:<br/>
 * ---------------------------------------<br/>
 * |.CodeBlock.|.nextPointer.|.CodeBlock.|<br/>
 * |.min 1byte.|....4byte....|.min 1byte.|<br/>
 * ---------------------------------------<br/>
 *<br/>
 * Occupied space Space:<br/>
 * ------------------------------------------------------------------<br/>
 * |.CodeBlock.|...................Data.................|.CodeBlock.|<br/>
 * |.min 1byte.|6byte to (max PAGE_SIZE - 10 byte) byte |.min 1byte.|<br/>
 * ------------------------------------------------------------------<br/>
 * </code>
 */
class Space {
protected:
    bool copyCodeBlocAtEnd(byte* startOfBlock, size_t sizeOfBlock);

public:
    /**
     * @return The size of the entire space block, including management informatin
     */
    size_t getSize();
    virtual bool isFreeMemory() = 0;
    virtual bool isOccupiedMemory() = 0;
    /**
     * @return the start of the space which could be actual data. <br/>
     * WARNING: if this Space is freeSpace, this pointer might point on the next pointer of the of this free space.
     * This place should only be used if the block is occupied or is immediatly converted to occupied space.
     */
    void* getStartOfSpace();

    /**
     * @return the leftmost byte of the entire block, including management information
     */
    byte *getLeftMostEnd() const;
    /**
     * @return the rightmost byte of the entire block, including management information
     */
    byte *getRightMostEnd() const;

    /**
     * Returns the size of a code block for a memory block with the given size. Does not change any actual codeBlocks
     * @param sizeOfBlockOfInterestInByte
     * @return size of a codeblock for a given memory block
     */
    static size_t computeCodeBlockSize(size_t sizeOfBlockOfInterestInByte);

    /**
     * Takes a a Space and returns a Space interpreted as Ocuupied. The code blocks are adapted acordingly.
     * @param neeSize
     * @return
     */
    static OccupiedSpace* toOccupied(Space*, size_t newSize);

};


#endif //MARA_SPACE_H
