#include <bits/stdc++.h>

using namespace std;

// функция используется для восстановления ответа
void dfs(
    int v, 
    unsigned long long now_distance,
    vector<vector<pair<int, unsigned long long>>> &g,
    vector<pair<unsigned long long, vector<int>>> &all_pathes,
    size_t k,
    vector<multiset<unsigned long long>> &distances, 
    unsigned long long true_distance,
    vector<int> &now_path
)
{
    if(all_pathes.size() == k)
        return;

    now_path.push_back(v);

    if(v == 0 && now_distance == 0){
        all_pathes.push_back({true_distance, now_path});
        reverse(all_pathes.back().second.begin(), all_pathes.back().second.end());
        return;
    }

    for(auto [u, w] : g[v]){
        if(distances[u].count(now_distance - w)){
            dfs(
                u,
                now_distance - w,
                g,
                all_pathes,
                k,
                distances,
                true_distance,
                now_path
            );
        }
    }
}

// функция возвращает список из number_of_paths пар вида {distance, path}, где 
// distance -- длина минимального пути от 0 до n - 1,
// path -- сам путь
// В случае, если number_of_paths путей не существует, возвращается список меньшего размера.

vector<pair<unsigned long long, vector<int>>> modified_dijksrta(vector<vector<pair<int, unsigned long long>>> &g, size_t number_of_paths){
    size_t n = g.size();

    multiset<pair<unsigned long long, int>> st; // С помощью сета поддерижваем необработанные вершины ({distance, vertex})

    st.insert({0ll, 0});

    vector<multiset<unsigned long long>> distances(n);
    // distances[v] -- множество длин первых number_of_paths кратчайших путей. Если путей меньше, то само множество может быть меньше number_of_paths
    // бесконечности поддерживать не будем, они без надобности

    distances[0].insert(0);

    while(st.size()){
        // берем минимальное необработанное расстояние и удаляем его из сета (как бы помечая, что оно обработано)
        auto p = *st.begin(); 
        st.erase(st.begin());

        auto d = p.first; // расстояние
        auto v = p.second; // вершина

        for(auto [u, w] : g[v]){ // (v, u, w) -- ребро между вершинами v и u с весом w
            auto new_d = d + w;

            st.insert({new_d, u});
            distances[u].insert(new_d);

            if(distances[u].size() > number_of_paths){ // если расстояний больше, чем number_of_paths, то удалим лишние (иначе алгоритм может работать бесконечно)
                auto deleted_d = *distances[u].rbegin();
                distances[u].erase(distances[u].find(deleted_d));
                st.erase(st.find({deleted_d, u}));
            }
        }
    }

    // для восстанавление ответа нужно будет построить реверсированный граф
    vector<vector<pair<int, unsigned long long>>> rg(n);

    for(int v = 0; v < n; v++)
        for(auto [u, w] : g[v])
            rg[u].push_back({v, w});

    // восстановление ответа
    vector<pair<unsigned long long, vector<int>>> result = {};

    // Для каждой длины будем перебирать все пути (но не больше, чем нужно), которые есть с выбранной длинной рекурсивно
    // Кроме того, будем считать, что в графе нет циклов нулевой длины, через который пройдет один из путей, 
    // потому что в таком случае рекурсия может работать бесконечно;
    // Впрочем, такой случай можно обработать. Если мы наткнулись на цикл нулевой длины, то это значит, что существует бесконечное количество
    // Путей одинаковой длины (мы можем сколько угодно раз пройти по циклу)

    // Стоит отметить, что восстановление пути может повлиять на асимптотику решения. Дело в том, что k-й непростой путь может иметь
    // порядка nk ребер (например, если граф является циклом). Это значит, что суммарная длина всех путей может быть порядка k^2 * n.
    // Тем не менее, при случайных графах все работает хорошо, поскольку при не специально выбранных графах количество ребер
    // в пути будет вряд ли больше m.

    for(auto [i, it, last] = tuple{0, distances[n - 1].begin(), 0ull}; it != distances[n - 1].end(); it++, i++){
        auto d = *it;

        if(d != last)
            dfs(n - 1, d, rg, result, number_of_paths, distances, d, *(new vector<int>()));

        last = d;
    }

    return result;
}

signed main(){

}