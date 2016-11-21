//
// Created by tom on 15.11.16.
//

#ifndef MARA_FREESPACE_H
#define MARA_FREESPACE_H


#include <stdint.h>
#include "Space.h"

class FreeSpace : public Space{
private:
    /**
     * The Offset to the next free space, starting from the right most end of this space.
     * 4 bytes long
     */
    uint32_t* nextOfsetFromLastByte;

public:
    virtual bool isFreeMemory() override;
    virtual bool isOccupiedMemory() override;

    FreeSpace* getNext();
    void setNext(FreeSpace* next);

    /**
     * Adjust the size of the fre space.<br/>
     * The right most end will be the same as before. The left most end will be the given byte. The pointer will
     * @param firstByte
     * @return
     */
    FreeSpace* resize(byte* firstByte);
};


#endif //MARA_FREESPACE_H
