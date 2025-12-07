// search.h: 声明搜索站点相关的函数
#pragma once
int search_in_same_route(char*, char*, int*, char**);
int search_in_different_route(char*, char*);
void check_path(char**, int, int*);
int check_station(char*);
char* find_station_by_alias(char* station);
void show_alias_options(char* input, char*** matches, int* match_count);
int fuzzy_search(char*);