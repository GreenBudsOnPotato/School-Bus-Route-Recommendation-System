#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

// 检查站点是否在路径中
int check_station_index(char** stops, int stops_count, const char* station)
{
    for (int i = 0; i < stops_count; i++) {
        if (strcmp(stops[i], station) == 0) {
            return 1;
        }
    }
    return 0;
}

int check_station(char* stop)
{

    int row_in_route = 0, row_in_alias = 0;
    char** routes = read_route(&row_in_route);
    char** alias = read_route(&row_in_alias);
    if (!routes || row_in_route <= 0) {
        free(routes);
        return 0;
    }

    int found_any = 0;

    for (int i = 0; i < row_in_route; i++)
    {
        if (!routes[i]) continue;

        // 复制并分割线路
        char* route_copy = strdup(routes[i]);
        if (!route_copy) continue;

        // 分割站点
        char** stops = NULL;
        int stops_count = 0;
        char* token = strtok(route_copy, "|");

        while (token) {
            // 去除首尾空格
            while (*token == ' ' || *token == '\t') token++;
            char* end = token + strlen(token) - 1;
            while (end > token && (*end == ' ' || *end == '\t')) *end-- = '\0';

            stops = realloc(stops, (stops_count + 1) * sizeof(char*));
            if (stops == NULL)
            {
                printf("分配内存失败！");
                free(route_copy);
                free(stops);
                free(routes);
                system("pause");
                system("cls");
                return NULL;
            }
            stops[stops_count++] = token;

            token = strtok(NULL, "|");
        }

        // 查找站点
        return find_station_index(stops, stops_count, stop);
    }
}

void check_path(char* stop, int path_amount, int *path_not_found)
{
    int row_in_route = 0, row_in_alias = 0;
    char** routes = read_route(&row_in_route);
    char** alias = read_route(&row_in_alias);
    if (!routes || row_in_route <= 0) {
        free(routes);
        return 0;
    }

    int found_any = 0;

    for (int i = 0; i < row_in_route; i++)
    {
        if (!routes[i]) continue;

        // 复制并分割线路
        char* route_copy = strdup(routes[i]);
        if (!route_copy) continue;

        // 分割站点
        char** stops = NULL;
        int stops_count = 0;
        char* token = strtok(route_copy, "|");

        while (token) {
            // 去除首尾空格
            while (*token == ' ' || *token == '\t') token++;
            char* end = token + strlen(token) - 1;
            while (end > token && (*end == ' ' || *end == '\t')) *end-- = '\0';

            stops = realloc(stops, (stops_count + 1) * sizeof(char*));
            if (stops == NULL)
            {
                printf("分配内存失败！");
                free(route_copy);
                free(stops);
                free(routes);
                system("pause");
                system("cls");
                return NULL;
            }
            stops[stops_count++] = token;

            token = strtok(NULL, "|");
        }

        // 查找站点
        int check,count = 0;
        for (int i = 0; i < path_amount; i++)
        {
            check = find_station_index(stops, stops_count, stop[i]);
            if (!check)
            {
                path_not_found[count++] = i;
            }
        }
    }
}