#include "dataManipulator.h"

void shell();
void init();
void ls(int8_t block, char *filename);
char *getWords(char *data);

void unLink(char *path, char *pathname, char *filename);

int main(void)
{
    shell();
    return 0;
}
void shell()
{
    char input[255];
    do
    {
        printf(">");
        fgets(input, sizeof input, stdin);
        if (strstr(input, "init") != NULL)
        {
            init();
        }
        else if (strstr(input, "mkdir") != NULL) //NEED TO FIX THE PATH PROBLEM.... BUT WORKING!
        {

            char *name = getLastWord(input);
            char *filename = getFilename(name);
            char *pathname = getPathname(name);
            int16_t block = verifyPath(pathname);
            if (block != -1)
            {
                mkdir(block, filename);
            }
        }
        else if (strstr(input, "ls") != NULL) // FUNCTIONAL, NEED TO CATCH EXCEPTIONS
        {
            char *name = getLastWord(input);
            int16_t res = verifyPath(name);
            char *filename = getFilename(name);
            if (res != -1)
            {
                ls(res, filename);
            }
        }
        else if (strstr(input, "unlink") != NULL)
        {
            char *name = getLastWord(input);
            char *filename = getFilename(name);
            char *pathname = getPathname(name);
            int16_t block = verifyPath(pathname);
            if (block != -1)
            {
                unLink(name, pathname, filename);
            }
        }
        else if (strstr(input, "create") != NULL)
        {
            char *name = getLastWord(input);
            char *filename = getFilename(name);
            char *pathname = getPathname(name);
            int16_t block = verifyPath(pathname);
            if (block != -1)
            {
                create(block, filename);
            }
        }
        else if (strstr(input, "write") != NULL)
        {
            char *path = getLastWord(input);
            char *filename = getFilename(path);
            char *pathname = getPathname(path);
            char* string = getWords(input);
            write(path, pathname, filename, string);
        }
        else if (strstr(input, "append") != NULL)
        {
            char *path = getLastWord(input);
            char *filename = getFilename(path);
            char *pathname = getPathname(path);
            char* string = getWords(input);
            append(path, pathname, filename, string);
        }
        else if (strstr(input, "read") != NULL)
        {
            char *path = getLastWord(input);
            char *filename = getFilename(path);
            char *pathname = getPathname(path);
            read(pathname, filename);
        }
        else if (strstr(input, "exit") != NULL)
        {
            exit(0);
        }
        else if (strstr(input, "test") != NULL)
        {
            char* test = getWords(input);
            printf("%s", test);
        }
        else
        {
            printf("Not recognized. Try digit 'help' for see all commands\n");
        }

    } while (1);
}

void removeFat(char *file, int16_t position)
{
    read_fat(file, fat);
    fat[position] = 0;
    write_fat(file, fat);
}

void unLink(char *path, char *pathname, char *filename)
{
    int16_t block;
    block = verifyPath(path);
    if (block == -1)
    {

        return;
    }
    int aux = isEmpty(block);
    if (aux == 1)
    {
    }
    else
    {
        printf("Is not empty");
        return;
    }
    removeFat("filesystem.dat", block);
    struct dir_entry_s dir_entry;

    memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
    strcpy((char *)dir_entry.filename, "");
    dir_entry.attributes = 0;
    dir_entry.first_block = 0;
    dir_entry.size = 0;

    int16_t blockpai;
    blockpai = verifyPath(pathname);
    int8_t index = findByName(blockpai, filename);

    write_dir_entry(blockpai, index, &dir_entry);
}

int32_t getSpaceFAT()
{ //descobrir espa�o vazio e retorna a posi��o
    read_fat("filesystem.dat", fat);
    int32_t i;
    for (i = ROOT_BLOCK + 1; i < BLOCKS; i++)
    {
        if (fat[i] == 0)
            return i;
    }
    return -1;
}

int8_t getFreeBlock(int8_t *block)
{

    int8_t i;
    for (i = 0; i < BLOCK_SIZE; i++)
    {
        if (!block[i])
            return i;
    }
    return -1;
}

void updateFAT(char *file, int16_t position)
{
    read_fat(file, fat);
    fat[position] = 0x7fff;
    //printf("fat[%d] = %d",position, fat[position]);
    write_fat(file, fat);
}

