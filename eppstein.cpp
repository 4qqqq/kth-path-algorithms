#include <bits/stdc++.h>

using namespace std;

const unsigned long long INF64 = 6e18;

struct Node{
    int dist = 0; // нужно для поддержания работы левосторонней кучи

    int& dist_method(Node* v){ return v ? v->dist : *(new int(0)); }

    Node* merge(Node* xx, Node* y){
        if(xx == 0) return y;
        if(y == 0) return xx;

        Node* x = copy(xx);

        if(y->sidetrack < x->sidetrack)
            swap(x, y);

        x->r.first = merge(x->r.first, y);
        x->r.second = (x->r.first ? x->r.first->sidetrack - x->sidetrack : 0);

        if(dist_method(x->r.first) > dist_method(x->l.first))
            swap(x->l, x->r);

        dist_method(x) = dist_method(x->r.first) + 1;

        return x;
    }

    pair<Node*, unsigned long long> l = {(Node*)(nullptr), 0ull},
                                    r = {(Node*)(nullptr), 0ull};

    int id = 0, // id вершины
        v = 0; // существует ребро (x, v, w) [x \in {u, p[u], p[p[u]], ..., t}]

    unsigned long long sidetrack = 0; // key

    Node(){}
    Node(int id, int v, unsigned long long sidetrack) : id(id), v(v), sidetrack(sidetrack) {}

    Node* copy(Node *a){
        if(!a) return 0;

        Node* res = new Node();
        res->id = a->id;
        res->v = a->v;
        res->sidetrack = a->sidetrack;
        res->l = a->l;
        res->r = a->r;
        res->dist = a->dist;
        return res;
    }
};

class cmp{
public:
    bool operator() (const Node* a, const Node* b) const {
        if(a->sidetrack != b->sidetrack) return a->sidetrack < b->sidetrack;
        return a->id > b->id;
    }

    bool operator() (const pair<unsigned long long, Node*> &a, const pair<unsigned long long, Node*> &b) const {
        if(a.first != b.first) return a.first > b.first;
        return a.second->id > b.second->id;
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

void dfs(int u, vector<vector<pair<int, unsigned long long>>> &g, vector<int> &p, vector<Node*> &adj, int &sz, vector<char> &was){
    if(was[u])
        return;

    was[u] = true;

    if(p[u] != -1){
        dfs(p[u], g, p, adj, sz, was);            
        adj[u] = Node().copy(adj[p[u]]);
    }

    for(auto [v, w] : g[u])
        adj[u] = Node().merge(adj[u], new Node(sz++, v, w));
}

vector<unsigned long long> eppstein_algorithm(const vector<vector<pair<int, unsigned long long>>> &pre_g_vector, size_t k){
    auto g_vector = pre_g_vector;
    auto n = g_vector.size();
    vector<vector<pair<int, unsigned long long>>> rg(n);

    for(int v = 0; v < n; v++)
        for(auto [u, w] : g_vector[v])
            rg[u].push_back({v, w});

    auto d = default_dijksrta(rg, n - 1);

    if(d[0] == INF64)
        return {};

    // очистим граф от вершин, из которых не достижима n - 1
    {
        map<int, int> mp;
        int sz = 0;
        vector<unsigned long long> new_d = {};

        for(int v = 0; v < n; v++)
            if(d[v] != INF64){
                mp[v] = sz++;
                new_d.push_back(d[v]);
            }

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
        d = new_d;
    }

    if(k == 1)
        return {d[0]};

    vector<pair<pair<int, int>, unsigned long long>> edges;
    vector<int> p(n, -1);

    // очистим список ребер от nxt[u]
    {
        vector<pair<pair<int, int>, pair<unsigned long long, unsigned long long>>> edges_with_nxt;

        for(int v = 0; v < n; v++)
            for(auto [u, w] : g_vector[v])
                edges_with_nxt.push_back({{v, u}, {d[u] - d[v] + w, w}});

        sort(edges_with_nxt.begin(), edges_with_nxt.end(), [](auto& a, auto& b){
            if(a.second != b.second){
                if(a.second.first != b.second.first)
                    return a.second.first < b.second.first;

                return a.second.second > b.second.second;
            }

            return a.first < b.first;
        });

        set<int> was;

        for(auto e : edges_with_nxt){
            if(e.first.first == n - 1 || was.count(e.first.first)){
                edges.push_back({e.first, e.second.first});
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
    vector<char> was(n);

    for(int v = 0; v < n; v++)
        if(!was[v])
            dfs(v, g, p, adj, sz_adj, was);

    priority_queue<pair<unsigned long long, Node*>, vector<pair<unsigned long long, Node*>>, cmp> q;
    vector<unsigned long long> ans = {d[0]};

    if(adj[0])
        q.push({d[0] + adj[0]->sidetrack, adj[0]});

    while(k-- > 2){
        if(q.empty())
            break;

        unsigned long long l;
        Node *vertex;
        tie(l, vertex) = q.top();
        ans.push_back(l);
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

    if(!q.empty()){
        ans.push_back(q.top().first);
    }

    return ans;
}

signed main(){

}