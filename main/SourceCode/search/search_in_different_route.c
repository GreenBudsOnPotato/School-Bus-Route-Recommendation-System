// search_in_different_route.c: 查找换乘方案
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <direct.h>
#include <Windows.h>
#include <errno.h>
#include "read.h"

// 定义常量
#define MAX_SOLUTIONS 5     // 最多输出5条方案
#define INF INT_MAX
#define TRANSFER_COST 1000  // 换乘代价大，确保优先选择换乘少的方案

typedef struct Edge Edge;
typedef struct Node Node;

struct Node {
    char* station;      // 站点名称
    int route_id;       // 所属线路ID
    int visited;        // 访问标记
    int distance;       // 距离起点距离（最短）
    int transfer_count; // 换乘次数
    Node* prev;         // 前驱节点
    Edge* edges;        // 邻接边
};

struct Edge {
    Node* to;           // 目标节点
    int weight;         // 权重
    int is_transfer;    // 是否为换乘边
    Edge* next;         // 下一条边
};

// 解决方案结构
typedef struct {
    Node** path;        // 路径节点数组
    int path_length;    // 路径长度
    int transfer_count; // 换乘次数
    int student_fare;   // 学生票价
    int normal_fare;    // 普通票价
} Solution;

static Node** graph_nodes = NULL;
static int node_count = 0;          // 节点数量
static int node_capacity = 0;       // 当前分配容量

// 去除 token 两端空白
static char* trim_token(char* token) {
    if (!token) return token;
    while (*token && isspace((unsigned char)*token)) token++;
    char* end = token + strlen(token) - 1;
    while (end >= token && isspace((unsigned char)*end)) { *end = '\0'; end--; }
    return token;
}

// 创建新节点
static Node* create_node(const char* station, int route_id) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;
    node->station = _strdup(station);
    node->route_id = route_id;
    node->visited = 0;
    node->distance = INF;
    node->transfer_count = 0;
    node->prev = NULL;
    node->edges = NULL;
    return node;
}

// 添加边
static int add_edge(Node* from, Node* to, int weight, int is_transfer) {
    if (!from || !to) return 0;
    Edge* edge = (Edge*)malloc(sizeof(Edge));
    if (!edge) return 0;
    edge->to = to;
    edge->weight = weight;
    edge->is_transfer = is_transfer;
    edge->next = from->edges;
    from->edges = edge;
    return 1;
}

// 查找或创建节点（防止已存在站点重复创建）
static Node* find_or_create_node(char* station, int route_id) {
    if (!station) return NULL;
    for (int i = 0; i < node_count; i++) {
        if (graph_nodes[i] && strcmp(graph_nodes[i]->station, station) == 0 && graph_nodes[i]->route_id == route_id) {
            return graph_nodes[i];
        }
    }

    // 扩容策略
    if (node_count >= node_capacity) {
        int new_cap = node_capacity ? node_capacity * 2 : 16;
        Node** tmp = (Node**)realloc(graph_nodes, new_cap * sizeof(Node*));
        if (!tmp) return NULL;
        graph_nodes = tmp;
        node_capacity = new_cap;
    }

    Node* new_node = create_node(station, route_id);
    if (!new_node) return NULL;
    graph_nodes[node_count] = new_node;
    node_count++;
    return new_node;
}

