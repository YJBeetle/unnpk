#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>

int main(int argc, char ** argv)
{
    printf("unnpk!\n");

    if(argc != 3)
    {
        printf("unnpk file.npk filename\n");
        exit(1);
    }

    char *npk_path = argv[1];
    char *out_path = argv[2];

    //打开npk
    FILE *npk = fopen(npk_path, "rb");
    if (npk == NULL)
    {
        printf("npk file open failed\n");
        exit(1);
    }

    //输出文件夹
    if (out_path[strlen(out_path) - 1] == '/')
        out_path[strlen(out_path) - 1] = 0;
    if (mkdir(out_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
    {
        printf("mkdir failed\n");
        exit(1);
    }

    //读取文件大小
    fseek(npk, 0L, SEEK_END);
    uint32_t npk_size = ftell(npk);

    //读取map偏移量
    fseek(npk, 0x14, SEEK_SET);
    uint32_t map_offset;
    fread(&map_offset, 4, 1, npk);

    //准备读取
    uint32_t file_info[7];
    FILE *file_out = NULL;
    char *file_out_name = malloc(strlen(out_path) + 1 + 8 + 1);
    uint8_t *file_raw = 0;

    printf("Index\tOffset\tSize\tUnknow(maybe check？)\tUnknow\n");
    for (int file_offset = map_offset; file_offset < npk_size; file_offset += 7 * 4)
    {
        //map读取文件信息
        fseek(npk, file_offset, SEEK_SET);
        fread(&file_info, 4, 7, npk);
        printf("%x\t%x\t%x\t%x\t%x\n", file_info[0], file_info[1], file_info[2], file_info[4], file_info[6]);

        sprintf(file_out_name, "%s/%X", out_path, file_info[0]);
        file_out = fopen(file_out_name, "w+");
        fseek(npk, file_info[1], SEEK_SET);
        file_raw = malloc(file_info[2]);
        fread(file_raw, 1, file_info[2], npk);
        fwrite(file_raw, 1, file_info[2], file_out);
        fclose(file_out);
        free(file_raw);
        file_raw = 0;
    }

    fclose(npk);

    return 0;
}
