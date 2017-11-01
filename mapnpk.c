#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    printf("# mapnpk!\n");

    if (argc != 2)
    {
        printf("\tHelp: mapnpk npk_file unnpk_direcory\n");
        exit(1);
    }

    char *npk_path = argv[1];

    //打开npk
    FILE *npk = fopen(npk_path, "rb");
    if (npk == NULL)
    {
        fprintf(stderr, "E: npk file open failed\n");
        exit(1);
    }

    //读取文件大小
    fseek(npk, 0L, SEEK_END);
    uint32_t npk_size = ftell(npk);
    printf("\tFile size:\t%d Byte\n", npk_size);

    //读取map偏移量
    fseek(npk, 0x14, SEEK_SET);
    uint32_t map_offset;
    fread(&map_offset, 4, 1, npk);
    printf("\tMap offset:\t%02X%02X%02X%02X (0x%08X)\n",
           ((uint8_t *)(&map_offset))[0],
           ((uint8_t *)(&map_offset))[1],
           ((uint8_t *)(&map_offset))[2],
           ((uint8_t *)(&map_offset))[3],
           map_offset);

    //准备读取
    uint32_t file_info[7];

    printf("| Index\t\t | Offset\t | Size (Byte)\t | Unzip size (Byte)\t | Chk\t\t | Unzip Chk\t | Is zip\t |\n| -\t\t | -\t\t | -\t\t | -\t\t | -\t | -\t\t | -\t\t |\n");
    for (int file_offset = map_offset; file_offset < npk_size; file_offset += 7 * 4)
    {
        //map读取文件信息
        fseek(npk, file_offset, SEEK_SET);
        fread(&file_info, 4, 7, npk);

        //控制台输出文件信息
        printf("| \t%02X%02X%02X%02X\t%d\t0x%08X\t | \t%02X%02X%02X%02X\t%d\t0x%08X\t | \t%02X%02X%02X%02X\t%d\t0x%08X\t | \t%02X%02X%02X%02X\t%d\t0x%08X\t | \t%02X%02X%02X%02X\t%d\t0x%08X\t | \t%02X%02X%02X%02X\t%d\t0x%08X\t | \t%s\t |\n",
               ((uint8_t *)(file_info))[0],
               ((uint8_t *)(file_info))[1],
               ((uint8_t *)(file_info))[2],
               ((uint8_t *)(file_info))[3],
               file_info[0],
               file_info[0],
               ((uint8_t *)(file_info + 1))[0],
               ((uint8_t *)(file_info + 1))[1],
               ((uint8_t *)(file_info + 1))[2],
               ((uint8_t *)(file_info + 1))[3],
               file_info[1],
               file_info[1],
               ((uint8_t *)(file_info + 2))[0],
               ((uint8_t *)(file_info + 2))[1],
               ((uint8_t *)(file_info + 2))[2],
               ((uint8_t *)(file_info + 2))[3],
               file_info[2],
               file_info[2],
               ((uint8_t *)(file_info + 3))[0],
               ((uint8_t *)(file_info + 3))[1],
               ((uint8_t *)(file_info + 3))[2],
               ((uint8_t *)(file_info + 3))[3],
               file_info[3],
               file_info[3],
               ((uint8_t *)(file_info + 4))[0],
               ((uint8_t *)(file_info + 4))[1],
               ((uint8_t *)(file_info + 4))[2],
               ((uint8_t *)(file_info + 4))[3],
               file_info[4],
               file_info[4],
               ((uint8_t *)(file_info + 5))[0],
               ((uint8_t *)(file_info + 5))[1],
               ((uint8_t *)(file_info + 5))[2],
               ((uint8_t *)(file_info + 5))[3],
               file_info[5],
               file_info[5],
               file_info[6] ? "Yes" : "No");
    }

    //clear
    fclose(npk);

    return 0;
}
