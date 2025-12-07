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
        case 0:
            printf("感谢您的使用，再见！\n");
            system("pause");
            free_graph();
            exit(0); // 退出程序
        case 1:
            print_route();
            break; // 函数功能：打印校车路线表
        case 2:
            print_alias();
            break; // 函数功能：打印别名和地标
        case 3:
            search_route();
            break; // 函数功能：提供乘车方案，实现一系列功能
        case 4:
            aliadd();
            break; // 添加站点别名、周边建筑及地标
        case 5:
            query();
            break; // 查询站点、别名、周边建筑及地标。功能：1.输入站点查询别名及该站点可乘的车；2.输入别名（调用模糊搜索）查找站点，实现功能1
        default:
            break;
        }
    }
    return -1; // 循环没有正常执行；程序异常退出，抛出异常。
}