#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include <magic.h>

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
        fprintf(stderr, "E: npk file open failed\n");
        exit(1);
    }

    //输出文件夹
    if (out_path[strlen(out_path) - 1] == '/')
        out_path[strlen(out_path) - 1] = 0;
    if (mkdir(out_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
    {
        fprintf(stderr, "W: mkdir failed\n");
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
    if (!(file_out_name = malloc(strlen(out_path) + 1 /* / */ + 8 /* 文件名 */ + 20 /* 扩展名 */ + 1 /* 0 */)))
    {
        fprintf(stderr, "E: No enough memory!\n");
        exit(1);
    }
    uint8_t *file_read_buf = 0;
    uint8_t *file_out_buf = 0;
    uLongf file_destLen = 0;
    char *file_out_type = 0;

    printf("| Index\t\t | Offset\t | Size\t\t | Unzip size\t | zip\t | Type\t\t | MIME Type\t |\n| -\t\t | -\t\t | -\t\t | -\t\t | -\t | -\t\t | -\t\t |\n");
    for (int file_offset = map_offset; file_offset < npk_size; file_offset += 7 * 4)
    {
        //map读取文件信息
        fseek(npk, file_offset, SEEK_SET);
        fread(&file_info, 4, 7, npk);

        //控制台输出文件信息
        printf("| %08X\t | 0x%08X\t | %.3g %s\t | %.3g %s\t | %s\t ",
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
            fprintf(stderr, "E: No enough memory!\n");
            exit(1);
        }
        fseek(npk, file_info[1], SEEK_SET);
        fread(file_read_buf, 1, file_info[2], npk);

        if (file_info[6])
        {
            //解压
            if (!(file_out_buf = malloc(file_info[3])))
            {
                fprintf(stderr, "E: No enough memory!\n");
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
                fprintf(stderr, "E: Uncompress failed!\n");
                fprintf(stderr, "Z_MEM_ERROR: No enough memory\n");
                exit(1);
                break;
            case Z_BUF_ERROR:
                free(file_out_buf);
                file_out_buf = 0;
                file_out_buf = file_read_buf;
                file_read_buf = 0;
                fprintf(stderr, "W: Uncompress failed!\n");
                fprintf(stderr, "Z_BUF_ERROR: Map is not right, The raw data will be output\n");
                break;
            case Z_DATA_ERROR:
                free(file_out_buf);
                file_out_buf = 0;
                file_out_buf = file_read_buf;
                file_read_buf = 0;
                fprintf(stderr, "W: Uncompress failed!\n");
                fprintf(stderr, "Z_DATA_ERROR: Data is not zlib, The raw data will be output\n");
                break;
            }
        }
        else
        {
            if (file_info[2] != file_info[3])
            {
                fprintf(stderr, "W: Map size column error!\n");
            }
            file_out_buf = file_read_buf;
            file_read_buf = 0;
        }

        //输出文件名
        sprintf(file_out_name, "%s/%08X", out_path, file_info[0]);

        //获取MIME类型辅助判断
        magic_t cookie;
        cookie = magic_open(MAGIC_MIME_TYPE);
        magic_load(cookie, NULL);
        file_out_type = (char *)magic_buffer(cookie, file_out_buf, file_info[3]);

        //判断文件类型
        if (strstr(file_out_type, "image/png"))
        {
            strcat(file_out_name, ".png");
            printf("| PNG\t\t ");
        }
        else if (strstr(file_out_type, "image/jpeg"))
        {
            strcat(file_out_name, ".jpg");
            printf("| JPG\t\t ");
        }
        else if (strstr(file_out_type, "xml"))
        {
            strcat(file_out_name, ".xml");
            printf("| XML\t\t ");
        }
        else if (memcmp(file_out_buf + 1, "KTX", 3) == 0)
        {
            strcat(file_out_name, ".ktx");
            printf("| KTX\t\t ");
        }
        else if (memcmp(file_out_buf, "RGIS", 3) == 0)
        {
            strcat(file_out_name, ".RGIS");
            printf("| RGIS\t\t ");
        }
        else if (memcmp(file_out_buf, "PKM", 3) == 0)
        {
            strcat(file_out_name, ".PKM");
            printf("| PKM\t\t ");
        }
        else if (strstr(file_out_type, "text"))
        {
            if (memcmp(file_out_buf, "<NeoX", 5) == 0 || memcmp(file_out_buf, "<Neox", 5) == 0)
            {
                strcat(file_out_name, ".NeoX.xml");
                printf("| NeoX XML\t\t ");
            }
            else if (memcmp(file_out_buf, "<FxGroup", 8) == 0)
            {
                strcat(file_out_name, ".FxGroup.xml");
                printf("| FxGroup XML\t\t ");
            }
            else if (memcmp(file_out_buf, "<SceneMusic", 11) == 0)
            {
                strcat(file_out_name, ".SceneMusic.xml");
                printf("| SceneMusic XML\t\t ");
            }
            else if (memcmp(file_out_buf, "<MusicTriggers", 14) == 0)
            {
                strcat(file_out_name, ".MusicTriggers.xml");
                printf("| MusicTriggers XML\t\t ");
            }
            else if (memcmp(file_out_buf, "<cinematic", 10) == 0)
            {
                strcat(file_out_name, ".cinematic.xml");
                printf("| cinematic XML\t\t ");
            }
            else if (memcmp(file_out_buf, "<EquipList", 10) == 0)
            {
                strcat(file_out_name, ".EquipList.xml");
                printf("| EquipList XML\t\t ");
            }
            else if (memcmp(file_out_buf, "<SceneConfig", 12) == 0)
            {
                strcat(file_out_name, ".SceneConfig.xml");
                printf("| SceneConfig XML\t\t ");
            }
            else if (file_out_buf[0] == '{' && file_out_buf[file_info[3] - 1] == '}')
            {
                strcat(file_out_name, ".json");
                printf("| JSON\t\t ");
            }
            else if (memmem(file_out_buf, file_info[3], "vec4", 4) || memmem(file_out_buf, file_info[3], "vec2", 4) || memmem(file_out_buf, file_info[3], "tex2D", 5) || memmem(file_out_buf, file_info[3], "tex3D", 5) || memmem(file_out_buf, file_info[3], "float", 5))
            {
                strcat(file_out_name, ".glsl");
                printf("| GLSL\t\t ");
            }
            else if (memmem(file_out_buf, file_info[3], "v ", 2) && memmem(file_out_buf, file_info[3], "vt ", 3) && memmem(file_out_buf, file_info[3], "f ", 2))
            {
                strcat(file_out_name, ".obj");
                printf("| OBJ\t\t ");
            }
            else
            {
                strcat(file_out_name, ".txt");
                printf("| TXT\t\t ");
            }
        }
        else
        {
            printf("| Unknow\t ");
        }
        printf("| %s\t ", file_out_type);
        magic_close(cookie);

        //打开并写入数据
        file_out = fopen(file_out_name, "w+");
        fwrite(file_out_buf, 1, file_info[3], file_out);
        fclose(file_out);

        //clear
        free(file_out_buf);
        file_out_buf = 0;

        printf("|\n");
    }

    //clear
    fclose(npk);
    free(file_out_name);
    file_out_name = 0;

    return 0;
}
