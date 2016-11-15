//
// Created by tom on 15.11.16.
//

#ifndef MARA_BUCKETLIST_H
#define MARA_BUCKETLIST_H


#include <cstdlib>
#include "FreeSpace.h"

class BucketList {
private:
    enum SIZE_CLASS{
        BYTE4,
        BYTE8,
        BYTE12,
        BYTE16,
        BYTE20,
        BYTE24,
        BYTE28,
        BYTE32,
        BYTE40,
        BYTE48,
        BYTE56,
        BYTE64,
        BYTE128,
        BYTE265,
        BYTE512,
        BYTE_MORE_THAN512
    };

    /**
     * The array with th e information of the dynamic free sections
     * <br/><br/>
     * The space pointed to at the given index is the first one of the size class.  <br/>
     * Each index represent another size class. Increasing indices represent increasing size classes.
     */
    FreeSpace* bucketList[BYTE_MORE_THAN512+1];

public:
    FreeSpace* getFreeSpace(size_t sizeInByte);

};


#endif //MARA_BUCKETLIST_H
