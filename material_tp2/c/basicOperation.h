#ifndef BASICOPERATION_H_INCLUDED
#define BASICOPERATION_H_INCLUDED
#include <stdint.h>
#include <stdio.h>
#define BLOCK_SIZE 1024
#define BLOCKS 2048
#define FAT_SIZE (BLOCKS * 2)
#define FAT_BLOCKS (FAT_SIZE / BLOCK_SIZE)
#define ROOT_BLOCK FAT_BLOCKS
#define DIR_ENTRY_SIZE 32
#define DIR_ENTRIES (BLOCK_SIZE / DIR_ENTRY_SIZE)

void read_block(char *file, int32_t block, int8_t *record);
void write_block(char *file, int32_t block, int8_t *record);

#endif // BASICOPERATION_H_INCLUDED