void init()
{
    FILE *f;
    int32_t i;
    /* create filesystem.dat if it doesn't exist */
    f = fopen("filesystem.dat", "a");
    fclose(f);

    /* initialize the FAT */
    for (i = 0; i < FAT_BLOCKS; i++)
        fat[i] = 0x7ffe;
    fat[ROOT_BLOCK] = 0x7fff;
    for (i = ROOT_BLOCK + 1; i < BLOCKS; i++)
        fat[i] = 0;

    /* write it to disk */
    write_fat("filesystem.dat", fat);

    /* initialize an empty data block */
    for (i = 0; i < BLOCK_SIZE; i++)
        data_block[i] = 0;

    /* write an empty ROOT directory block */
    write_block("filesystem.dat", ROOT_BLOCK, data_block);

    /* write the remaining data blocks to disk */
    for (i = ROOT_BLOCK + 1; i < BLOCKS; i++)
        write_block("filesystem.dat", i, data_block);
}
void mkdir(int16_t block, char *name)
{
    if (findByName(block, name) != -1)
    {
        printf("already exist with same name '%s'. \n", name);
        return;
    }
    int8_t space = getBlockEntry(block);
    if (space == -1)
    {
        printf("Space unavailable");
        return;
    }
    struct dir_entry_s dir_entry;

    memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
    strcpy((char *)dir_entry.filename, name);
    dir_entry.attributes = 0x02;
    dir_entry.first_block = getSpaceFAT();
    dir_entry.size = 0;

    write_dir_entry(block, space, &dir_entry);
    write_block("filesystem.dat", block, data_block);
    updateFAT("filesystem.dat", dir_entry.first_block);
}

void create(int16_t block, char *name)
{

    if (findByName(block, name) != -1)
    {
        printf("already exist with same name '%s'. \n", name);
        return;
    }

    int8_t space = getBlockEntry(block);
    if (space == -1)
    {
        printf("Space unavailable");
        return;
    }
    struct dir_entry_s dir_entry;

    memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
    strcpy((char *)dir_entry.filename, name);
    dir_entry.attributes = 0x01;
    dir_entry.first_block = getSpaceFAT();
    dir_entry.size = 0;
    write_dir_entry(block, space, &dir_entry);
    write_block("filesystem.dat", block, data_block);
    updateFAT("filesystem.dat", dir_entry.first_block);
}

void ls(int8_t block, char *filename)
{
    if (!isADir(block, filename))
    {
        return;
    }
    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        printf("\nFilename :%s Size :%d First Block :%d\n", dir_entry.filename, dir_entry.size, dir_entry.first_block);
    }
}

void write(char *path, char *pathname, char *filename, char *string)
{
    int16_t block = verifyPath(pathname);
    if (block == -1)
    {
        return;
    }

    if (isADir(block, filename) == 1)
    {
        printf("it is a directory.");
        return;
    }

    int32_t fileBlock = findByName(block, filename);
    if (fileBlock == -1)
        printf("file does not exists.");

    for (int size = 0; size < strlen(string); size++)
    {
        data_block[size] = (int8_t)string[size];
    }
    write_block("filesystem.dat", fileBlock, data_block);
}

void read(char *pathname, char *filename)
{
    int16_t block = verifyPath(pathname);
    if (block == -1)
    {
        return;
    }

    if (isADir(block, filename) == 1)
    {
        printf("it is a directory.");
        return;
    }

    int32_t fileBlock = findByName(block, filename);

    read_block("filesystem.dat", fileBlock, data_block);
    for (int size = 0; size < 30; size++)
    {
        printf("%c", data_block[size]);
    }
    printf("\n");
}
void append(char *path, char *pathname, char *filename, char *string)
{

    int16_t block = verifyPath(pathname);
    if (block == -1)
    {
        return;
    }
    if (isADir(block, filename) == 1)
    {
        printf("it is a directory.");
        return;
    }

    int32_t fileBlock = findByName(block, filename);
    if (fileBlock == -1)
        fileBlock = getSpaceFAT();

    read_block("filesystem.dat", fileBlock, data_block);

    int lastPos = 0;
    for (lastPos; data_block[lastPos] != 0; lastPos++)
        printf("%c", data_block[lastPos]);

    for (int size = lastPos, indexString = 0; size < BLOCK_SIZE && indexString < strlen(string); size++, indexString++)
    {
        data_block[size] = (int8_t)string[indexString];
    }
    write_block("filesystem.dat", fileBlock, data_block);
}