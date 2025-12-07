// print_route.c: 打印所有校车的所有站点
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"

void print_alias(void)
{
    int row = 0;
    char** all = read_alias(&row);

    // 输出路线
    printf("西南大学校车站点别名及周边建筑一览（更新至2025-11-25）\n");
    printf("当前校内共设置了 %d 个站点。\n", row);
    for (int i = 0; i < row; i++)
    {
        printf("%s\n", all[i]);
    }

    free(all);
    putchar('\n');

    system("pause");
    system("cls");
}