// 构建图
void build_graph() {
    if (node_count > 0) return;

    int route_count = 0;
    char** all_routes = read_route(&route_count);
    if (!all_routes || route_count <= 0) {
        if (all_routes) free(all_routes);
        return;
    }

    // 创建所有节点（按线路）
    for (int i = 0; i < route_count; i++) {
        if (!all_routes[i]) continue;

        char* route_copy = _strdup(all_routes[i]);
        if (!route_copy) continue;

        char* token = strtok(route_copy, "|");
        while (token) {
            char* t = trim_token(token);
            if (t && *t) {
                Node* node = find_or_create_node(t, i);
            }
            token = strtok(NULL, "|");
        }

        free(route_copy);
    }

    // 添加线路内部的相邻边
    for (int i = 0; i < route_count; i++) {
        if (!all_routes[i]) continue;

        char* route_copy = _strdup(all_routes[i]);
        if (!route_copy) continue;

        Node* prev_node = NULL;
        char* token = strtok(route_copy, "|");
        while (token) {
            char* t = trim_token(token);
            if (t && *t) {
                // 找到当前节点（route_id 与 station 均匹配）
                Node* current_node = NULL;
                for (int j = 0; j < node_count; j++) {
                    if (graph_nodes[j] && strcmp(graph_nodes[j]->station, t) == 0 && graph_nodes[j]->route_id == i) {
                        current_node = graph_nodes[j];
                        break;
                    }
                }

                if (prev_node && current_node) {
                    add_edge(prev_node, current_node, 1, 0);
                    add_edge(current_node, prev_node, 1, 0);
                }
                prev_node = current_node;
            }
            token = strtok(NULL, "|");
        }

        free(route_copy);
    }

    // 添加换乘边（站名相同但 route_id 不同）
    for (int i = 0; i < node_count; i++) {
        if (!graph_nodes[i]) continue;
        for (int j = i + 1; j < node_count; j++) {
            if (!graph_nodes[j]) continue;
            if (strcmp(graph_nodes[i]->station, graph_nodes[j]->station) == 0 && graph_nodes[i]->route_id != graph_nodes[j]->route_id) {
                add_edge(graph_nodes[i], graph_nodes[j], TRANSFER_COST, 1);
                add_edge(graph_nodes[j], graph_nodes[i], TRANSFER_COST, 1);
            }
        }
    }

    // 释放 read_route 返回的内存
    for (int i = 0; i < route_count; i++) free(all_routes[i]);
    free(all_routes);
}

// Dijkstra
void dijkstra(Node* start) {
    if (!start) return;

    for (int i = 0; i < node_count; i++) {
        if (graph_nodes[i]) {
            graph_nodes[i]->distance = INF;
            graph_nodes[i]->visited = 0;
            graph_nodes[i]->prev = NULL;
            graph_nodes[i]->transfer_count = 0;
        }
    }

    start->distance = 0;
    start->transfer_count = 0;

    for (int i = 0; i < node_count; i++) {
        Node* current = NULL;
        int min_dist = INF;

        for (int j = 0; j < node_count; j++) {
            if (graph_nodes[j] && !graph_nodes[j]->visited && graph_nodes[j]->distance < min_dist) {
                min_dist = graph_nodes[j]->distance;
                current = graph_nodes[j];
            }
        }

        if (!current || min_dist == INF) break;

        current->visited = 1;

        Edge* edge = current->edges;
        while (edge) {
            if (!edge->to->visited) {
                int new_dist = current->distance + edge->weight;
                int new_transfer = current->transfer_count + edge->is_transfer;
                if (new_dist < edge->to->distance || (new_dist == edge->to->distance && new_transfer < edge->to->transfer_count)) {
                    edge->to->distance = new_dist;
                    edge->to->transfer_count = new_transfer;
                    edge->to->prev = current;
                }
            }
            edge = edge->next;
        }
    }
}

// 查找包含某站点的所有节点
Node** find_nodes_with_station(char* station, int* count) {
    if (!count) return NULL;
    *count = 0;
    if (!station) return NULL;

    Node** result = NULL;
    int cap = 0;

    for (int i = 0; i < node_count; i++) {
        if (!graph_nodes[i]) continue;
        if (strcmp(graph_nodes[i]->station, station) == 0) {
            if (*count >= cap) {
                int new_cap = cap ? cap * 2 : 8;
                Node** tmp = (Node**)realloc(result, new_cap * sizeof(Node*));
                if (!tmp) {
                    free(result);
                    *count = 0;
                    return NULL;
                }
                result = tmp;
                cap = new_cap;
            }
            result[*count] = graph_nodes[i];
            (*count)++;
        }
    }
    return result;
}

// 检查路径是否有效：换乘站不能是起点或终点
static int is_valid_path(Node* start_node, Node* end_node, Node* path_end) {
    if (!start_node || !end_node || !path_end) return 0;
    if (strcmp(start_node->station, end_node->station) == 0) return 1;    // 如果起点和终点相同，直接返回有效

    int length = 0;
    Node* cur = path_end;
    while (cur) {
        length++;
        cur = cur->prev;
    }

    if (length <= 1) return 1;

    // 重建路径并检查
    Node** path = (Node**)malloc(length * sizeof(Node*));
    if (!path) return 1;

    cur = path_end;
    for (int i = length - 1; i >= 0; i--) {
        path[i] = cur;
        cur = cur->prev;
    }

    for (int i = 1; i < length; i++) {
        if (path[i]->route_id != path[i - 1]->route_id) {
            char* transfer_station = path[i]->station;

            // 检查这个换乘站是否是起点或终点
            if (strcmp(transfer_station, start_node->station) == 0) {
                free(path);
                return 0;
            }
            if (strcmp(transfer_station, end_node->station) == 0) {
                free(path);
                return 0;
            }
        }
    }

    free(path);
    return 1;
}

