// search_input.c: 查找乘坐路径（包括换乘方案）
/*功能实现单：
1.同线路起点终点打印
2.票价显示
3.换乘方案（不过于复杂）
4.偏好显示
5.途经点换乘方案
6.模糊查找
7.别名及周边建筑地标查找*/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"

void search_route(void)
{
    system("cls");
    const size_t BUF = 22;          // 每个输入缓冲区大小，足够容纳多字节中文
    const int MAX_POINTS = 5;       // 途经点最大数量
    char* origin = malloc(BUF);     // 存储用户输入的起点站
    char* dest = malloc(BUF);       // 存储用户输入的目的地
    int judge_quit = 1;             // 用于判断是否退出循环的参数
    int path_point_amount = 0;      // 记录途径点数量的变量
    char** path_point_stop = NULL;  // 存储用户输入的途径点（因为不知道用户键入途经点的数量所以声明空指针）

    // 读取起点
    while ((getchar()) != '\n'); // 读走空字符
    do
    {
        while (1)
        {
            printf("请输入您所在的位置（如站名、附近建筑和地标，不超过10个字）：");
            if (fgets(origin, (int)BUF, stdin) == NULL)
            {
                fprintf(stderr, "出错啦，请重试。\n");
                // 清除可能残留的输入
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                continue;
            }
            // 去掉末尾换行
            size_t ln = strlen(origin);
            if (ln > 0 && origin[ln - 1] == '\n')
                origin[ln - 1] = '\0';

            if (origin[0] == '\0')
            {
                printf("输入不能为空，请重新输入。\n");
                continue;
            }
            break;
        }

        // 读取目的地
        while (1)
        {
            printf("请输入您的目的地（如站名、附近建筑和地标，不超过10个字）：");
            if (fgets(dest, (int)BUF, stdin) == NULL)
            {
                fprintf(stderr, "出错啦，请重试。\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                continue;
            }
            size_t ln = strlen(dest);
            if (ln > 0 && dest[ln - 1] == '\n')
                dest[ln - 1] = '\0';

            if (dest[0] == '\0')
            {
                printf("输入不能为空，请重新输入。\n");
                continue;
            }
            break;
        }

        // 读取途经点数量
        while (1)
        {
            printf("请输入途经地个数（阿拉伯数字，最高支持加入%d个途径点，0 表示无）：", MAX_POINTS);
            if (scanf("%d", &path_point_amount) != 1)
            {
                printf("输入有误，请重新输入！\n");
                // 清除错误的输入
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            // 清除输入缓冲区中的剩余字符（包括换行符）
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            break;
        }

        // 如果需要途经点，分配数组并读取每一项
        if (path_point_amount > 0)
        {
            path_point_stop = malloc((size_t)path_point_amount * sizeof(char*)); // 分配存储途径点的内存
            if (path_point_stop == NULL)
            {
                fprintf(stderr, "出错啦（内存分配失败）！\n");
                free(origin);
                free(dest);
                return;
            }
            for (int i = 0; i < path_point_amount; i++)
            {
                path_point_stop[i] = malloc(BUF);
                if (path_point_stop[i] == NULL)
                {
                    fprintf(stderr, "出错啦（内存分配失败）！\n");
                    for (int j = 0; j < i; j++)
                        free(path_point_stop[j]);
                    free(path_point_stop);
                    free(origin);
                    free(dest);
                    return;
                }

                while (1)
                {
                    printf("请输入第 %d 个途径点（如站名、附近建筑和地标，不超过10个字）：", i + 1);
                    if (fgets(path_point_stop[i], (int)BUF, stdin) == NULL)
                    {
                        fprintf(stderr, "读取失败，请重试。\n");
                        clearerr(stdin);
                        continue;
                    }

                    size_t ln = strlen(path_point_stop[i]);
                    if (ln > 0 && path_point_stop[i][ln - 1] == '\n')
                        path_point_stop[i][ln - 1] = '\0';

                    // 检查空输入
                    if (path_point_stop[i][0] == '\0')
                    {
                        printf("输入不能为空，请重新输入。\n");
                        continue;
                    }
                    break;
                }
            }
        }

        /*遍历线路配置文件，依次查找用户输入的所有站点是否在线路上；
        如果有，不改变值；
        如果没有，便遍历别名配置文件，并将可能的站点名打印（模糊查找在此处实现），由用户选择，并将最终结果赋给对应要修改的内存；
        若还是没有找到，提示用户重新输入*/

        // 站点验证和模糊查找
        int check_origin, check_dest;
        check_origin = check_station(origin);
        check_dest = check_station(dest);

        // 验证起点和目的地
        if (!check_origin)
        {
            check_origin = fuzzy_search(origin); //调用模糊搜索
            if (!check_origin)
            {
                judge_quit = 0;
            }
        }
        if (!check_dest)
        {
            check_dest = fuzzy_search(dest);
            if (!check_dest)
            {
                judge_quit = 0;
            }
        }

        // 验证途经点
        if (path_point_amount > 0)
        {
            int* path_not_found = malloc(path_point_amount * sizeof(int));
            check_path(path_point_stop, path_point_amount, path_not_found);

            for (int i = 0; i < path_point_amount; i++)
            {
                if (path_not_found[i])
                {
                    printf("未找到途经点\"%s\"，考虑到您可能输入的是一个附近的建筑或地标，或者某个站点的别名。正在通过别名查找...\n", path_point_stop[i]);

                    char** matches = NULL;
                    int match_count = 0;
                    show_alias_options(path_point_stop[i], &matches, &match_count);

                    if (match_count == 1)
                    {
                        printf("找到匹配站点: %s\n", matches[0]);
                        free(path_point_stop[i]);
                        path_point_stop[i] = strdup(matches[0]);
                    }
                    else if (match_count > 1)
                    {
                        printf("找到多个匹配站点，请选择：\n");
                        for (int j = 0; j < match_count; j++)
                        {
                            printf("%d. %s\n", j + 1, matches[j]);
                        }
                        printf("请选择(1-%d): ", match_count);

                        int choice;
                        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= match_count)
                        {
                            free(path_point_stop[i]);
                            path_point_stop[i] = strdup(matches[choice - 1]);
                            printf("已选择: %s\n", path_point_stop[i]);
                        }
                        else
                        {
                            printf("选择无效，使用第一个匹配。\n");
                            free(path_point_stop[i]);
                            path_point_stop[i] = strdup(matches[0]);
                        }
                        while ((getchar()) != '\n');
                    }
                    else
                    {
                        char* found = find_station_by_alias(path_point_stop[i]);
                        if (found)
                        {
                            printf("通过别名找到站点: %s\n", found);
                            free(path_point_stop[i]);
                            path_point_stop[i] = found;
                        }
                        else
                        {
                            printf("未找到匹配站点。\n");
                            judge_quit = 0;
                            system("pause");
                            system("cls");
                        }
                    }

                    if (matches)
                    {
                        for (int j = 0; j < match_count; j++) free(matches[j]);
                        free(matches);
                    }
                }
            }
            free(path_not_found);
        }
    }while (!judge_quit);

    // 打印读取到的内容，方便用户查看
    printf("\n--- 输入摘要 ---\n");
    printf("起点：%s\n", origin);
    printf("目的地：%s\n", dest);
    if (path_point_amount > 0)
    {
        printf("途经点（共 %d 个）：\n", path_point_amount);
        for (int i = 0; i < path_point_amount; i++)
            printf("  %d. %s\n", i + 1, path_point_stop[i]);
    }
    else
    {
        printf("无途经点。\n");
    }
    printf("----------------\n");
    // 查找线路函数
    int judge = 0;
    judge = search_in_same_route(origin, dest, &path_point_amount, path_point_stop);
    if (judge == 0)
    {
        judge = search_in_different_route(origin, dest);
        if (judge == 0)
        {
            printf("没有找到可以到达的线路或换乘方案。请重试！\n");
        }
    }
    printf("---没有更多了---\n");

    // 释放资源
    free(origin);
    free(dest);
    if (path_point_stop != NULL)
    {
        for (int i = 0; i < path_point_amount; i++)
            free(path_point_stop[i]);
        free(path_point_stop);
    }

    system("pause");
    system("cls");
}