// file_check.c: 检查存储线路和站点别名的文件是否存在，若无，则创建并写入；若仍无法创建写入，则报错并退出程序。
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>

void file_check(void)
{
    // 检查并创建 route.cfg
    
    FILE* route = fopen("config/route.cfg", "r");
    if (route != NULL)
    {
        fclose(route);
    }
    else
    {
        MessageBoxA(NULL, "未找到路线配置文件！将创建默认文件。", "提示", MB_OK);
        char path[] = "config";
        if (_mkdir(path))
        {
            MessageBoxA(NULL, "无法创建存放配置文件的路径，请确认程序运行的目录是否拥有修改权限！", "错误", MB_OK | MB_ICONERROR);
            exit(-1);
        }
        route = fopen("config/route.cfg", "w");
        if (route != NULL)
        {
            const char* default_route =
                "1路(一路)经管院|六号门|二号门|八教|田家炳|圆顶|五号门\n"
                "2路(二路)经管院|六号门|二号门|八教|田家炳|西师街|五号门\n"
                "3路(三路)竹园|二号门|五教|梅园|橘园|五号门\n"
                "4路(四路)二号门|二食堂|地科院|外语学院|二十六教|梅园|橘园\n"
                "5路(五路)二号门|八教|田家炳|圆顶|五号门\n"
                "6路(六路)竹园|二号门|梅园|橘园|田家炳|八教|二号门|竹园\n"
                "7路(七路)二号门|地科院|五教|二十六教|梅园|橘园|田家炳|李园|大礼堂|二号门\n"
                "8路(八路)音乐学院|八教|田家炳|橘园|梅园|外语学院|音乐学院\n"
                "9路(九路)竹园|二号门|八教|田家炳|圆顶|五号门|橘园|梅园|中图|二号门|竹园\n";
            fprintf(route, "%s", default_route);
            MessageBoxA(NULL, "创建成功！配置文件已保存至“程序目录/config”中！", "提示", MB_OK);
            fclose(route);
        }
        else
        {
            MessageBoxA(NULL, "无法创建路线配置文件！", "错误", MB_OK | MB_ICONERROR);
            exit(-1);
        }
    }

    /* 检查并创建 alias_beside.cfg */
    FILE* alias = fopen("config/alias_beside.cfg", "r");
    if (alias != NULL)
    {
        fclose(alias);
    }
    else
    {
        MessageBoxA(NULL, "未找到别名及周边建筑配置文件！将创建默认文件。", "提示", MB_OK);
        alias = fopen("config/alias_beside.cfg", "w");
        if (alias != NULL)
        {
            const char* default_alias =
                "经管院|\n"
                "六号门|\n"
                "二号门|\n"
                "八教|\n"
                "田家炳|\n"
                "行署楼 一号门|\n"
                "圆顶|\n"
                "五号门|\n"
                "西师街|\n"
                "竹园|\n"
                "五教|\n"
                "十教|\n"
                "梅园|\n"
                "橘园|\n"
                "二食堂|\n"
                "地科院|\n"
                "外语学院|\n"
                "二十六教|\n"
                "李园|\n"
                "大礼堂|\n"
                "音乐学院|\n"
                "中图|";
            fprintf(alias, "%s", default_alias);
            MessageBoxA(NULL, "创建成功！配置文件已保存至“程序目录/config”中！", "提示", MB_OK);
            fclose(alias);
        }
        else
        {
            MessageBoxA(NULL, "无法创建别名及周边建筑配置文件！", "错误", MB_OK | MB_ICONERROR);
            exit(-1);
        }
    }
}