// 重建路径
Solution* reconstruct_path(Node* start_node, Node* end_node, Node* path_end) {
    if (!path_end) return NULL;

    // 检查路径是否有效（换乘站不能是起点或终点）
    if (!is_valid_path(start_node, end_node, path_end)) {
        return NULL;
    }

    // 计算路径长度
    int length = 0;
    Node* cur = path_end;
    while (cur) {
        length++;
        cur = cur->prev;
    }
    if (length == 0) return NULL;

    Solution* sol = (Solution*)malloc(sizeof(Solution));
    if (!sol) return NULL;
    sol->path_length = length;
    sol->path = (Node**)malloc(length * sizeof(Node*));
    if (!sol->path) { free(sol); return NULL; }

    // 重新计算换乘次数，排除起点和终点的线路变化
    sol->transfer_count = 0;

    // 重建路径并计算换乘次数
    cur = path_end;
    for (int i = length - 1; i >= 0; i--) {
        sol->path[i] = cur;
        cur = cur->prev;
    }

    // 计算换乘次数
    // 起点索引为0，终点索引为length-1
    for (int i = 1; i < length - 1; i++) {
        if (sol->path[i]->route_id != sol->path[i - 1]->route_id) {
            // 检查这个换乘是否是同一站点
            if (strcmp(sol->path[i]->station, sol->path[i - 1]->station) == 0) {
                sol->transfer_count++;
            }
        }
    }

    sol->student_fare = sol->transfer_count + 1;
    sol->normal_fare = (sol->transfer_count + 1) * 2;

    return sol;
}

// 打印解决方案
void print_solution(Solution* sol, int index) {
    if (!sol) return;
    printf("\n=== 方案 %d ===\n", index + 1);
    printf("换乘次数: %d\n", sol->transfer_count);
    printf("总票价: %d元（学生/教职工）| %d元（其他人员）\n",
        sol->student_fare, sol->normal_fare);
    printf("详细路径:\n");

    int current_route = -1;
    for (int i = 0; i < sol->path_length; i++) {
        Node* node = sol->path[i];
        if (i > 0 && node->route_id != sol->path[i - 1]->route_id) {
            printf("\n  在【%s】站换乘 %d路\n", node->station, node->route_id + 1);
        }
        if (i == 0 || node->route_id != current_route) {
            printf("  %d路: %s", node->route_id + 1, node->station);
            current_route = node->route_id;
        }
        else {
            printf(" → %s", node->station);
        }
    }
    printf("\n");
}

// 打印解决方案到日志文件中
void print_solution_in_different_route_to_file(Solution* sol, int index, FILE* fp) {
    if (!sol || !fp) return;

    fprintf(fp, "\n=== 方案 %d ===\n", index + 1);
    fprintf(fp, "换乘次数: %d\n", sol->transfer_count);
    fprintf(fp, "总票价: %d元（学生/教职工）| %d元（其他人员）\n",
        sol->student_fare, sol->normal_fare);
    fprintf(fp, "详细路径:\n");

    int current_route = -1;
    for (int i = 0; i < sol->path_length; i++) {
        Node* node = sol->path[i];
        if (i > 0 && node->route_id != sol->path[i - 1]->route_id) {
            fprintf(fp, "\n  在【%s】站换乘 %d路\n", node->station, node->route_id + 1);
        }
        if (i == 0 || node->route_id != current_route) {
            fprintf(fp, "  %d路: %s", node->route_id + 1, node->station);
            current_route = node->route_id;
        }
        else {
            fprintf(fp, " → %s", node->station);
        }
    }
    fprintf(fp, "\n");
}

// 比较解决方案（按距离再按换乘次数）
int compare_solutions(const void* a, const void* b) {
    const Solution* sol_a = (const Solution*)a;
    const Solution* sol_b = (const Solution*)b;

    int dist_a = INF, dist_b = INF;
    if (sol_a->path && sol_a->path_length > 0 && sol_a->path[sol_a->path_length - 1]) {
        dist_a = sol_a->path[sol_a->path_length - 1]->distance;
    }
    if (sol_b->path && sol_b->path_length > 0 && sol_b->path[sol_b->path_length - 1]) {
        dist_b = sol_b->path[sol_b->path_length - 1]->distance;
    }

    if (dist_a != dist_b) return (dist_a < dist_b) ? -1 : 1;
    return sol_a->transfer_count - sol_b->transfer_count;
}

