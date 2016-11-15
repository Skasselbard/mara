//
// Created by tom on 14.11.16.
//

#ifndef MARA_PAGE_H
#define MARA_PAGE_H


#include <cstddef>

typedef char byte;

class Page {
private:
    void* startOfPage;
    size_t pageSize;
    byte* staticEnd;
    byte startOfDynamicLocations[];

    ~Page();

public:
    Page(size_t sizeInBytes);
    int getStaticBlock(size_t sizeInByte);

};


#endif //MARA_PAGE_H
