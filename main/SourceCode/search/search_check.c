// search_check.c: 查找用户输入的站点并替换为标准站点名
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"
#include "search.h"

// 检查站点是否在路径中
int check_station_index(char** stops, int stops_count, const char* station)
{
    for (int i = 0; i < stops_count; i++)
    {
        if (strcmp(stops[i], station) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// 检查单个站点是否存在
int check_station(char* stop)
{
    int row_in_route = 0;
    char** routes = read_route(&row_in_route);
    if (!routes || row_in_route <= 0)
    {
        if (routes)
        {
            for (int i = 0; i < row_in_route; i++) free(routes[i]);
            free(routes);
        }
        return 0;
    }

    int found = 0;

    for (int i = 0; i < row_in_route; i++)
    {
        if (!routes[i]) continue;

        char* route_copy = strdup(routes[i]);
        if (!route_copy) continue;

        char** stops = NULL;
        int stops_count = 0;
        char* token = strtok(route_copy, "|");

        while (token)
        {
            while (*token == ' ' || *token == '\t') token++;
            char* end = token + strlen(token) - 1;
            while (end > token && (*end == ' ' || *end == '\t')) *end-- = '\0';

            stops = realloc(stops, (stops_count + 1) * sizeof(char*));
            stops[stops_count++] = token;
            token = strtok(NULL, "|");
        }

        found = check_station_index(stops, stops_count, stop);
        free(stops);
        free(route_copy);

        if (found) break;
    }

    for (int i = 0; i < row_in_route; i++) free(routes[i]);
    free(routes);

    return found;
}

// 通过别名查找站点
char* find_station_by_alias(char* input)
{
    int row_alias = 0;
    char** aliases = read_alias(&row_alias);
    if (!aliases || row_alias <= 0)
    {
        return NULL;
    }

    // 遍历别名配置
    for (int i = 0; i < row_alias; i++)
    {
        if (!aliases[i]) continue;

        char* alias_copy = strdup(aliases[i]);
        char* formal_name = strtok(alias_copy, "|");

        if (!formal_name)
        {
            free(alias_copy);
            continue;
        }

        // 检查别名
        char* alias = strtok(NULL, "|");
        while (alias)
        {
            // 去除空格
            while (*alias == ' ' || *alias == '\t') alias++;
            char* end = alias + strlen(alias) - 1;
            while (end > alias && (*end == ' ' || *end == '\t')) *end-- = '\0';

            if (strstr(alias, input) != NULL)
            {
                char* result = strdup(formal_name);
                free(alias_copy);

                // 释放资源
                for (int j = 0; j < row_alias; j++) free(aliases[j]);
                free(aliases);

                return result;
            }
            alias = strtok(NULL, "|");
        }
        free(alias_copy);
    }

    for (int i = 0; i < row_alias; i++) free(aliases[i]);
    free(aliases);

    return NULL;
}

// 显示所有匹配的别名选项
void show_alias_options(char* input, char*** matches, int* match_count)
{
    int row_alias = 0;
    char** aliases = read_alias(&row_alias);
    if (!aliases || row_alias <= 0) return;

    *matches = NULL;
    *match_count = 0;

    for (int i = 0; i < row_alias; i++)
    {
        if (!aliases[i]) continue;

        char* alias_copy = strdup(aliases[i]);
        char* formal_name = strtok(alias_copy, "|");

        if (!formal_name)
        {
            free(alias_copy);
            continue;
        }

        char* alias = strtok(NULL, "|");
        while (alias)
        {
            while (*alias == ' ' || *alias == '\t') alias++;
            char* end = alias + strlen(alias) - 1;
            while (end > alias && (*end == ' ' || *end == '\t')) *end-- = '\0';

            if (strstr(alias, input) != NULL)
            {
                int exists = 0;
                for (int j = 0; j < *match_count; j++)
                {
                    if (strcmp((*matches)[j], formal_name) == 0)
                    {
                        exists = 1;
                        break;
                    }
                }

                if (!exists)
                {
                    *matches = realloc(*matches, (*match_count + 1) * sizeof(char*));
                    (*matches)[*match_count] = strdup(formal_name);
                    (*match_count)++;
                }
                break;
            }
            alias = strtok(NULL, "|");
        }
        free(alias_copy);
    }

    for (int i = 0; i < row_alias; i++) free(aliases[i]);
    free(aliases);
}

// 检查路径中的站点
void check_path(char** path_stops, int path_amount, int* path_not_found)
{
    for (int i = 0; i < path_amount; i++)
    {
        path_not_found[i] = !check_station(path_stops[i]);
    }
}