// search_in_same_route.c: 同线路查找，如果在同线路
// 上能够找到起点和目的地，并且途径点在两者之间，返回真；否则返回假，进入异线查找。
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include <errno.h>
#include "read.h"

// 声明解决方案链表

typedef struct {
    int route;
    char* origin;
    char* dest;
}solution;

// 检查站点是否在路径中并返回索引
int find_station_index(char** stops, int stops_count, const char* station)
{
    for (int i = 0; i < stops_count; i++)
    {
        if (strcmp(stops[i], station) == 0)
        {
            return i;
        }
    }
    return -1;
}

// 检查途经点是否在起点和终点之间
int validate_path_points(char** stops, int stops_count, int start_idx, int end_idx, char** path_points, int path_count)
{
    for (int i = 0; i < path_count; i++)
    {
        int point_idx = find_station_index(stops, stops_count, path_points[i]);
        if (point_idx == -1) return 0; // 途经点不在线路上

        // 检查途经点是否在起点和终点之间
        if ((start_idx < end_idx && (point_idx < start_idx || point_idx > end_idx)) ||
            (start_idx > end_idx && (point_idx > start_idx || point_idx < end_idx))) return 0; // 途经点不在起点和终点之间
    }
    return 1;
}

// 打印路线
void print_route_success(char** stops, int stops_count, int start_idx, int end_idx)
{
    int step = (start_idx < end_idx) ? 1 : -1;
    int current = start_idx;

    while (current != end_idx)
    {
        printf("%s", stops[current]);
        current += step;
        if (current != end_idx + step)
        {
            printf(" → ");
        }
    }
    printf("%s\n", stops[end_idx]);
}

// 打印路线到日志
void print_solution_in_same_route_to_file(int count, solution* solve, FILE* fp)
{
    for (int i = 0; i < count; i++)
        fprintf(fp, "%d路：%s → %s\n", solve[i].route, solve[i].origin, solve[i].dest);
}

int search_in_same_route(char* origin, char* dest, int* path_amount, char** path)
{
    if (!origin || !dest || !path_amount) return 0;

    int row_in_route = 0;
    char** routes = read_route(&row_in_route);
    if (!routes || row_in_route <= 0)
    {
        free(routes);
        return 0;
    }

    int found_any = 0, count = 0;
    solution* solve = NULL;

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
                return found_any;
            }
            stops[stops_count++] = token;

            token = strtok(NULL, "|");
        }

        // 查找起点和终点
        int origin_idx = find_station_index(stops, stops_count, origin);
        int dest_idx = find_station_index(stops, stops_count, dest);

        // 检查是否在同一线路且不是同一站点
        if (origin_idx != -1 && dest_idx != -1 && origin_idx != dest_idx)
        {
            // 检查途经点
            if (validate_path_points(stops, stops_count, origin_idx, dest_idx, path, *path_amount))
            {
                printf("%d路：", i + 1);
                print_route_success(stops, stops_count, origin_idx, dest_idx);
                printf("票价：1元（非教职工及学生2元）\n");
                found_any = 1;
                solve = realloc(solve, (count + 1) * sizeof(solution));
                if (solve == NULL)
                {
                    printf("分配内存失败！");
                    free(stops);
                    free(route_copy);
                    free(routes);
                    system("pause");
                    system("cls");
                    return found_any;
                }
                solve[count].route = i + 1;
                // 分配并复制origin和dest字符串
                solve[count].origin = malloc(strlen(origin) + 1);
                solve[count].dest = malloc(strlen(dest) + 1);
                if (solve[count].origin) strcpy(solve[count].origin, origin);
                if (solve[count].dest) strcpy(solve[count].dest, dest);
                count++;
            }
        }
        free(stops);
        free(route_copy);
    }

    // 打印乘车方案到日志
    if (found_any == 1)
    {
        int print_file_judge = 0;
        while (1) {
            printf("您是否需要输出乘车方案到日志中(0-->否，1-->是)：");
            if (scanf("%d", &print_file_judge) != 1 || print_file_judge < 0 || print_file_judge > 1) {
                fprintf(stderr, "输入有误，请重新输入!\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                continue;
            }
            break;
        }
        if (print_file_judge == 1) {
            FILE* fp = NULL;
            const char path[] = "output";
            if (_mkdir(path) != 0) {
                if (errno != EEXIST) {
                    MessageBoxA(NULL, "无法创建存放日志文件的路径，请确认程序运行的目录是否拥有修改权限！", "错误", MB_OK | MB_ICONERROR);
                    goto end; // 跳过输出，直接释放动态内存
                }
            }

            fp = fopen("output/solution.log", "ab");
            if (!fp) {
                MessageBoxA(NULL, "无法创建或读取到日志文件，请确认程序运行的目录是否拥有修改权限！", "错误", MB_OK | MB_ICONERROR);
                goto end;
            }

            fprintf(fp, "********************%s → %s********************\n", origin, dest);
            print_solution_in_same_route_to_file(count, solve, fp);
            fprintf(fp, "************************* (^-^) *************************\n");
            fclose(fp);
            printf("成功将乘车方案输出到日志文件中！\n");
        }
    }

    end:
    for (int i = 0; i < row_in_route; i++) free(routes[i]);
    free(routes);
    free(solve);

    return found_any;
}