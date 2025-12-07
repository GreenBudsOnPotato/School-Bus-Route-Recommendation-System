// query.c: 查询站点、别名、周边建筑及地标。功能：1.输入站点查询别名及该站点可乘的车；2.输入别名（调用模糊搜索）查找站点，实现功能1
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"
#include "search.h"

void query()
{
	// 读取文件
	char** route = NULL;
	char** alias = NULL;
	int row_in_route = 0, row_in_alias = 0;
	route = read_route(&row_in_route);
	alias = read_alias(&row_in_alias);

	// 指定用户输入（清理可能残留的换行）
	while ((getchar()) != '\n');
	const size_t BUF = 22;
	char* input = malloc(BUF);
	if (!input)
	{
		printf("分配内存失败！\n");
		system("pause");
		system("cls");
		return;
	}
	while (1)
	{
		printf("请输入您想查询的站点（可以输入其别名，或者在其周边的建筑与地标，不超过10个字）：");
		if (fgets(input, (int)BUF, stdin) == NULL)
		{
			fprintf(stderr, "出错啦，请重试。\n");
			// 清除可能残留的输入
			int c;
			while ((c = getchar()) != '\n' && c != EOF) {}
			continue;
		}
		// 去掉末尾换行
		size_t ln = strlen(input);
		if (ln > 0 && input[ln - 1] == '\n')
			input[ln - 1] = '\0';

		if (input[0] == '\0')
		{
			printf("输入不能为空，请重新输入。\n");
			continue;
		}
		break;
	}

	// 查找站点
	int is_exist = 0;
	is_exist = check_station(input);
	if (!is_exist)
	{
		is_exist = fuzzy_search(input);
		if (!is_exist)
		{
			free(input);
			return;
		}
	}

	// 输出站点所在线路及其所有别名
	printf("%s站：可乘坐", input);
	for (int i = 0; i < row_in_route; i++)
	{
		if (!route || !route[i]) continue;

		// 复制字符串
		char* route_copy = strdup(route[i]);
		if (route_copy == NULL) continue;

		// 分割站点并逐一与用户输入比较
		char* token = strtok(route_copy, "|");
		while (token != NULL)
		{
			char* start = token;
			while (*start == ' ' || *start == '\t') start++;

			char* end = start + strlen(start) - 1;
			while (end > start && (*end == ' ' || *end == '\t')) end--;
			*(end + 1) = '\0';

			if (strcmp(start, input) == 0) {
				printf("%d路 ", i + 1);
				break;
			}

			token = strtok(NULL, "|");
		}

		free(route_copy);
	}
	printf("\n站点别名、周边建筑与地标：");
	for (int i = 0; i < row_in_alias; i++)
	{
		if (!alias || !alias[i]) continue;
		char* alias_copy = strdup(alias[i]);
		if (alias_copy == NULL) continue;

		// 分割站点名
		char* token = strtok(alias_copy, "|");
		char* start = token;
		while (*start == ' ' || *start == '\t') start++;
		char* end = start + strlen(start) - 1;
		while (end > start && (*end == ' ' || *end == '\t')) end--;
		*(end + 1) = '\0';
		if (strcmp(start, input) == 0) {
			printf("%s\n", alias[i]);
			free(alias_copy);
			break;
		}
	}

	free(input);
	system("pause");
	system("cls");
}