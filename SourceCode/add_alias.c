// add_alias.c: 添加站点别名、周边建筑及地标
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include "read.h"
#include "search.h"

//验证站点
static char* get_valid_station_name(void) {
    const size_t BUF = 22;
    char* input = malloc(BUF);
    char* formal_name = NULL;

    if (!input) {
        printf("内存分配失败！\n");
        return NULL;
    }

    while (1) {
        printf("请输入要添加别名的站点（支持别名和模糊搜索）：");
        if (fgets(input, BUF, stdin) == NULL) {
            free(input);
            return NULL;
        }

        // 去除末尾换行
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
            len--;
        }

        // 去除首尾空格
        while (len > 0 && isspace((unsigned char)input[0])) {
            memmove(input, input + 1, len);
            len--;
        }

        while (len > 0 && isspace((unsigned char)input[len - 1])) {
            input[len - 1] = '\0';
            len--;
        }

        if (len == 0) {
            printf("输入不能为空！\n");
            continue;
        }

        // 检查是否是正式站点名
        if (check_station(input)) {
            formal_name = _strdup(input);
            free(input);
            return formal_name;
        }

        // 尝试模糊搜索
        printf("正在通过别名查找站点...\n");
        if (fuzzy_search(input)) {
            formal_name = _strdup(input);
            free(input);
            return formal_name;
        }

        printf("未找到站点 \"%s\"，请重新输入。\n", input);
    }

    free(input);
    return NULL;
}

// 检查别名 站点名是否已存在
static int is_name_exist(const char* name) {
    if (!name) return 0;

    // 站名相关
    char* name_copy = _strdup(name);
    if (!name_copy) return 0;

    int is_formal_station = check_station(name_copy);
    free(name_copy);

    if (is_formal_station) {
        return 1;
    }

    // 别名相关
    int row_alias = 0;
    char** aliases = read_alias(&row_alias);
    if (!aliases || row_alias <= 0) {
        if (aliases) {
            for (int i = 0; i < row_alias; i++) free(aliases[i]);
            free(aliases);
        }
        return 0;
    }

    // 遍历别名文件查找
    for (int i = 0; i < row_alias; i++) {
        if (!aliases[i]) continue;

        char* alias_copy = _strdup(aliases[i]);
        if (!alias_copy) continue;

        // 分割每一行的所有别名
        char* token = strtok(alias_copy, "|");
        while (token) {
            // 去除空格
            char* trimmed_token = token;
            while (*trimmed_token == ' ' || *trimmed_token == '\t') trimmed_token++;

            char* end = trimmed_token + strlen(trimmed_token) - 1;
            while (end > trimmed_token && (*end == ' ' || *end == '\t')) {
                *end = '\0';
                end--;
            }

            if (strcmp(trimmed_token, name) == 0) {
                free(alias_copy);
                for (int j = 0; j < row_alias; j++) free(aliases[j]);
                free(aliases);
                return 1; // 找到匹配
            }

            token = strtok(NULL, "|");
        }

        free(alias_copy);
    }

    for (int i = 0; i < row_alias; i++) free(aliases[i]);
    free(aliases);

    return 0; // 未找到
}

// 获取有效的新别名
static char* get_valid_alias(const char* station_name) {
    char alias[100] = { 0 };

    while (1) {
        printf("请输入要添加的新别名/周边建筑/地标（不超过10个字符，注意不要输入“|”）：");
        if (fgets(alias, sizeof(alias), stdin) == NULL) {
            return NULL;
        }

        // 去除末尾换行
        size_t len = strlen(alias);
        if (len > 0 && alias[len - 1] == '\n') {
            alias[len - 1] = '\0';
            len--;
        }

        // 去除首尾空格
        while (len > 0 && isspace((unsigned char)alias[0])) {
            memmove(alias, alias + 1, len);
            len--;
        }

        while (len > 0 && isspace((unsigned char)alias[len - 1])) {
            alias[len - 1] = '\0';
            len--;
        }

        // 检查用户输入是否为空
        if (len == 0) {
            printf("别名不能为空！\n");
            continue;
        }

        // 检查是否超限
        if (len > 10) {
            printf("别名过长，请控制在10个字符以内！\n");
            continue;
        }

        // 检查是否包含管道符
        if (strchr(alias, '|')) {
            printf("别名不能包含 '|' 字符！\n");
            continue;
        }

        // 检查是否与站点名相同
        if (strcmp(alias, station_name) == 0) {
            printf("别名不能与站点名相同！\n");
            continue;
        }

        // 检查别名是否已存在
        if (is_name_exist(alias)) {
            printf("别名 \"%s\" 已存在！请重新输入。\n", alias);
            continue;
        }

        return _strdup(alias);
    }
}

