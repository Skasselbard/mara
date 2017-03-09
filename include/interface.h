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
int setDefaultBlockSize(size_t sizeInByte);

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

/**
 * Reserves memory in the dynamic sector. Memory in this sector can be freed using the dynamicDelete method.
 * @param sizeInByte how many bytes shall be reserved
 * @return a pointer to the first byte in a reserved space with at least the requested size
 */
void* dynamicNew(size_t sizeInByte);

/**
 * frees a previously reserved space in the dynamic sector
 * @param address the pointer that was returned by dynamicNew
 * @return true if the operation was successful, false elsewhen
 */
int dynamicDelete(void* address);

#endif //MARA_INTERFACE_H
