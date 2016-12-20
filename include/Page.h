//
// Created by tom on 14.11.16.
//

#ifndef MARA_PAGE_H
#define MARA_PAGE_H



#include <stddef.h>
#include "BucketList.h"
#include "predefined.h"

class Space;
class FreeSpace;
class OccupiedSpace;

class Page {
private:
    /**
     * Pointer to the first byte of the page
     */
    void *startOfPage;

    /**
     * Pointer to the next page
     */
    Page *nextPage;

    /**
     * pointer to the leftmost byte of the static sector <br/>
     * the rightmost byte is the last byte of the page
     */
    byte* staticEnd;

    /**
     * pointer to the rightmost allocated byte of the dynamic sector <br/>
     * behind this pointer can only be an allocated chunk form the static
     * sector. space between this pointer and the staticEnd pointer has to be free memory.
     */
    byte* dynamicEnd;

    BucketList bucketList;




    /**
     * TODO: describe alignment
     * align the requested block size
     * @param requestedSizeInByte the needed size for the requested block
     * @return the aligned size of the requested block
     */
    size_t align(size_t requestedSizeInByte);

    /**
     * Merges the three blocks into one Block of free Space.
     * The bucketList will be updated accordingly<br/>
     * WARNING: the blocks have to be adjacent to each other. Merging distant blocks will cause undefined behavior.
     * Probably causing the world as we know it, cease to exist!
     * @param leftBlock leftBlock to be merged. Ignored if null
     * @param middleBlock middle Block to be merged
     * @param rightBlock right Block to be merged. Ignored if null
     *
     * @return the new block of free space
     */
    FreeSpace * mergeFreeSpace(Space *leftBlock, Space *middleBlock, Space *rightBlock);

    /**
     * Merges both blocks to one. The types of Blocks are ignored.
     * @param leftBlock
     * @param middleBlock
     */
    void mergeWithLeft(Space* leftBlock, Space* middleBlock);

    /**
     * Merges both blocks to one. The types of Blocks are ignored.
     * @param leftBlock
     * @param middleBlock
     */
    void mergeWithRight(Space* middleBlock, Space* rightBlock);

    /**
     * Takes free space und cut the specified amount from space, starting at the left end. The new block has the adapted
     * code blocks with the new size.
     * @param freeSpace space to be cut
     * @param bytesToCutOf amount of bytes to cut off from the left
     * @return null if the resulting block would be smaller than the smallest addressable block. A pointer to the
     * resulting block otherwise
     */
    FreeSpace* cutLeftFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf);

    /**
     * Takes free space und cut the specified amount from space, starting at the right end. The new block has the adapted
     * code blocks with the new size.
     * @param freeSpace space to be cut
     * @param bytesToCutOf amount of bytes to cut off from the left
     * @return null if the resulting block would be smaller than the smallest addressable block. A pointer to the
     * resulting block otherwise
     */
    FreeSpace* cutRightFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf);

    FreeSpace* generateFirstBucketEntry();

    void initializeBucketList();


public:
    Page(size_t sizeInBytes);

    /**
     * should be private destructor because we dont expect pages to be deleted before program termination
     */
    ~Page();

    /**
    * the size in byte with which this page was initialized
    */
    const size_t pageSize;

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

    OccupiedSpace * getDynamicBlock(size_t sizeInByte);

    Page* getNextPage();
    void setNextPage(Page* nextPage);

    /**
     * @param firstByte a pointer to the block of interest
     * @return true if the pointer is in between the start of page and the left most byte of the static sector.
     * false otherwise. Blocks in the static sector CANNOT be detected with this function.
     */
    bool blockIsInSpace(void* firstByte);


    bool deleteBlock(void* firstByte);

    void * getStartOfPage();

};


#endif //MARA_PAGE_H