// 在别名文件中添加新别名
static int add_alias_to_file(const char* station, const char* new_alias) {
    if (!station || !new_alias) return 0;

    // 读取别名文件
    int row_alias = 0;
    char** aliases = read_alias(&row_alias);
    if (!aliases) return 0;

    // 查找站点所在行
    int station_index = -1;

    for (int i = 0; i < row_alias; i++) {
        if (!aliases[i]) continue;

        char* alias_copy = _strdup(aliases[i]);
        if (!alias_copy) continue;

        // 获取正式站点名（第一个|之前的部分）
        char* formal_name = strtok(alias_copy, "|");
        if (formal_name) {
            // 去除空格
            char* trimmed_name = formal_name;
            while (*trimmed_name == ' ' || *trimmed_name == '\t') trimmed_name++;

            char* end = trimmed_name + strlen(trimmed_name) - 1;
            while (end > trimmed_name && (*end == ' ' || *end == '\t')) {
                *end = '\0';
                end--;
            }

            if (strcmp(trimmed_name, station) == 0) {
                station_index = i;
                free(alias_copy);
                break;
            }
        }

        free(alias_copy);
    }

    // 开始添加用户输入的内容
    int success = 0;
    FILE* fp = NULL;

    if (station_index >= 0) {
        // 在现有行中添加
        fp = fopen("config/alias_beside.cfg", "r+");
        if (!fp) goto cleanup;

        // 移动文件指针到对应行
        fseek(fp, 0, SEEK_SET);
        for (int i = 0; i < station_index; i++) {
            char buffer[256];
            if (!fgets(buffer, sizeof(buffer), fp)) goto cleanup;
        }

        // 读取当前行
        long line_start = ftell(fp);
        char line[512];
        if (!fgets(line, sizeof(line), fp)) goto cleanup;

        // 去除末尾换行
        size_t line_len = strlen(line);
        if (line_len > 0 && line[line_len - 1] == '\n') {
            line[line_len - 1] = '\0';
        }

        // 移动到行首并写入新内容
        fseek(fp, line_start, SEEK_SET);
        fprintf(fp, "%s|%s\n", line, new_alias);
        success = 1;
    }
    else {
        // 添加到文件末尾
        fp = fopen("config/alias_beside.cfg", "a");
        if (!fp) goto cleanup;

        fprintf(fp, "%s|%s\n", station, new_alias);
        success = 1;
    }

cleanup:
    if (fp) fclose(fp);

    for (int i = 0; i < row_alias; i++) free(aliases[i]);
    free(aliases);

    return success;
}

// 功能函数接口
void aliadd(void) {
    system("cls");
    printf("========== 添加站点别名/周边建筑/地标 ==========\n\n");

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // 获取有效站点名
    char* station_name = get_valid_station_name();
    printf("\n已找到站点：%s\n\n", station_name);

    // 获取有效别名
    char* new_alias = get_valid_alias(station_name);

    // 步骤3：确认添加
    printf("\n请确认添加：\n");
    printf("站点：%s\n", station_name);
    printf("新别名：%s\n", new_alias);
    printf("确认添加吗？(y/n): ");

    char confirm;
    scanf("%c", &confirm);
    while ((c = getchar()) != '\n' && c != EOF); // 清理缓冲区

    if (confirm == 'y' || confirm == 'Y') {
        // 步骤4：添加到文件
        if (add_alias_to_file(station_name, new_alias)) {
            printf("\n 成功添加别名 \"%s\" 到站点 \"%s\"！\n", new_alias, station_name);
        }
        else {
            printf("\n 添加失败，请检查文件权限或磁盘空间！\n");
        }
    }
    else {
        printf("\n操作已取消。\n");
    }

    // 清理内存
    free(station_name);
    free(new_alias);

    system("pause");
    system("cls");
}