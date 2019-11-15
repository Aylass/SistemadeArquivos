#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

int8_t findFreeSpace(int8_t block){
    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        if(strcmp("", dir_entry.filename) == 0)
            return i;
    }
    return -1;
}

int isEmpty(int8_t block){
    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        printf("%s", dir_entry.filename);
        if(strcmp("", dir_entry.filename) != 0)
            return 0;//ta vazio
    }
    return 1;//tem coisa
}

int startsWith(char* string,char* pre){
    return strncmp(pre, string, strlen(pre)) == 0;
}
int8_t findByName(int16_t block, char* name){
    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        if(strcmp(name, dir_entry.filename) == 0)
            return dir_entry.first_block;
    }
    return -1;
}
char* getLastWord(char* name){
    char* token;
    char* aux;
    aux = strtok(name, " ");
   while( aux != NULL ) {
        token = aux;
        aux = strtok(NULL, " ");
   }
   int size = strlen(token);
   token[size-1] = '\0';
   return token;
}
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
char * getFilename(char* path){
        char *dirc, *basec, *bname, *dname;
           basec = strdup(path);
           bname = basename(basec);
           return bname;
}
char * getPathname(char* path){
        char *dirc, *basec, *bname, *dname;
           dirc = strdup(path);
           dname = dirname(dirc);
           return dname;
}
int main(void)
{
    shell();
    return 0;
}

int16_t verifyPath(char* path){
    if(strlen(path) == 1 && path[0] == '/'){
        return ROOT_BLOCK;
    }
    if(path[0] == '/' && strlen(path) > 1)
        path = &path[1];
    char* aux;
    char* del = "/";
    int16_t lPath = ROOT_BLOCK;
    aux = strtok(path, del);
    while(aux != NULL)
    {
        int verifier = findByName(lPath,aux);
        if(verifier != -1){
            lPath = verifier;
        }
        else{
            printf("Path not found.\n", path);
            lPath = -1;
            break;
        }
        aux = strtok(NULL, del);
   }
   return lPath;
}


//void removeLastBackSlash(char* path){
//    int size = strlen(path)-2;
//    while(path[size] != '/'){
//        size--;
//    }
//    path[size] = '\0';
//}

void verifyDir(){
 return;
}
void shell()
{
    char input[255];
    do
    {
        printf(">");
        fgets(input, sizeof input, stdin);
        if (startsWith(input, "init"))
        {
            init();
        }
        else if (startsWith(input, "mkdir")) //NEED TO FIX THE PATH PROBLEM.... BUT WORKING!
        {
            char* name = getLastWord(input);
            char* filename = getFilename(name);
            char* pathname = getPathname(name);
            int16_t block = verifyPath(pathname);
            if(block != -1){
                mkdir(block,filename);
            }
        }
        else if (startsWith(input, "ls")) // FUNCTIONAL, NEED TO CATCH EXCEPTIONS
        {
            char* name = getLastWord(input);

            int16_t res = verifyPath(name);
            if(res != -1)
                ls(res);
        }
         else if (startsWith(input, "test"))
        {

        }
        else if (startsWith(input, "unlink"))
        {
            char* name = getLastWord(input);
            char* filename = getFilename(name);
            char* pathname = getPathname(name);

            unLink(name,pathname,filename);
        }
        else if (startsWith(input, "create"))
        {
            char* name = getLastWord(input);
            char* filename = getFilename(name);
            char* pathname = getPathname(name);
            int16_t block = verifyPath(pathname);
            if(block != -1){
                create(block,filename);
            }
        }
        else if (startsWith(input, "write"))
        {
            char* path = getLastWord(input);
            char* filename = getFilename(path);
            char* pathname = getPathname(path);
            char* string = "pamela larinha dolinho";
            write(path,pathname,filename,string);

        }
        else if (startsWith(input, "exit"))
        {
            exit(0);
        }
        else
        {
            printf("Not recognized. Try digit 'help' for see all commands\n");
        }

    } while (1);
}

void write(char* path, char* pathname, char* filename, char* string){
    int size = 0;
    int16_t block = verifyPath(path);
    printf("%s", path);
    write_block("filesystem.dat",block,data_block);
    for(size; size < strlen(string);size++){
        data_block[size] = (int8_t)string[size];
    }

    write_block("filesystem.dat",block,data_block);
}

void removeFat(char *file, int16_t position)
{
    read_fat(file,fat);
    fat[position] = 0;
    write_fat(file,fat);
}

int8_t findByBlock(int8_t block,char* name){

    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        if(strcmp(dir_entry.filename, name) == 0){
           return i;
        }
    }
    return -1;
}

void unLink(char* path, char* pathname, char* filename){
    int16_t block;
    block = verifyPath(path);
    if(block == -1){

        return;
    }
    int aux = isEmpty(block);
    if(aux == 1){
    }else{
        printf("Is not empty");
        return;
    }
    removeFat("filesystem.dat",block);
    struct dir_entry_s dir_entry;

    memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
    strcpy((char *)dir_entry.filename, "");
    dir_entry.attributes = 0;
    dir_entry.first_block = 0;
    dir_entry.size = 0;

    int16_t blockpai;
    blockpai = verifyPath(pathname);
    int8_t index = findByBlock(blockpai,filename);

    write_dir_entry(blockpai, index , &dir_entry);
}

int32_t getSpaceFAT()
{ //descobrir espa�o vazio e retorna a posi��o

    int32_t i;
    for (i = ROOT_BLOCK + 1; i < BLOCKS; i++){
        if (fat[i] == 0)
            return i;
    }
    return -1;

}

int8_t getFreeBlock(int8_t* block){

    int8_t i;
    for(i = 0; i < BLOCK_SIZE;i++){
        if (!block[i])
            return i;
    }
    return -1;
}

void updateFAT(char *file, int16_t position)
{
    read_fat(file,fat);
    fat[position] = 0x7fff;
    write_fat(file,fat);
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
void mkdir(int16_t block,char* name)
{
    if(findByName(block,name) != -1){
        printf("already exist with same name '%s'. \n", name);
        return;
    }
    int8_t space = findFreeSpace(block);
    if(space == -1){
        printf("Space unavailable");
        return;
    }
    struct dir_entry_s dir_entry;


    memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
    strcpy((char *)dir_entry.filename, name);
    dir_entry.attributes = 0x02;
    dir_entry.first_block = getSpaceFAT();
    dir_entry.size = 0;

    write_dir_entry(block, space , &dir_entry);
    write_block("filesystem.dat", block, data_block);
    updateFAT("filesystem.dat",dir_entry.first_block);
}

void create(int16_t block,char* name)
{
    if(findByName(block,name) != -1){
        printf("already exist with same name '%s'. \n", name);
        return;
    }
    int8_t space = findFreeSpace(block);
    if(space == -1){
        printf("Space unavailable");
        return;
    }
    struct dir_entry_s dir_entry;


    memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
    strcpy((char *)dir_entry.filename, name);
    dir_entry.attributes = 0x01;
    dir_entry.first_block = getSpaceFAT();
    dir_entry.size = 0;

    write_dir_entry(block, space , &dir_entry);
    write_block("filesystem.dat", block, data_block);
    updateFAT("filesystem.dat",dir_entry.first_block);
}

void ls(int8_t block){

    int8_t i;
    struct dir_entry_s dir_entry;
    for (i = 0; i < DIR_ENTRIES; i++)
    {
        read_dir_entry(block, i, &dir_entry);
        printf("\nFilename :%s Size :%d First Block :%d\n",dir_entry.filename, dir_entry.size, dir_entry.first_block);
    }
}



