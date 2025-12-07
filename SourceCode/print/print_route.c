// print_route.c: 打印所有校车的所有站点
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"

void print_route(void)
{
    int row = 0;
    char** all = read_route(&row);

    // 输出路线
    printf("西南大学校车路线表（更新至2025-11-25）\n");
    printf("当前校内共开通了 %d 条路线。\n", row);
    for (int i = 0; i < row; i++)
    {
        if (all[i] == NULL) continue;

        // 复制字符串
        char* route_copy = strdup(all[i]);
        if (route_copy == NULL) continue;

        // 分割站点
        char* token = strtok(route_copy, "|");
        while (token != NULL)
        {
            // 去除首尾空格
            char* start = token;
            while (*start == ' ' || *start == '\t') start++;

            char* end = start + strlen(start) - 1;
            while (end > start && (*end == ' ' || *end == '\t')) end--;
            *(end + 1) = '\0';

            printf("%s", start);
            token = strtok(NULL, "|");
            if (token != NULL) {
                printf(" → ");
            }
        }
        printf("\n");

        free(route_copy);
    }

    // 释放内存
    for (int i = 0; i < row; i++) {
        free(all[i]);
    }
    free(all);

    putchar('\n');

    system("pause");
    system("cls");
}