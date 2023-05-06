#include <bits/stdc++.h>

using namespace std;

const unsigned long long INF64 = 6e18;

struct Node{
    int dist = 0; // нужно для поддержания работы левосторонней кучи
    int& dist_method(){ return this ? this->dist : *(new int(0)); }

    Node* merge(Node* y){
        Node* x = this->copy();
        if(x == 0) return y;
        if(y == 0) return x;

        if(y->sidetrack < x->sidetrack)
            swap(x, y);

        x->r.first = x->r.first->merge(y);
        x->r.second = (x->r.first ? x->r.first->sidetrack - x->sidetrack : 0);

        if(x->r.first->dist_method() > x->l.first->dist_method())
            swap(x->l, x->r);

        x->dist_method() = x->r.first->dist_method() + 1;

        return x;
    }

    pair<Node*, unsigned long long> l = {(Node*)(nullptr), 0ull},
                                    r = {(Node*)(nullptr), 0ull};

    int id = 0, // id вершины
        v = 0; // существует ребро (x, v, w) [x \in {u, p[u], p[p[u]], ..., t}]

    unsigned long long sidetrack = 0; // key

    Node(){}
    Node(int id, int v, unsigned long long sidetrack) : id(id), v(v), sidetrack(sidetrack) {}

    Node* insert(Node* v){
        return this->merge(v);
    }

    Node* extract_min(){
        return this->l.first->merge(this->r.first);
    }

    Node* copy(){
        if(!this) return 0;

        Node* res = new Node();
        res->id = id;
        res->v = v;
        res->sidetrack = sidetrack;
        res->l = l;
        res->r = r;
        res->dist = dist;
        return res;
    }
};

class cmp{
public:
    bool operator() (const Node* a, const Node* b) const {
        if(a->sidetrack != b->sidetrack) return a->sidetrack < b->sidetrack;
        return a->id < b->id;
    }

    bool operator() (const pair<unsigned long long, Node*> &a, const pair<unsigned long long, Node*> &b) const {
        if(a.first != b.first) return a.first < b.first;
        return a.second->id < b.second->id;
    }
};

template<typename container_of_pil>
vector<unsigned long long> default_dijksrta(
    const vector<container_of_pil>& g,
    int start
){
    auto n = g.size();
    vector<unsigned long long> distance(n, INF64);
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
            if(d + w < distance[u]){
                distance[u] = d + w;
                q.push({distance[u], u});
            }
        }
    }

    return distance;
}

void dfs(int u, vector<vector<pair<int, unsigned long long>>> &g, vector<int> &p, vector<Node*> &adj, int &sz){
    if(adj[u])
        return;

    if(p[u] != -1){
        dfs(p[u], g, p, adj, sz);
        adj[u] = adj[p[u]]->copy();
    }

    for(auto [v, w] : g[u])
        adj[u] = adj[u]->merge(new Node(sz++, v, w));
}

pair<bool, unsigned long long> eppstein_algorithm(const vector<vector<pair<int, unsigned long long>>> &pre_g_vector, size_t k){
    auto g_vector = pre_g_vector;
    auto n = g_vector.size();
    vector<vector<pair<int, unsigned long long>>> rg(n);

    for(int v = 0; v < n; v++)
        for(auto [u, w] : g_vector[v])
            rg[u].push_back({v, w});

    auto d = default_dijksrta(rg, n - 1);

    if(d[0] == INF64)
        return {false, 0ull};

    // очистим граф от вершин, из которых не достижима n - 1
    {
        map<int, int> mp;
        int sz = 0;

        for(int v = 0; v < n; v++)
            if(d[v] != INF64)
                mp[v] = sz++;

        vector<vector<pair<int, unsigned  long long>>> new_g_vector(sz);

        for(int v = 0; v < n; v++){
            if(d[v] == INF64) continue;

            for(auto [u, w] : g_vector[v]){
                if(d[u] != INF64){
                    new_g_vector[mp[v]].push_back({mp[u], w});
                }
            }
        }

        g_vector = new_g_vector;
        n = sz;
    }

    if(k == 1)
        return {true, d[0]};

    vector<pair<pair<int, int>, unsigned long long>> edges;
    vector<int> p(n, -1);

    // очистим список ребер от nxt[u]
    {
        vector<pair<pair<int, int>, unsigned long long>> edges_with_nxt;

        for(int v = 0; v < n; v++)
            for(auto [u, w] : g_vector[v])
                edges_with_nxt.push_back({{v, u}, d[u] - d[v] + w});

        sort(edges_with_nxt.begin(), edges_with_nxt.end(), [](auto& a, auto& b){
            if(a.second != b.second) return a.second < b.second;
            return a.first < b.first;
        });

        set<int> was;

        for(auto e : edges_with_nxt){
            if(e.first.first == n - 1 || was.count(e.first.first)){
                edges.push_back(e);
            }else{
                was.insert(e.first.first);
                p[e.first.first] = e.first.second;
            }
        }
    }

    vector<vector<pair<int, unsigned long long>>> g(n);

    // edges already sorted by w
    for(auto e : edges){
        auto u = e.first.first;
        auto v = e.first.second;
        auto w = e.second;
        g[u].push_back({v, w});
    }

    vector<Node*> adj(n);
    int sz_adj = 0;

    for(int v = 0; v < n; v++)
        if(!adj[v])
            dfs(v, g, p, adj, sz_adj);

    priority_queue<pair<unsigned long long, Node*>, vector<pair<unsigned long long, Node*>>, cmp> q;

    for(int u = 0; u < n; u++)
        if(adj[u])
            q.push({d[u] + adj[u]->sidetrack, adj[u]});

    while(k-- > 2){
        if(q.empty())
            break;

        unsigned long long l;
        Node *vertex;
        tie(l, vertex) = q.top();
        q.pop();

        if(vertex->l.first){
            q.push({l + vertex->l.first->sidetrack - vertex->sidetrack, vertex->l.first});
        }

        if(vertex->r.first){
            q.push({l + vertex->r.first->sidetrack - vertex->sidetrack, vertex->r.first});
        }

        int v = vertex->v;

        if(adj[v])
            q.push({l + adj[v]->sidetrack, adj[v]});
    }

    return (q.empty() ? make_pair(false, 0ull) : make_pair(true, q.top().first));
}

signed main() {

}