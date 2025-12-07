// UI.c: 实现利用命令行生成文字式可视化界面
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int UI(void)
{
	int select = 0;
	while (1)
	{
		printf("欢迎使用西南大学校车路线推荐系统！（Version-20251207-Debug）\n");
		printf("************************************\n");
		printf("* 1 校车路线表\t\t2 别名一览 *\n");
		printf("* 3 校车乘坐推荐                   *\n");
		printf("* 4 站点、别名、周边建筑及地标添加 *\n");
		printf("* 5 站点、别名、周边建筑及地标查询 *\n");
		printf("************************************\n");
		printf("请键入您的选择(1-5，输入0退出程序): ");
		if (!(scanf("%d", &select)) || select < 0 || select >= 6)
		{
			fprintf(stderr, "输入有误,请重新输入！\n");
			clearerr(stdin);
			system("pause");
			system("cls");
			continue;
		}
		system("cls");
		break;
	}
	return select;
}