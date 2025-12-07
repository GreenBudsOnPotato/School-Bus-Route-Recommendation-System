// read.c: 读取两个配置文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 利用“\n”读取线路数
static char** split_lines(char* all, size_t read, int* out_row)
{
    if (out_row == NULL) return NULL;
    *out_row = 0;

    if (all == NULL || read == 0) return NULL;

    // 先统计行数
    size_t lines_count = 0;
    for (size_t i = 0; i < read; i++) {
        if (all[i] == '\n') lines_count++;
    }
    // 如果最后一行没有以 '\n' 结尾，但有内容，则也算一行
    if (read > 0 && all[read - 1] != '\n') lines_count++;

    if (lines_count == 0) return NULL;

    char** lines = (char**)malloc(lines_count * sizeof(char*));
    if (lines == NULL) return NULL;

    char* line_start = all;
    size_t idx = 0;
    for (size_t i = 0; i < read && idx < lines_count; i++)
    {
        if (all[i] == '\r') {
            all[i] = '\0';
            continue;
        }
        if (all[i] == '\n')
        {
            size_t len = (size_t)(&all[i] - line_start);
            // 剔除结尾可能的 '\r'
            if (len > 0 && line_start[len - 1] == '\r') len--;
            lines[idx] = (char*)malloc(len + 1);
            if (lines[idx] == NULL)
            {
                for (size_t j = 0; j < idx; j++) free(lines[j]);
                free(lines);
                return NULL;
            }
            if (len > 0) memcpy(lines[idx], line_start, len);
            lines[idx][len] = '\0';
            idx++;
            line_start = &all[i + 1];
        }
    }

    // 处理最后一行（如果最后没有 '\n'）
    if (idx < lines_count && line_start <= all + read - 1)
    {
        size_t len = (size_t)((all + read) - line_start);
        if (len > 0 && line_start[len - 1] == '\r') len--;
        lines[idx] = (char*)malloc(len + 1);
        if (lines[idx] == NULL)
        {
            for (size_t j = 0; j < idx; j++) free(lines[j]);
            free(lines);
            return NULL;
        }
        if (len > 0) memcpy(lines[idx], line_start, len);
        lines[idx][len] = '\0';
        idx++;
    }

    *out_row = (int)idx;
    return lines;
}

char** read_route(int* row) // 读取线路函数
{
    if (row == NULL) return NULL;
    *row = 0;

    FILE* route = fopen("config\\route.cfg", "rb");
    if (route == NULL)
    {
        fprintf(stderr, "无法打开文件: config\\route.cfg\n");
        system("pause");
        system("cls");
        return NULL;
    }

    if (fseek(route, 0, SEEK_END) != 0)
    {
        fclose(route);
        fprintf(stderr, "读取文件长度失败\n");
        system("pause");
        system("cls");
        return NULL;
    }

    long int filelen = ftell(route);
    if (filelen < 0)
    {
        fclose(route);
        fprintf(stderr, "获取文件长度失败\n");
        system("pause");
        system("cls");
        return NULL;
    }

    rewind(route);

    char* all = (char*)malloc((size_t)filelen + 1);
    if (all == NULL)
    {
        fclose(route);
        fprintf(stderr, "内存分配失败\n");
        system("pause");
        system("cls");
        return NULL;
    }

    size_t read = fread(all, 1, (size_t)filelen, route);
    all[read] = '\0';

    fclose(route);

    char** lines = split_lines(all, read, row);

    free(all);
    return lines;
}

char** read_alias(int* row) // 读取别名函数
{
    if (row == NULL) return NULL;
    *row = 0;

    FILE* alias_beside = fopen("config\\alias_beside.cfg", "rb");
    if (alias_beside == NULL)
    {
        fprintf(stderr, "无法打开文件: config\\alias_beside.cfg\n");
        system("pause");
        system("cls");
        return NULL;
    }

    if (fseek(alias_beside, 0, SEEK_END) != 0)
    {
        fclose(alias_beside);
        fprintf(stderr, "读取文件长度失败\n");
        system("pause");
        system("cls");
        return NULL;
    }

    long int filelen = ftell(alias_beside);
    if (filelen < 0)
    {
        fclose(alias_beside);
        fprintf(stderr, "获取文件长度失败\n");
        system("pause");
        system("cls");
        return NULL;
    }

    rewind(alias_beside);

    char* all = (char*)malloc((size_t)filelen + 1);
    if (all == NULL)
    {
        fclose(alias_beside);
        fprintf(stderr, "内存分配失败\n");
        system("pause");
        system("cls");
        return NULL;
    }

    size_t read = fread(all, 1, (size_t)filelen, alias_beside);
    all[read] = '\0';

    fclose(alias_beside);

    char** lines = split_lines(all, read, row);

    free(all);
    return lines;
}