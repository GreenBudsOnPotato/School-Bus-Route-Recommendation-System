/*西南大学计算机与信息科学学院 软件学院 程序设计1（C语言）-0002 结课项目：西南大学校车路线推荐系统
设计者：龚林枫（软件工程 本科2025级2班 222025321262045）
拟题动机：刚开学时对校车各路和每个站的周边建筑不熟悉，不知道怎么坐。很多内向的同学也不敢开口询问，故设计该软件以便新生报到
程序功能：
    1.输入起点和目的地站（包括周边建筑），返回校车线路（以及如果要换乘的话，返回换乘方案）
    2.用户可以储存和查询各站点周边建筑和地标（未来可以将其用于论坛或社区）（时间够的情况下考虑是否支持别名检索或模糊检索）
    3.更多功能待开发时再探索*/
#include <stdio.h>
#include <stdlib.h>
#include "func.h"
#include "print.h"

int main()
{
    file_check();   // 函数功能：检查存储线路和站点别名的文件是否存在
    int select = 0; // 进入程序，用户键入选择
    while (1)
    {
        select = UI(); // UI界面
        switch (select)
        {
        case 0: printf("感谢您的使用，再见！\n"); system("pause"); free_graph(); exit(0); // 退出程序
        case 1: print_route(); break; // 函数功能：打印校车路线表
        case 2: print_alias(); break; // 函数功能：打印别名和地标
        case 3: search_route(); break; // 函数功能：提供乘车方案，实现一系列功能
        case 4: aliadd(); break;
        case 5: query(); break;
        default: break;
        }
    }
    return -1; // 循环没有正常执行；程序异常退出，抛出异常。
}