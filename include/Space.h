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
public:
    /**
     * @return The size of the entire space block, including management informatin
     */
    size_t getSize();
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
     * @param newSize the size to new block should have
     * @return a pointer to the new spece with updated codeblocks
     */
    void toOccupied(size_t newSize);

    /**
     * Copies a code block from the beginning of space to the end of space
     * @param startOfBlock beginning of the block to copy
     * @param sizeOfBlock amount of bytes the block uses
     * @return true on success
     */
    bool copyCodeBlockToEnd(byte *startOfBlock, size_t sizeOfBlock);

    /**
     * Copies a code block from the end of space to the beginning of space
     * @param startOfBlock beginning of the block to copy
     * @param sizeOfBlock amount of bytes the block uses
     * @return true on success
     */
    bool copyCodeBlockToFront(byte *startOfBlock, size_t sizeOfBlock);

    /**
     * @param lastByte of the left neighbor
     * @return pointer to the left neighboor
     */
    static Space* getleftNeighbor(byte* lastByte);


};


#endif //MARA_SPACE_H
