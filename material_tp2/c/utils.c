#include "utils.h"

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
    bname = (char *)basename(basec);
    return bname;
}
char *getPathname(char *path)
{
    char *dirc, *basec, *bname, *dname;
    dirc = strdup(path);
    dname = (char *)dirname(dirc);
    return dname;
}

int16_t verifyPath(char *path)
{
    printf("%s", path);
    // int result = strlen(path);
    if (path[0] == '/')
    {
        // return ROOT_BLOCK;
    }
    // if (path[0] == '/' && strlen(path) > 1)
    //     path = &path[1];
    // char *aux;
    // char *del = "/";
    // int16_t lPath = ROOT_BLOCK;
    // aux = strtok(path, del);
    // while (aux != NULL)
    // {
    //     int verifier = findByName(lPath, aux);
    //     if (verifier != -1)
    //     {
    //         lPath = verifier;
    //     }
    //     else
    //     {
    //         printf("Path not found.\n");
    //         lPath = -1;
    //         break;
    //     }
    //     aux = strtok(NULL, del);
    // }
    // return lPath;
}

int8_t findByBlock(int8_t block, char *name)
{

    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        if (strcmp(dir_entry.filename, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int8_t findFreeSpace(int8_t block)
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