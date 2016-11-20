//
// Created by tom on 15.11.16.
//

#ifndef MARA_OCCUPIEDSPACE_H
#define MARA_OCCUPIEDSPACE_H


#include "Space.h"

class OccupiedSpace : public Space {
public:
    virtual bool isFreeMemory() override;

    virtual bool isOccupiedMemory() override;

};


#endif //MARA_OCCUPIEDSPACE_H
