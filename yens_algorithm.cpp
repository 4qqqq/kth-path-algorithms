#include <bits/stdc++.h>

using namespace std;

const unsigned long long INF64 = 6e18;

// функция возвращает пару из {distance, path}, где 
// distance -- длина минимального пути от 0 до n - 1,
// path -- сам путь

template<typename container_of_pil>
pair<unsigned long long, vector<int>> default_dijksrta(
    const vector<container_of_pil>& g,
    int start,
    const unordered_set<int>& deleted_vertex
){
    auto n = g.size();
    vector<unsigned long long> distance(n, INF64);
    vector<int> parent(n, -1);
    distance[start] = 0;
    priority_queue<pair<unsigned long long, int>, vector<pair<unsigned long long, int>>, greater<pair<unsigned long long, int>>> q;
    q.push({0ull, start});

    while(q.size()){
        auto temp = q.top();
        q.pop();

        auto d = temp.first;
        auto v = temp.second;

        if(d != distance[v] || d == INF64)
            continue;

        for(auto [u, w] : g[v]){
            if(deleted_vertex.count(u) == 0 && d + w < distance[u]){
                distance[u] = d + w;
                q.push({distance[u], u});
                parent[u] = v;
            }
        }
    }

    vector<int> path;
    int now = n - 1;

    while(now != -1){
        path.push_back(now);
        now = parent[now];
    }

    reverse(path.begin(), path.end());

    return {distance[n - 1], path};
}

// определим значение hash для пары, чтобы можно было пользоваться unordered_map<> и unordered_set<>
namespace std {
    template<typename A, typename B>
    class hash<pair<A, B>>{
        public:

        unsigned long long operator()(const pair<A, B>& value) const {
            auto l = hash<A>()(value.first);
            auto r = hash<B>()(value.second);
            return l * 347 + r;
        }
    };
}


// возвращает отсортированный список из number_of_paths пар вида {distance, path}, где
// distance -- длина очередного кратчайшего пути
// path -- сам путь

vector<pair<unsigned long long, vector<int>>> yens_algorithm(const vector<vector<pair<int, unsigned long long>>> &g_vector, size_t number_of_paths){
    // структура для поддержки путей в алгоритме Йена
    struct Path{
        unsigned long long distance;
        vector<int> order;
        unordered_set<pair<pair<int, int>, unsigned long long>> deleted_edges;

        Path() :  
            distance(0),
            order({}),
            deleted_edges({})
        {}

        Path(const pair<unsigned long long, vector<int>> &p) :
            distance(p.first),
            order(p.second),
            deleted_edges({})
        {}

        Path(const pair<unsigned long long, vector<int>> &p, const unordered_set<pair<pair<int, int>, unsigned long long>> &deleted_edges) :
            distance(p.first),
            order(p.second),
            deleted_edges(deleted_edges)
        {}

        pair<unsigned long long, vector<int>> to_vector(){
            return {distance, order};
        }

        bool operator < (const Path &another) const {
            if(distance != another.distance) return distance < another.distance;
            return order < another.order;
        }

        bool operator == (const Path &another){
            return distance == another.distance && order == another.order;
        }
    };

    size_t n = g_vector.size();

    set<Path> candidates;
    vector<Path> chosen = {Path(default_dijksrta(g_vector, 0, *(new unordered_set<int>())))};

    unordered_map<pair<int, int>, unsigned long long> g_matrix;

    for(int v = 0; v < n; v++)
        for(auto [u, w] : g_vector[v])
            g_matrix[{v, u}] = w;

    for(size_t k = 1; k < number_of_paths; k++){
        unordered_set<int> deleted_vertex = {};
        const auto& p = chosen.back();
        auto deleted_edges = p.deleted_edges;
        vector<unordered_set<pair<unsigned long long, int>>> g_set(n);

        for(int v = 0; v < n; v++)
            for(auto [u, w] : g_vector[v])
                if(!deleted_edges.count({{v, u}, w}))
                    g_set[v].insert({u, w});

        unsigned long long added_distance = 0;
        vector<int> prefix = {};

        for(int i = 0; i + 1 < p.order.size(); i++){
            auto v = p.order[i];
            auto u = p.order[i + 1];
            auto w = g_matrix[{v, u}];
            g_set[v].erase({u, w});
            deleted_edges.insert({{v, u}, w});

            auto pp = default_dijksrta(g_set, v, deleted_vertex);

            if(pp.first != INF64){
                vector<int> real_order = prefix;
                real_order.insert(real_order.end(), pp.second.begin(), pp.second.end());
                candidates.insert(Path({added_distance + pp.first, real_order}, deleted_edges));
            }

            deleted_edges.erase({{v, u}, w});
            added_distance += w;
            deleted_vertex.insert(v);
            prefix.push_back(v);
        }

        while(candidates.size() > number_of_paths)
            candidates.erase(candidates.find(*candidates.rbegin()));

        if(candidates.size()){
            chosen.push_back(*candidates.begin());
            candidates.erase(candidates.begin());
        }else{
            break;
        }
    }

    vector<pair<unsigned long long, vector<int>>> result(chosen.size());

    for(int i = 0; i < chosen.size(); i++)
        result[i] = chosen[i].to_vector();

    return result;
}

signed main() {

}