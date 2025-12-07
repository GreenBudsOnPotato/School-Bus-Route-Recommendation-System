// search_fuzzy_search.c: 实现单索引的模糊查找
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"

int fuzzy_search(char* stop)
{
    printf("未找到站点\"%s\"。考虑到您可能输入的是一个附近的建筑或地标，或者某个站点的别名。正在通过别名查找...\n", stop);

    char** matches = NULL;
    int match_count = 0,is_found = 0;
    show_alias_options(stop, &matches, &match_count);

    if (match_count == 1)
    {
        printf("找到匹配站点: %s\n", matches[0]);
        strcpy(stop, matches[0]);
        // stop = strdup(matches[0]);
        is_found = 1;
    }
    else if (match_count > 1)
    {
        printf("找到多个匹配站点，请选择：\n");
        for (int i = 0; i < match_count; i++)
        {
            printf("%d. %s\n", i + 1, matches[i]);
        }
        printf("请选择(1-%d): ", match_count);

        int choice;
        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= match_count)
        {
            strcpy(stop, matches[choice - 1]);
            printf("已选择: %s\n", stop);
        }
        else {
            printf("选择无效，使用第一个匹配。\n");
            strcpy(stop, matches[0]);
        }
        while ((getchar()) != '\n'); // 清除输入缓冲区
        is_found = 1;
    }
    else
    {
        char* found = find_station_by_alias(stop);
        if (found)
        {
            printf("通过别名找到站点: %s\n", found);
            strcpy(stop, found);
            is_found = 1;
        }
        else
        {
            printf("未找到匹配站点！\n");
            system("pause");
            system("cls");
            is_found = 0;
        }
    }

    // 释放匹配数组
    if (matches)
    {
        for (int i = 0; i < match_count; i++) free(matches[i]);
        free(matches);
    }
    return is_found;
}