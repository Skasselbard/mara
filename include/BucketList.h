//
// Created by tom on 15.11.16.
//

#ifndef MARA_BUCKETLIST_H
#define MARA_BUCKETLIST_H


#include <cstdlib>
#include "FreeSpace.h"

class BucketList {
private:
    /**
     * Defines the possible sizes of the buckets and the index in the bucketlist. Data in that bucket cannot
     * be greater than its class. Eg. data <= 4 byte in class BYTE4 or 28 byte < data <= 32 byte in class BYTE32
     */
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
        BYTE256,
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

    /**
     * @param freeSpace the Space to search for
     * @return null if not i list, same pointer if found.
     */
    FreeSpace* searchInList(FreeSpace* freeSpace, FreeSpace* &predecessor);

    /**
     * @param sizeClass
     * @return null if bucket is empty, the last element otherwise
     */
    FreeSpace* getLastInBucket(SIZE_CLASS sizeClass);

    /**
     * Get the correct index in the bucket list for a block with the given size
     */
    SIZE_CLASS getCorrectBucket(size_t blockSizeInByte);

public:
    /**
     * This function does only give a freeSpace of the page. It does not alter the list itself.
     * @param sizeInByte of the block of interest
     * @return null if there was no fitting space found. A pointer to the first free space in the list.
     */
    FreeSpace* getFreeSpace(size_t sizeInByte);

    bool deleteFromList(FreeSpace* freeSpace);
    bool addToList(FreeSpace* freeSpace);

};


#endif //MARA_BUCKETLIST_H
