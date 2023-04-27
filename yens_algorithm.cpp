#include <bits/stdc++.h>

using namespace std;

// функция возвращает пару из {distance, path}, где 
// distance -- длина минимального пути от 0 до n - 1,
// path -- сам путь

template<typename container_of_pil>
pair<unsigned long long, vector<int>> default_dijksrta(const vector<container_of_pil> &g){
    const unsigned long long INF64 = 6e18;

    auto n = g.size();
    vector<unsigned long long> distance(n, INF64);
    vector<int> parent(n, -1);
    distance[0] = 0;
    priority_queue<pair<unsigned long long, int>, vector<pair<unsigned long long, int>>, greater<pair<unsigned long long, int>>> q;
    q.push({0ull, 0});

    while(q.size()){
        auto temp = q.top();
        q.pop();

        auto d = temp.first;
        auto v = temp.second;

        if(d != distance[v] || d == INF64)
            continue;

        for(auto [u, w] : g[v]){
            if(d + w < distance[u]){
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

    bool operator < (const Path &another){
        if(distance != another.distance) return distance < another.distance;
        return order < another.order;
    }

    bool operator == (const Path &another){
        return distance == another.distance && order == another.order;
    }
};

// возвращает отсортированный список из number_of_paths пар вида {distance, path}, где
// distance -- длина очередного кратчайшего пути
// path -- сам путь

vector<pair<unsigned long long, vector<int>>> yens_algorithm(const vector<vector<pair<int, unsigned long long>>> &g_vector, size_t number_of_paths){
    vector<Path> chosen(number_of_paths);
    chosen[0] = Path(default_dijksrta(g_vector));

    size_t n = g_vector.size();
    unordered_map<pair<int, int>, unsigned long long> g_matrix;

    for(size_t v = 0; v < n; v++)
        for(auto [u, w] : g_vector[v])
            g_matrix[{v, u}] = w;

    vector<Path> candidates = {};

    for(size_t k = 1; k < number_of_paths; k++){
        // поскольку в алгоритме Йена необходимо удалять/добавлять ребра в граф, воспользуемся
        // хеш-таблицей, время выполнения указанных операций в которой равно О(1) в среднем
        auto get_graph_without_deleted_edges{
            [&g_vector, n](unordered_set<pair<pair<int, int>, unsigned long long>> &deleted_edges){
                vector<unordered_set<pair<int, unsigned long long>>> res(n);

                for(size_t v = 0; v < n; v++)
                    for(auto [u, w] : g_vector[v])
                        if(deleted_edges.count({{v, u}, w}) == 0)
                            res[v].insert({u, w});

                return res;
            }
        };

        auto &pth = chosen[k - 1];
        auto del = pth.deleted_edges;

        auto g_set = get_graph_without_deleted_edges(del);

        for(size_t i = 0; i + 1 < pth.order.size(); i++){
            auto v = pth.order[i];
            auto u = pth.order[i + 1];
            auto w = g_matrix[{v, u}];

            del.insert({{v, u}, w});
            g_set[v].erase({u, w});

            candidates.push_back(Path(default_dijksrta(g_set), del));

            del.erase({{v, u}, w});
            g_set[v].insert({u, w});
        }

        if(candidates.size() == 0)
            break;

        sort(candidates.begin(), candidates.end());
        candidates.resize(unique(candidates.begin(), candidates.end()) - candidates.begin());
        chosen[k] = candidates[0];
        candidates.erase(candidates.begin());
    }

    while(chosen.back().order.size() == 0)
        chosen.pop_back();

    vector<pair<unsigned long long, vector<int>>> result(chosen.size());

    for(size_t i = 0; i < chosen.size(); i++)
        result[i] = {chosen[i].distance, chosen[i].order};

    return result;
}

signed main() {

}