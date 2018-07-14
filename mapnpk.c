#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "args.h"

int main(int argc, char **argv)
{
    //args
    argsdata args;
    args_init(&args);
    args_parse(argc, argv, &args);

    //打开npk
    FILE *npk = fopen(args.in_filename, "rb");
    if (npk == NULL)
    {
        fprintf(stderr, "E: npk file open failed\n");
        exit(1);
    }

    //打开输出
    FILE *out = NULL;
    if (args.out_filename)
        out = fopen(args.out_filename, "w");
    else
        out = stdout;

    //头
    if (args.format == MARKDOWN)
        fprintf(out, "# mapnpk!\n");

    //读取文件大小
    fseek(npk, 0L, SEEK_END);
    uint32_t npk_size = ftell(npk);
    if (args.format == MARKDOWN)
        fprintf(out, "\tFile size:\t%d Byte\n", npk_size);
    else if (args.format == CSV)
        fprintf(out, "File size:, %d Byte\n", npk_size);

    //读取map偏移量
    fseek(npk, 0x14, SEEK_SET);
    uint32_t map_offset;
    fread(&map_offset, 4, 1, npk);
    if (args.format == MARKDOWN)
    {
        if (args.type == HEX)
            fprintf(out, "\tMap offset:\t0x%08X\n",
                    map_offset);
        else if (args.type == INT)
            fprintf(out, "\tMap offset:\t%d\n",
                    map_offset);
        else if (args.type == ORIGINAL)
            fprintf(out, "\tMap offset:\t%02X%02X%02X%02X\n",
                    ((uint8_t *)(&map_offset))[0],
                    ((uint8_t *)(&map_offset))[1],
                    ((uint8_t *)(&map_offset))[2],
                    ((uint8_t *)(&map_offset))[3]);
    }
    else if (args.format == CSV)
    {
        if (args.type == HEX)
            fprintf(out, "Map offset:, 0x%08X\n",
                    map_offset);
        else if (args.type == INT)
            fprintf(out, "Map offset:, %d\n",
                    map_offset);
        else if (args.type == ORIGINAL)
            fprintf(out, "Map offset:, %02X%02X%02X%02X\n",
                    ((uint8_t *)(&map_offset))[0],
                    ((uint8_t *)(&map_offset))[1],
                    ((uint8_t *)(&map_offset))[2],
                    ((uint8_t *)(&map_offset))[3]);
    }

    //准备读取
    uint32_t file_info[7];

    if (args.format == MARKDOWN)
        fprintf(out, "| Index | Offset | Size (Byte) | Unzip size (Byte) | Chk | Unzip Chk | Is zip |\n| --- | --- | --- | --- | --- | --- | --- |\n");
    else if (args.format == CSV)
        fprintf(out, "Index, Offset, Size (Byte), Unzip size (Byte), Chk, Unzip Chk, Is zip\n");

    for (int file_offset = map_offset; file_offset < npk_size; file_offset += 7 * 4)
    {
        //map读取文件信息
        fseek(npk, file_offset, SEEK_SET);
        fread(&file_info, 4, 7, npk);

        //控制台输出文件信息
        if (args.format == MARKDOWN)
        {
            if (args.type == HEX)
                fprintf(out, "| 0x%08X | 0x%08X | 0x%08X | 0x%08X | 0x%08X | 0x%08X | %s |\n",
                        file_info[0],
                        file_info[1],
                        file_info[2],
                        file_info[3],
                        file_info[4],
                        file_info[5],
                        file_info[6] ? "Yes" : "No");
            else if (args.type == INT)
                fprintf(out, "| %d | %d | %d | %d | %d | %d | %s |\n",
                        file_info[0],
                        file_info[1],
                        file_info[2],
                        file_info[3],
                        file_info[4],
                        file_info[5],
                        file_info[6] ? "Yes" : "No");
            else if (args.type == ORIGINAL)
                fprintf(out, "| %02X%02X%02X%02X | %02X%02X%02X%02X | %02X%02X%02X%02X | %02X%02X%02X%02X | %02X%02X%02X%02X | %02X%02X%02X%02X | %s |\n",
                        ((uint8_t *)(file_info))[0],
                        ((uint8_t *)(file_info))[1],
                        ((uint8_t *)(file_info))[2],
                        ((uint8_t *)(file_info))[3],
                        ((uint8_t *)(file_info + 1))[0],
                        ((uint8_t *)(file_info + 1))[1],
                        ((uint8_t *)(file_info + 1))[2],
                        ((uint8_t *)(file_info + 1))[3],
                        ((uint8_t *)(file_info + 2))[0],
                        ((uint8_t *)(file_info + 2))[1],
                        ((uint8_t *)(file_info + 2))[2],
                        ((uint8_t *)(file_info + 2))[3],
                        ((uint8_t *)(file_info + 3))[0],
                        ((uint8_t *)(file_info + 3))[1],
                        ((uint8_t *)(file_info + 3))[2],
                        ((uint8_t *)(file_info + 3))[3],
                        ((uint8_t *)(file_info + 4))[0],
                        ((uint8_t *)(file_info + 4))[1],
                        ((uint8_t *)(file_info + 4))[2],
                        ((uint8_t *)(file_info + 4))[3],
                        ((uint8_t *)(file_info + 5))[0],
                        ((uint8_t *)(file_info + 5))[1],
                        ((uint8_t *)(file_info + 5))[2],
                        ((uint8_t *)(file_info + 5))[3],
                        file_info[6] ? "Yes" : "No");
        }
        else if (args.format == CSV)
        {
            if (args.type == HEX)
                fprintf(out, "0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, %s\n",
                        file_info[0],
                        file_info[1],
                        file_info[2],
                        file_info[3],
                        file_info[4],
                        file_info[5],
                        file_info[6] ? "Yes" : "No");
            else if (args.type == INT)
                fprintf(out, "%d, %d, %d, %d, %d, %d, %s\n",
                        file_info[0],
                        file_info[1],
                        file_info[2],
                        file_info[3],
                        file_info[4],
                        file_info[5],
                        file_info[6] ? "Yes" : "No");
            else if (args.type == ORIGINAL)
                fprintf(out, "%02X%02X%02X%02X, %02X%02X%02X%02X, %02X%02X%02X%02X, %02X%02X%02X%02X, %02X%02X%02X%02X, %02X%02X%02X%02X, %s\n",
                        ((uint8_t *)(file_info))[0],
                        ((uint8_t *)(file_info))[1],
                        ((uint8_t *)(file_info))[2],
                        ((uint8_t *)(file_info))[3],
                        ((uint8_t *)(file_info + 1))[0],
                        ((uint8_t *)(file_info + 1))[1],
                        ((uint8_t *)(file_info + 1))[2],
                        ((uint8_t *)(file_info + 1))[3],
                        ((uint8_t *)(file_info + 2))[0],
                        ((uint8_t *)(file_info + 2))[1],
                        ((uint8_t *)(file_info + 2))[2],
                        ((uint8_t *)(file_info + 2))[3],
                        ((uint8_t *)(file_info + 3))[0],
                        ((uint8_t *)(file_info + 3))[1],
                        ((uint8_t *)(file_info + 3))[2],
                        ((uint8_t *)(file_info + 3))[3],
                        ((uint8_t *)(file_info + 4))[0],
                        ((uint8_t *)(file_info + 4))[1],
                        ((uint8_t *)(file_info + 4))[2],
                        ((uint8_t *)(file_info + 4))[3],
                        ((uint8_t *)(file_info + 5))[0],
                        ((uint8_t *)(file_info + 5))[1],
                        ((uint8_t *)(file_info + 5))[2],
                        ((uint8_t *)(file_info + 5))[3],
                        file_info[6] ? "Yes" : "No");
        }
    }

    //clear
    fclose(npk);

    return 0;
}
