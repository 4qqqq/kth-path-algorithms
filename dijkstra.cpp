#include <bits/stdc++.h>

using namespace std;

// функция возвращает пару {true, длина k-го кратчайшего пути}
// В случае, если number_of_paths путей не существует, возвращается {false, 0}

pair<bool, unsigned long long> modified_dijksrta(vector<vector<pair<int, unsigned long long>>> &g, size_t number_of_paths){
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

    return (distances[n - 1].size() == number_of_paths ? make_pair(true, *distances[n - 1].rbegin()) : make_pair(false, 0ull));
}

signed main(){

}