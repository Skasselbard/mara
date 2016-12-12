//
// Created by tom on 15.11.16.
//

#ifndef MARA_BUCKETLIST_H
#define MARA_BUCKETLIST_H


#include <cstdlib>
#include <tgmath.h>
#include "FreeSpace.h"

class BucketList {
private:
    static const size_t lastLinear4Scaling = 32;
    static const size_t lastLinear16Scaling = 128;
    static const size_t largestBucketSize = 1024;

    static const size_t blSize = lastLinear4Scaling/4
                                       + (lastLinear16Scaling-lastLinear4Scaling+4)/16
                                       + (const size_t) (log2(largestBucketSize)-log2(lastLinear16Scaling));

    /**
     * The array with th e information of the dynamic free sections
     * <br/><br/>
     * The space pointed to at the given index is the first one of the size class.  <br/>
     * Each index represent another size class. Increasing indices represent increasing size classes.
     */
    FreeSpace* bucketList[blSize];

    byte* startOfPage;

private:

    /**
     * @param freeSpace the Space to search for
     * @return null if not i list, same pointer if found.
     */
    FreeSpace* searchInList(FreeSpace* freeSpace, FreeSpace* &predecessor);

    /**
     * @param size
     * @return null if bucket is empty, the last element otherwise
     */
    FreeSpace* getLastInBucket(size_t size);

    /**
     * Get the correct index in the bucket list for a block with the given size
     */
    static unsigned int lookupBucket(size_t size) ;

    /**
     * @param index start index to search. The returned index will greater or equal to this index.
     * @return a bucket index with a non null entry. The index will always be >= the given index.
     */
    unsigned int findNonEmptyBucket(unsigned int index);

    /**
     * @param minimumSize count of bytes the space has to have at a minimum
     * @param index the index of the bucketlist, where to search
     * @return null if no fitting space is found in the bucket, a freeSpace with a size greater than byte
     */
    FreeSpace *findFittingSpaceInBucket(size_t minimumSize, unsigned int index);


public:
    /**
     * This function does only give a freeSpace of the page. It does not alter the list itself.
     * @param sizeInByte of the block of interest
     * @return null if there was no fitting space found. A pointer to the first free space in the list Otherwise.
     */
    FreeSpace* getFreeSpace(size_t sizeInByte);

    bool deleteFromList(FreeSpace* freeSpace);
    bool addToList(FreeSpace* freeSpace);

    void setStartOfPage(byte *startOfPage);

};


#endif //MARA_BUCKETLIST_H
