#include "basicOperation.h"
void read_block(char *file, int32_t block, int8_t *record)
{
    FILE *f;

    f = fopen(file, "r+");
    fseek(f, block * BLOCK_SIZE, SEEK_SET);
    fread(record, 1, BLOCK_SIZE, f);
    fclose(f);
}
void write_block(char *file, int32_t block, int8_t *record)
{
    FILE *f;

    f = fopen(file, "r+");
    fseek(f, block * BLOCK_SIZE, SEEK_SET);
    fwrite(record, 1, BLOCK_SIZE, f);
    fclose(f);
}
