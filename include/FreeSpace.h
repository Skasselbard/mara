//
// Created by tom on 15.11.16.
//

#ifndef MARA_FREESPACE_H
#define MARA_FREESPACE_H


#include "Space.h"

class FreeSpace : public Space{
public:
    virtual bool isFreeMemory() override;
    virtual bool isOccupiedMemory() override;

    FreeSpace* getNext();
    void setNext(FreeSpace* next);
};


#endif //MARA_FREESPACE_H
