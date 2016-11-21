//
// Created by tom on 14.11.16.
//

#ifndef MARA_INTERFACE_H
#define MARA_INTERFACE_H

int setDefaultBlockSize(size_t sizeInByte);
void* staticNew(size_t sizeInByte);
void* dynamicNew(size_t sizeInByte);
int dynamicDelete(void* address);

#endif //MARA_INTERFACE_H
