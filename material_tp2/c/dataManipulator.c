#include "dataManipulator.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
/* reads a data block from disk */

void read_block(char *file, int32_t block, int8_t *record)
{
    FILE *f;

    f = fopen(file, "r+");
    fseek(f, block * BLOCK_SIZE, SEEK_SET);
    fread(record, 1, BLOCK_SIZE, f);
    fclose(f);
}

/* writes a data block to disk */

void write_block(char *file, int32_t block, int8_t *record)
{
    FILE *f;

    f = fopen(file, "r+");
    fseek(f, block * BLOCK_SIZE, SEEK_SET);
    fwrite(record, 1, BLOCK_SIZE, f);
    fclose(f);
}

/* reads the FAT from disk */

void read_fat(char *file, int16_t *fat)
{
    FILE *f;

    f = fopen(file, "r+");
    fseek(f, 0, SEEK_SET);
    fread(fat, 2, BLOCKS, f);
    fclose(f);
}

/* writes the FAT to disk */

void write_fat(char *file, int16_t *fat)
{
    FILE *f;

    f = fopen(file, "r+");
    fseek(f, 0, SEEK_SET);
    fwrite(fat, 2, BLOCKS, f);
    fclose(f);
}

/* reads a directory entry from a directory */

void read_dir_entry(int32_t block, int32_t entry, struct dir_entry_s *dir_entry)
{
    read_block("filesystem.dat", block, data_block);
    memcpy(dir_entry, &data_block[entry * sizeof(struct dir_entry_s)], sizeof(struct dir_entry_s));
}

/* writes a directory entry in a directory */

void write_dir_entry(int block, int entry, struct dir_entry_s *dir_entry)
{

    read_block("filesystem.dat", block, data_block);
    memcpy(&data_block[entry * sizeof(struct dir_entry_s)], dir_entry, sizeof(struct dir_entry_s));
    write_block("filesystem.dat", block, data_block);
}
int isEmpty(int8_t block)
{
    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        printf("%s", dir_entry.filename);
        if (strcmp("", dir_entry.filename) != 0)
            return 0; //tem coisa
    }
    return 1; //ta vazio
}
int startsWith(char *string, char *pre)
{
    return strncmp(pre, string, strlen(pre)) == 0;
}
int8_t findByName(int16_t block, char *name)
{
    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        if (strcmp(name, dir_entry.filename) == 0)
            return dir_entry.first_block;
    }
    return -1;
}
char *getLastWord(char *name)
{
    char *token;
    char *aux;
    aux = strtok(name, " ");
    while (aux != NULL)
    {
        token = aux;
        aux = strtok(NULL, " ");
    }
    int size = strlen(token);
    token[size - 1] = '\0';
    return token;
}
char *getFilename(char *path)
{
    char *dirc, *basec, *bname, *dname;
    basec = strdup(path);
    bname = basename(basec);
    return bname;
}
char *getPathname(char *path)
{
    char *dirc, *basec, *bname, *dname;
    dirc = strdup(path);
    dname = dirname(dirc);
    return dname;
}

int16_t verifyPath(char *path)
{
    int result = strlen(path);
    if (path[0] == '/')
    {
        return ROOT_BLOCK;
    }
    if (path[0] == '/' && strlen(path) > 1)
        path = &path[1];
    char *aux;
    char *del = "/";
    int16_t lPath = ROOT_BLOCK;
    aux = strtok(path, del);
    while (aux != NULL)
    {
        int verifier = findByName(lPath, aux);
        if (verifier != -1)
        {
            lPath = verifier;
        }
        else
        {
            printf("Path not found.\n");
            lPath = -1;
            break;
        }
        aux = strtok(NULL, del);
    }
    return lPath;
}
int8_t getBlockEntry(int8_t block)
{
    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        if (strcmp("", dir_entry.filename) == 0)
            return i;
    }
    return -1;
}