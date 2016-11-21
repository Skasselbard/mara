//
// Created by tom on 15.11.16.
//

#ifndef MARA_FREESPACE_H
#define MARA_FREESPACE_H


#include <stdint.h>
#include "Space.h"

class FreeSpace : public Space{
private:
    bool copyNextPointerFromEndToFront(uint32_t* front, uint32_t* end);
    uint32_t* getLeftNext(size_t codeBlockSize);
    uint32_t* getRightNext(size_t codeBlockSize);

public:
    /**
     * @return a pointer to the next FreeSpace in the bucket. The left next pointer will be taken as reference
     */
    FreeSpace* getNext();

    /**
     * Adapt the next pointer in the data structure. The next pointer is adjacent to the code blocks and stored
     * as a 4 byte integer interpreted as offset from the last byte (to the right)
     * @param next pointer to the next free space
     */
    void setNext(FreeSpace* next);

    /**
     * Adjust the size of the fre space.<br/>
     * The right most end will be the same as before. The left most end will be the given byte. The next pointer
     * from the free space will be copied from the right to the left
     * @param firstByte the new first byte of the space
     * @return a pointer the the left most byte of the free space (should be the same as the input)
     */
    FreeSpace* resize(byte* firstByte);
};


#endif //MARA_FREESPACE_H
