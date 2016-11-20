//
// Created by tom on 20.11.16.
//

#ifndef MARA_SPACE_H
#define MARA_SPACE_H


#include <glob.h>
#include "Page.h"


/**
 * Basic Structure:<br/>
 * <code>
 * Standard Free Space:
 * ------------------------------------------------------------------------------------<br/>
 * |.CodeBlock.|.nextPointer.|.........Free Space...........|.nextPointer.|.CodeBlock.|<br/>
 * |.min 1byte.|....4byte....|.max PAGE_SIZE - 10 byte byte.|....4byte....|.min 1byte.|<br/>
 * ------------------------------------------------------------------------------------<br/>
 *
 * 6byte Free Space:
 * ---------------------------------------<br/>
 * |.CodeBlock.|.nextPointer.|.CodeBlock.|<br/>
 * |.min 1byte.|....4byte....|.min 1byte.|<br/>
 * ---------------------------------------<br/>
 *
 * Occupied space Space:
 * --------------------------------------------------------<br/>
 * |.CodeBlock.|............Data..............|.CodeBlock.|<br/>
 * |.min 1byte.|.max PAGE_SIZE - 10 byte byte |.min 1byte.|<br/>
 * --------------------------------------------------------<br/>
 * </code>
 */
class Space {
private:
    byte* leftCodeBlock;
    byte* rightCodeBlock;
    /**
     * the leftmost byte of the entire block, including management information
     */
    byte* leftMostEnd;
    /**
     * the rightmost byte of the entire block, including management information
     */
    byte* rightMostEnd;

public:
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

};


#endif //MARA_SPACE_H
