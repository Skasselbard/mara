//
// Created by tom on 14.11.16.
//

#ifndef MARA_PAGE_H
#define MARA_PAGE_H

#define ALLIGNMENT
#ifdef ALLIGNMENT
    #define ALLIGN_STATIC
    #define ALLIGN_DAYNAMIC
#endif

#include <stddef.h>

typedef char byte;

class Page {
private:
    /**
     * Pointer to the first byte of the page
     * TODO: See if needed
     */
    void* startOfPage;

    /**
     * the size in byte with which this page was initialized
     */
    const size_t pageSize;

    /**
     * pointer to the leftmost byte of the static sector
     * the rightmost byte is the last byte of the page
     */
    byte* staticEnd;

    /**
     * The size of all static blocks combined
     */
    size_t staticSectorSize;

    /**
     * The array with th e information of the dynamic free sections
     */
    byte startOfDynamicLocations[];



    /**
     * Free blocks in between the dynamic blocks are included in the size
     * @return the size in bytes which the dynamic sector spans
     */
    size_t getDynamicSectorSize();



    /**
     * TODO: discribe allignment
     * allign the requestet block size
     * @param requestetSizeInByte the needed size for the requested block
     * @return the allignet size of the requested block
     */
    size_t allign(size_t requestetSizeInByte);

public:
    Page(size_t sizeInBytes);

    /**
     * shuold be private destructor because we dont expect pages to be deleted before program termination
     */
    ~Page();

    /**
     * returns a new static block
     * @param sizeInByte
     * @return
     */
    void* getStaticBlock(size_t sizeInByte);
    /**
     * returns if a requested block size would fit in the page
     */
    bool staticBlockFitInPage(size_t blockSizeInByte);

};


#endif //MARA_PAGE_H