//
// Created by jg on 12/20/16.
//

#ifndef MARA_MEMDUMP_H
#define MARA_MEMDUMP_H

class MemDump {

private:

    static const int bytesPerLine;

public:

    static void dumpFullBlock(void * left);

    static void dumpCodeBlock(void * left);

    static void dumpMemoryBlock(void * left);

    static void dumpArray(void * left, size_t size);

};


#endif //MARA_MEMDUMP_H