// 释放图内存（程序退出时释放，防止重复构图）
void free_graph() {
    for (int i = 0; i < node_count; i++) {
        if (!graph_nodes[i]) continue;
        Edge* edge = graph_nodes[i]->edges;
        while (edge) {
            Edge* next = edge->next;
            free(edge);
            edge = next;
        }
        free(graph_nodes[i]->station);
        free(graph_nodes[i]);
    }
    free(graph_nodes);
    graph_nodes = NULL;
    node_count = 0;
    node_capacity = 0;
}

// 主搜索函数
int search_in_different_route(char* origin, char* dest) {
    if (!origin || !dest) return 0;

    // 确保图已构建
    if (node_count == 0) build_graph();

    // 查找起点和终点的所有节点
    int start_count = 0, end_count = 0;
    Node** start_nodes = find_nodes_with_station(origin, &start_count);
    Node** end_nodes = find_nodes_with_station(dest, &end_count);

    if (!start_nodes || !end_nodes || start_count == 0 || end_count == 0) {
        printf("起点或终点不在任何线路上！\n");
        if (start_nodes) free(start_nodes);
        if (end_nodes) free(end_nodes);
        return 0;
    }

    // 存储所有解决方案
    Solution solutions[MAX_SOLUTIONS];
    int solution_count = 0;

    for (int i = 0; i < start_count && solution_count < MAX_SOLUTIONS; i++) {
        if (!start_nodes[i]) continue;
        dijkstra(start_nodes[i]);

        for (int j = 0; j < end_count && solution_count < MAX_SOLUTIONS; j++) {
            if (!end_nodes[j]) continue;
            if (end_nodes[j]->distance < INF) {
                // 传递起点和终点节点给reconstruct_path进行验证
                Solution* sol = reconstruct_path(start_nodes[i], end_nodes[j], end_nodes[j]);
                if (sol) {
                    // 检查路径是否包含换乘站是起点或终点的情况
                    solutions[solution_count].path_length = sol->path_length;
                    solutions[solution_count].path = (Node**)malloc(sol->path_length * sizeof(Node*));
                    if (solutions[solution_count].path) {
                        memcpy(solutions[solution_count].path, sol->path, sol->path_length * sizeof(Node*));
                        solutions[solution_count].transfer_count = sol->transfer_count;
                        solutions[solution_count].student_fare = sol->student_fare;
                        solutions[solution_count].normal_fare = sol->normal_fare;
                        solution_count++;
                    }
                    free(sol->path);
                    free(sol);
                }
            }
        }
    }

    // 排序解决方案
    if (solution_count > 1) {
        qsort(solutions, solution_count, sizeof(Solution), compare_solutions);
    }

    // 输出结果
    printf("\n======= 换乘方案推荐 =======\n");
    int display_count = (solution_count > 5) ? 5 : solution_count;
    if (solution_count == 0) {
        printf("很抱歉，没有找到合适的换乘方案。\n");
    }
    else {
        for (int i = 0; i < display_count; i++) {
            print_solution(&solutions[i], i);
        }
        printf("\n共找到 %d 种方案，显示前 %d 种最优方案。\n", solution_count, display_count);
        printf("因性能问题，程序暂不支持在需要换乘时考虑途径点，敬请谅解！\n");
    }
    printf("============================\n");

    // 输出乘车方案到日志文件中
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
                return solution_count;
            }
        }

        fp = fopen("output/solution.log", "ab");
        if (!fp) {
            MessageBoxA(NULL, "无法创建或读取到日志文件，请确认程序运行的目录是否拥有修改权限！", "错误", MB_OK | MB_ICONERROR);
            return solution_count;
        }

        fprintf(fp, "********************%s → %s********************", origin, dest);
        for (int i = 0; i < display_count; i++)
            print_solution_in_different_route_to_file(&solutions[i], i, fp);
        fprintf(fp, "************************* (^-^) *************************\n");
        fclose(fp);
        printf("成功将乘车方案输出到日志文件中！\n");
    }

    // 释放内存
    for (int i = 0; i < display_count; i++) {
        free(solutions[i].path);
    }

    free(start_nodes);
    free(end_nodes);

    return solution_count;
}