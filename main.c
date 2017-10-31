#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>

int main(int argc, char **argv)
{
    printf("#unnpk!\n");

    if (argc != 3)
    {
        printf("\tHelp: unnpk npk_file unnpk_direcory\n");
        exit(1);
    }

    char *npk_path = argv[1];
    char *out_path = argv[2];

    //打开npk
    FILE *npk = fopen(npk_path, "rb");
    if (npk == NULL)
    {
        printf("\tE: npk file open failed\n");
        exit(1);
    }

    //输出文件夹
    if (out_path[strlen(out_path) - 1] == '/')
        out_path[strlen(out_path) - 1] = 0;
    if (mkdir(out_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
    {
        printf("\tW: mkdir failed\n");
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
    char *file_out_name = 0;
    if (!(file_out_name = malloc(strlen(out_path) + 1 /* / */ + 8 /* 文件名 */ + 5 /* 扩展名 */ + 1 /* 0 */)))
    {
        printf("\tE: No enough memory!\n");
        exit(1);
    }
    uint8_t *file_read_buf = 0;
    uint8_t *file_out_buf = 0;
    uLongf file_destLen = 0;

    printf("| Index\t\t | Offset\t | Size\t\t | Unzip size\t | zip\t |\n| -\t\t | -\t\t | -\t\t | -\t\t | -\t |\n");
    for (int file_offset = map_offset; file_offset < npk_size; file_offset += 7 * 4)
    {
        //map读取文件信息
        fseek(npk, file_offset, SEEK_SET);
        fread(&file_info, 4, 7, npk);

        //控制台输出文件信息
        printf("| %8x\t | 0x%08X\t | %.3g %s\t | %.3g %s\t | %s\t |\n",
               file_info[0],
               file_info[1],
               (file_info[2] > 1000000) ? (float)file_info[2] / 1000000 : (file_info[2] > 1000) ? (float)file_info[2] / 1000 : (float)file_info[2],
               (file_info[2] > 1000000) ? "MB" : (file_info[2] > 1000) ? "KB" : "Byte",
               (file_info[3] > 1000000) ? (float)file_info[3] / 1000000 : (file_info[3] > 1000) ? (float)file_info[3] / 1000 : (float)file_info[3],
               (file_info[3] > 1000000) ? "MB" : (file_info[3] > 1000) ? "KB" : "Byte",
               file_info[6] ? "Yes" : "No");

        //读取数据
        if (!(file_read_buf = malloc(file_info[2])))
        {
            printf("\tE: No enough memory!\n");
            exit(1);
        }
        fseek(npk, file_info[1], SEEK_SET);
        fread(file_read_buf, 1, file_info[2], npk);

        if (file_info[6])
        {
            //解压
            if (!(file_out_buf = malloc(file_info[3])))
            {
                printf("\tE: No enough memory!\n");
                exit(1);
            }
            file_destLen = file_info[3];
            switch (uncompress(file_out_buf, &file_destLen, file_read_buf, file_info[2]))
            {
            case Z_OK:
                //clear
                free(file_read_buf);
                file_read_buf = 0;
                break;
            case Z_MEM_ERROR:
                printf("\tE: Uncompress failed!\n");
                printf("Z_MEM_ERROR: No enough memory\n");
                exit(1);
                break;
            case Z_BUF_ERROR:
                free(file_out_buf);
                file_out_buf = 0;
                file_out_buf = file_read_buf;
                file_read_buf = 0;
                printf("\tW: Uncompress failed!\n");
                printf("Z_BUF_ERROR: Map is not right, The raw data will be output\n");
                printf("| Index\t\t | Offset\t | Size\t\t | Unzip size\t | zip\t |\n| -\t\t | -\t\t | -\t\t | -\t\t | -\t |\n");
                break;
            case Z_DATA_ERROR:
                free(file_out_buf);
                file_out_buf = 0;
                file_out_buf = file_read_buf;
                file_read_buf = 0;
                printf("\tW: Uncompress failed!\n");
                printf("Z_DATA_ERROR: Data is not zlib, The raw data will be output\n");
                printf("| Index\t\t | Offset\t | Size\t\t | Unzip size\t | zip\t |\n| -\t\t | -\t\t | -\t\t | -\t\t | -\t |\n");
                break;
            }
        }
        else
        {
            if (file_info[2] != file_info[3])
            {
                printf("W: size error!\n");
            }
            file_out_buf = file_read_buf;
            file_read_buf = 0;
        }

        //输出文件名
        sprintf(file_out_name, "%s/%08X", out_path, file_info[0]);

        //文件头判断类型
        if (memcmp(file_out_buf, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8) == 0)
            strcat(file_out_name, ".png");

        //打开并写入数据
        file_out = fopen(file_out_name, "w+");
        fwrite(file_out_buf, 1, file_info[3], file_out);
        fclose(file_out);

        //clear
        free(file_out_buf);
        file_out_buf = 0;
    }

    //clear
    fclose(npk);
    free(file_out_name);
    file_out_name = 0;

    return 0;
}
