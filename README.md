# kth-path-algorithms
Репозиторий для хранения реализаций алгоритмов нахождения k-го пути во взвешенном орграфе. Эти реализации в первую очередь созданы для курсовой работы.

## yens_algorithm.cpp
Реализация алгоритма Йены в виде функции 
```c++ 
vector<pair<unsigned long long, vector<int>>> yens_algorithm(const vector<vector<pair<int, unsigned long long>>> &g_vector, size_t number_of_paths) ```, где:

```c++ const vector<vector<pair<int, unsigned long long>>> &g_vector``` --- список смежностей графа, т.е. g_vector[u] := список пар вида {v, w}, каждая из которых означает наличие ориентрованногоребра (u, v, w) в графе g;

```c++ size_t number_of_paths ``` --- количество путей, которые нужно найти алгоритмом (другими словами, число )

## dijkstra.cpp