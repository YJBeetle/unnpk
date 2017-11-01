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
    if (!(file_out_name = malloc(strlen(out_path) + 1 /* / */ + 30 /* 类型文件夹 */ + 1 /* / */ + 8 /* 文件名 */ + 20 /* .扩展名 */ + 1 /* 0 */)))
    {
        fprintf(stderr, "E: No enough memory!\n");
        exit(1);
    }
    uint8_t *file_read_buf = 0;
    uint8_t *file_out_buf = 0;
    uLongf file_destLen = 0;
    char *file_out_type = 0;      //存储Mime类型
    char *file_out_type_p = 0;    //处理Mime类型文件夹用
    char *file_out_extension = 0; //存储扩展名

    printf("| Index\t\t | Offset\t | Size\t\t | Unzip size\t | zip\t | MIME Type\t | Extension\t |\n| -\t\t | -\t\t | -\t\t | -\t\t | -\t | -\t\t | -\t\t |\n");
    for (int file_offset = map_offset; file_offset < npk_size; file_offset += 7 * 4)
    {
        //map读取文件信息
        fseek(npk, file_offset, SEEK_SET);
        fread(&file_info, 4, 7, npk);

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
                file_info[3] = file_info[2];
                fprintf(stderr, "W: Uncompress failed!\n");
                fprintf(stderr, "Z_BUF_ERROR: Map is not right, The raw data will be output\n");
                break;
            case Z_DATA_ERROR:
                free(file_out_buf);
                file_out_buf = 0;
                file_out_buf = file_read_buf;
                file_read_buf = 0;
                file_info[3] = file_info[2];
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

        //获取MIME类型辅助判断
        magic_t cookie;
        cookie = magic_open(MAGIC_MIME_TYPE);
        magic_load(cookie, NULL);
        file_out_type = (char *)magic_buffer(cookie, file_out_buf, file_info[3]);

        //判断文件扩展名
        if (strstr(file_out_type, "image/png"))
        {
            file_out_extension = ".png";
        }
        else if (strstr(file_out_type, "image/jpeg"))
        {
            file_out_extension = ".jpg";
        }
        else if (strstr(file_out_type, "image/vnd.adobe.photoshop"))
        {
            file_out_extension = ".psd";
        }
        else if (strstr(file_out_type, "video/mp4"))
        {
            file_out_extension = ".mp4";
        }
        else if (strstr(file_out_type, "application/zip"))
        {
            file_out_extension = ".zip";
        }
        else if (strstr(file_out_type, "application/font-sfnt"))
        {
            file_out_extension = ".ttf";
        }
        else if (strstr(file_out_type, "xml"))
        {
            file_out_extension = ".xml";
        }
        else if (memcmp(file_out_buf + 1, "KTX", 3) == 0)
        {
            file_out_extension = ".ktx";
        }
        else if (memcmp(file_out_buf, "RGIS", 4) == 0)
        {
            file_out_extension = ".RGIS";
        }
        else if (memcmp(file_out_buf, "PKM", 3) == 0)
        {
            file_out_extension = ".PKM";
        }
        else if (strstr(file_out_type, "text"))
        {
            if (memcmp(file_out_buf, "<NeoX", 5) == 0 || memcmp(file_out_buf, "<Neox", 5) == 0)
            {
                file_out_extension = ".NeoX.xml";
            }
            else if (memcmp(file_out_buf, "<FxGroup", 8) == 0)
            {
                file_out_extension = ".FxGroup.xml";
            }
            else if (memcmp(file_out_buf, "<SceneMusic", 11) == 0)
            {
                file_out_extension = ".SceneMusic.xml";
            }
            else if (memcmp(file_out_buf, "<MusicTriggers", 14) == 0)
            {
                file_out_extension = ".MusicTriggers.xml";
            }
            else if (memcmp(file_out_buf, "<cinematic", 10) == 0)
            {
                file_out_extension = ".cinematic.xml";
            }
            else if (memcmp(file_out_buf, "<EquipList", 10) == 0)
            {
                file_out_extension = ".EquipList.xml";
            }
            else if (memcmp(file_out_buf, "<SceneConfig", 12) == 0)
            {
                file_out_extension = ".SceneConfig.xml";
            }
            else if (file_out_buf[0] == '{' && file_out_buf[file_info[3] - 1] == '}')
            {
                file_out_extension = ".json";
            }
            else if (memmem(file_out_buf, file_info[3], "vec4", 4) || memmem(file_out_buf, file_info[3], "vec2", 4) || memmem(file_out_buf, file_info[3], "tex2D", 5) || memmem(file_out_buf, file_info[3], "tex3D", 5) || memmem(file_out_buf, file_info[3], "float", 5) || memmem(file_out_buf, file_info[3], "define", 5) || memmem(file_out_buf, file_info[3], "incloud", 5))
            {
                file_out_extension = ".glsl";
            }
            else if (memmem(file_out_buf, file_info[3], "v ", 2) && memmem(file_out_buf, file_info[3], "vt ", 3) && memmem(file_out_buf, file_info[3], "f ", 2))
            {
                file_out_extension = ".obj";
            }
            else
            {
                file_out_extension = ".txt";
            }
        }
        else
        {
            file_out_extension = "";
        }

        //输出文件名
        sprintf(file_out_name, "%s/%s/%08X%s", out_path, file_out_type, file_info[0], file_out_extension);

        //创建mime分类文件夹
        file_out_type_p = file_out_type;
        while ((file_out_type_p = strchr(file_out_type_p, '/') + 1) - 1)
        {
            file_out_name[strlen(out_path) + 1 + (file_out_type_p - 1 - file_out_type)] = 0;
            mkdir(file_out_name, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            file_out_name[strlen(out_path) + 1 + (file_out_type_p - 1 - file_out_type)] = '/';
        }
        file_out_name[strlen(out_path) + 1 + strlen(file_out_type)] = 0;
        mkdir(file_out_name, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        file_out_name[strlen(out_path) + 1 + strlen(file_out_type)] = '/';

        //打开并写入数据
        file_out = fopen(file_out_name, "w+");
        fwrite(file_out_buf, 1, file_info[3], file_out);
        fclose(file_out);

        //控制台输出文件信息
        printf("| %08X\t | 0x%08X\t | %.3g %s\t | %.3g %s\t | %s\t | %s\t | %s\t |\n",
        file_info[0],
        file_info[1],
        (file_info[2] > 1000000) ? (float)file_info[2] / 1000000 : (file_info[2] > 1000) ? (float)file_info[2] / 1000 : (float)file_info[2],
        (file_info[2] > 1000000) ? "MB" : (file_info[2] > 1000) ? "KB" : "Byte",
        (file_info[3] > 1000000) ? (float)file_info[3] / 1000000 : (file_info[3] > 1000) ? (float)file_info[3] / 1000 : (float)file_info[3],
        (file_info[3] > 1000000) ? "MB" : (file_info[3] > 1000) ? "KB" : "Byte",
        file_info[6] ? "Yes" : "No",
        file_out_type,
        strlen(file_out_extension)?file_out_extension:"None");

        //clear
        free(file_out_buf);
        file_out_buf = 0;
        magic_close(cookie);
    }

    //clear
    fclose(npk);
    free(file_out_name);
    file_out_name = 0;

    return 0;
}
