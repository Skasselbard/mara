//
// Created by tom on 14.11.16.
//

#ifndef MARA_INTERFACE_H
#define MARA_INTERFACE_H

/**
 * Sets the default size for blocks allocated from the system. These blocks are used bei mara for internal memory management.
 * Calling staticNew and dynamicNew will allocate memory in one of these blocks. Because of internal structure, the
 * blocks cannot be greater than approximately 4GB.
 * @param sizeInByte default size for malloc calls from mara
 * @return 0 on success
 */
int setPageSize(size_t sizeInByte);

/**
 * Reserves memory in the static sector. Memory in this sector is expected to live as long as the program. Memory
 * allocated with this function CANNOT be freed. Mara returns a pointer to the location with an unused block with the
 * given size and completely ignore this space in the future. The advantage is that these blocks will produce absolutely
 * overhead inside of its page.
 * @param sizeInByte size of the block you want to use
 * @return a pointer to the first byte of the block you want to use. After this operation the block will stay allocated
 * until complete program termination.
 */
void* staticNew(size_t sizeInByte);

#endif //MARA_INTERFACE_H
