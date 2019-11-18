#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 1024
#define BLOCKS 2048
#define FAT_SIZE (BLOCKS * 2)
#define FAT_BLOCKS (FAT_SIZE / BLOCK_SIZE)
#define ROOT_BLOCK FAT_BLOCKS
#define DIR_ENTRY_SIZE 32
#define DIR_ENTRIES (BLOCK_SIZE / DIR_ENTRY_SIZE)

/* FAT data structure */

int16_t fat[BLOCKS];

/* data block */

int8_t data_block[BLOCK_SIZE];

/* directory entry */
struct dir_entry_s
{
    int8_t filename[25];
    int8_t attributes;
    int16_t first_block;
    int32_t size;
};

void write(char *path, char *pathname, char *filename, char *string);

void write_dir_entry(int block, int entry, struct dir_entry_s *dir_entry);

void read_dir_entry(int32_t block, int32_t entry, struct dir_entry_s *dir_entry);

void write_fat(char *file, int16_t *fat);

void read_fat(char *file, int16_t *fat);

void write_block(char *file, int32_t block, int8_t *record);

void read_block(char *file, int32_t block, int8_t *record);

int8_t findFreeSpace(int8_t block);

int8_t findByBlock(int8_t block, char *name);

int16_t verifyPath(char *path);

char *getPathname(char *path);

char *getFilename(char *path);

char *getLastWord(char *name);

int8_t findByName(int16_t block, char *name);

int startsWith(char *string, char *pre);

int isEmpty(int8_t block);