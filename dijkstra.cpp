#include <bits/stdc++.h>

using namespace std;

multiset<long long> dijksrta(vector<vector<pair<int, long long>>> &g, size_t k){
    size_t n = g.size();

    multiset<pair<long long, int>> st; // С помощью сета поддерижваем необработанные вершины ({distance, vertex})

    st.insert({0ll, 0});

    vector<multiset<long long>> distances(n);
    // distances[v] -- множество длин первых k кратчайших путей. Если путей меньше, то само множество может быть меньше k
    // бесконечности поддерживать не будем, они без надобности

    distances[0].insert(0);

    while(st.size()){
        // берем минимальное необработанное расстояние и удаляем его из сета (как бы помечая, что оно обработано)
        auto p = *st.begin(); 
        st.erase(st.begin());

        long long d = p.first; // расстояние
        int v = p.second; // вершина

        for(auto [u, w] : g[v]){ // (v, u, w) -- ребро между вершинами v и u с весом w
            long long new_d = d + w;

            st.insert({new_d, u});
            distances[u].insert(new_d);

            if(distances[u].size() > k){ // если расстояний больше, чем k, то удалим лишние (иначе алгоритм может работать бесконечно)
                long long deleted_d = *distances[u].rbegin();
                distances[u].erase(distances[u].find(deleted_d));
                st.erase(st.find({deleted_d, u}));
            }
        }
    }

    return distances[n - 1];
}

signed main(){

}