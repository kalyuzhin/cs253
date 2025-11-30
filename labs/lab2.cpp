#include "lab2.h"

static constexpr int SZ = 4;
static constexpr int N = SZ * SZ;
static constexpr ull GOAL = 0x123456789ABCDEF0ULL;


static const int DX[4] = {-1, 1, 0, 0};
static const int DY[4] = {0, 0, -1, 1};
static const char MV[4] = {'U', 'D', 'L', 'R'};

static inline int invMove(int m) { return m ^ 1; }

static inline int get_tile(ull s, int idx) {
    return (int) ((s >> (4 * (15 - idx))) & 0xFULL);
}

static inline ull set_tile(ull s, int idx, int val) {
    ull mask = ~(0xFULL << (4 * (15 - idx)));
    s &= mask;
    s |= (ull) val << (4 * (15 - idx));
    return s;
}

static inline int find_blank(ull s) {
    for (int i = 0; i < N; ++i) if (get_tile(s, i) == 0) return i;
    return -1;
}

static void print_state(ostream &os, ull s) {
    for (int i = 0; i < N; ++i) {
        int t = get_tile(s, i);
        if (t == 0) os << ". "; else os << uppercase << hex << t << ' ' << dec;
        if ((i + 1) % SZ == 0) os << '\n';
    }
}

static bool parse_hex_state(const string &hex, ull &out) {
    if (hex.size() != 16) return false;
    ull s = 0;
    array<int, 16> seen{};
    seen.fill(0);
    for (char c: hex) {
        int v;
        if (c >= '0' && c <= '9') v = c - '0';
        else if (c >= 'a' && c <= 'f') v = 10 + (c - 'a');
        else if (c >= 'A' && c <= 'F') v = 10 + (c - 'A');
        else return false;
        if (v < 0 || v > 15) return false;
        seen[v]++;
        s = (s << 4) | (ull) v;
    }
    for (int i = 0; i < 16; ++i) if (seen[i] != 1) return false;
    out = s;
    return true;
}

static bool solvable(ull s) {
    vec<int> a;
    a.reserve(16);
    int blankRowTop = 0;
    for (int i = 0; i < N; ++i) {
        int t = get_tile(s, i);
        if (t == 0) blankRowTop = i / SZ + 1; else a.push_back(t);
    }
    int inv = 0;
    for (int i = 0; i < (int) a.size(); ++i) for (int j = 0; j < i; ++j) if (a[j] > a[i]) ++inv;
    return ((inv + blankRowTop) % 2) == 0;
}

static inline vec<pair<ull, int>> neighbors(ull s) {
    int pos = find_blank(s);
    int x = pos / SZ, y = pos % SZ;
    vec<pair<ull, int>> res;
    res.reserve(4);
    for (int d = 0; d < 4; ++d) {
        int nx = x + DX[d], ny = y + DY[d];
        if (nx < 0 || ny < 0 || nx >= SZ || ny >= SZ) continue;
        int npos = nx * SZ + ny;
        int t = get_tile(s, npos);
        ull ns = set_tile(s, pos, t);
        ns = set_tile(ns, npos, 0);
        res.emplace_back(ns, d);
    }
    return res;
}

static inline int manhattan(ull s) {
    int sum = 0;
    for (int i = 0; i < N; ++i) {
        int v = get_tile(s, i);
        if (v == 0) continue;
        int goal = v - 1;
        int r = i / SZ, c = i % SZ;
        int gr = goal / SZ, gc = goal % SZ;
        sum += abs(r - gr) + abs(c - gc);
    }
    return sum;
}

static inline int linear_conflict(ull s) {
    int add = 0;
    for (int r = 0; r < SZ; ++r) {
        int base = r * SZ;
        vec<int> seq;
        seq.reserve(SZ);
        for (int c = 0; c < SZ; ++c) {
            int v = get_tile(s, base + c);
            if (v != 0 && (v - 1) / SZ == r) seq.push_back(v);
        }
        for (size_t i = 0; i < seq.size(); ++i) {
            int gi = (seq[i] - 1) % SZ;
            for (size_t j = i + 1; j < seq.size(); ++j) {
                int gj = (seq[j] - 1) % SZ;
                if (gi > gj) add += 2;
            }
        }
    }
    for (int c = 0; c < SZ; ++c) {
        vec<int> seq;
        seq.reserve(SZ);
        for (int r = 0; r < SZ; ++r) {
            int v = get_tile(s, r * SZ + c);
            if (v != 0 && (v - 1) % SZ == c) seq.push_back(v);
        }
        for (size_t i = 0; i < seq.size(); ++i) {
            int gi = (seq[i] - 1) / SZ;
            for (size_t j = i + 1; j < seq.size(); ++j) {
                int gj = (seq[j] - 1) / SZ;
                if (gi > gj) add += 2;
            }
        }
    }
    return add;
}

static inline int heuristic(ull s) { return manhattan(s) + linear_conflict(s); }

static void emit_solution(ostream &os, ull start, const vec<int> &moves) {
    os << "Moves (" << moves.size() << "): ";
    for (int m: moves) os << MV[m];
    os << "\n\n";
    ull cur = start;
    print_state(os, cur);
    os << "\n";
    for (int m: moves) {
        int pos = find_blank(cur);
        int x = pos / SZ, y = pos % SZ;
        int nx = x + DX[m], ny = y + DY[m];
        int npos = nx * SZ + ny;
        int t = get_tile(cur, npos);
        cur = set_tile(cur, pos, t);
        cur = set_tile(cur, npos, 0);
        print_state(os, cur);
        os << "\n";
    }
}

struct Metrics {
    long long expanded = 0;
    double seconds = 0.0;
};

static pair<vec<int>, Metrics> solve_bfs(ull start) {
    Metrics met;
    auto t0 = chrono::steady_clock::now();
    queue<ull> q;
    unordered_map<ull, pair<ull, int>> parent;
    parent.reserve(1 << 20);
    unordered_set<ull> vis;
    vis.reserve(1 << 20);
    vis.insert(start);
    q.push(start);
    while (!q.empty()) {
        ull cur = q.front();
        q.pop();
        ++met.expanded;
        if (cur == GOAL) break;
        for (auto [ns, dir]: neighbors(cur))
            if (!vis.count(ns)) {
                vis.insert(ns);
                parent.emplace(ns, make_pair(cur, dir));
                q.push(ns);
            }
    }
    auto t1 = chrono::steady_clock::now();
    met.seconds = chrono::duration<double>(t1 - t0).count();
    vec<int> path;
    if (!parent.count(GOAL)) { return {path, met}; }
    ull cur = GOAL;
    while (cur != start) {
        auto it = parent.find(cur);
        auto pr = it->second;
        path.push_back(pr.second);
        cur = pr.first;
    }
    reverse(path.begin(), path.end());
    return {path, met};
}

static bool dfs_limit(ull cur, int limit, int lastMove, vec<int> &path, Metrics &met, unordered_set<ull> &onpath) {
    ++met.expanded;
    if (cur == GOAL) return true;
    if (limit == 0) return false;
    onpath.insert(cur);
    for (auto [ns, dir]: neighbors(cur)) {
        if (lastMove != -1 && dir == invMove(lastMove)) continue;
        if (onpath.count(ns)) continue;
        path.push_back(dir);
        if (dfs_limit(ns, limit - 1, dir, path, met, onpath)) return true;
        path.pop_back();
    }
    onpath.erase(cur);
    return false;
}

static pair<vec<int>, Metrics> solve_dfs(ull start, int maxDepth) {
    Metrics met;
    vec<int> path;
    unordered_set<ull> onpath;
    auto t0 = chrono::steady_clock::now();
    dfs_limit(start, maxDepth, -1, path, met, onpath);
    auto t1 = chrono::steady_clock::now();
    met.seconds = chrono::duration<double>(t1 - t0).count();
    return {path, met};
}

static pair<vec<int>, Metrics> solve_ids(ull start, int maxDepth = 80) {
    Metrics met;
    auto t0 = chrono::steady_clock::now();
    for (int depth = 0; depth <= maxDepth; ++depth) {
        vec<int> path;
        unordered_set<ull> onpath;
        onpath.reserve(1 << 18);
        if (dfs_limit(start, depth, -1, path, met, onpath)) {
            auto t1 = chrono::steady_clock::now();
            met.seconds = chrono::duration<double>(t1 - t0).count();
            return {path, met};
        }
    }
    auto t1 = chrono::steady_clock::now();
    met.seconds = chrono::duration<double>(t1 - t0).count();
    return {vec<int>{}, met};
}

struct ANode {
    ull s;
    int g, f;
    int move;
    ull parent;
};

struct CmpAN {
    bool operator()(const ANode &a, const ANode &b) const {
        if (a.f != b.f) return a.f > b.f;
        if (a.g != b.g) return a.g < b.g;
        return a.s > b.s;
    }
};

static pair<vec<int>, Metrics> solve_astar(ull start) {
    Metrics met;
    auto t0 = chrono::steady_clock::now();
    priority_queue<ANode, vec<ANode>, CmpAN> pq;
    unordered_map<ull, int> bestG;
    bestG.reserve(1 << 21);
    unordered_map<ull, pair<ull, int>> parent;
    parent.reserve(1 << 21);
    pq.push({start, 0, heuristic(start), -1, 0});
    bestG[start] = 0;
    while (!pq.empty()) {
        ANode cur = pq.top();
        pq.pop();
        ++met.expanded;
        if (cur.s == GOAL) break;
        if (cur.g != bestG[cur.s]) continue;
        for (auto [ns, dir]: neighbors(cur.s)) {
            if (cur.move != -1 && dir == invMove(cur.move)) continue;
            int ng = cur.g + 1;
            auto it = bestG.find(ns);
            if (it != bestG.end() && ng >= it->second) continue;
            if (it == bestG.end() || ng < it->second) {
                bestG[ns] = ng;
                parent[ns] = {cur.s, dir};
                int nf = ng + heuristic(ns);
                pq.push({ns, ng, nf, dir, cur.s});
            }
        }
    }
    auto t1 = chrono::steady_clock::now();
    met.seconds = chrono::duration<double>(t1 - t0).count();
    vec<int> path;
    if (!parent.count(GOAL)) return {path, met};
    ull cur = GOAL;
    while (cur != start) {
        auto pr = parent[cur];
        path.push_back(pr.second);
        cur = pr.first;
    }
    reverse(path.begin(), path.end());
    return {path, met};
}

struct IDAEnv {
    ull start;
    Metrics met;
    vec<int> path;
};

static int ida_dfs(ull s, int g, int bound, int lastMove, IDAEnv &env) {
    int h = heuristic(s);
    int f = g + h;
    if (f > bound) return f;
    if (s == GOAL) return -1;
    ++env.met.expanded;
    int minNext = INT_MAX;
    for (auto [ns, dir]: neighbors(s)) {
        if (lastMove != -1 && dir == invMove(lastMove)) continue;
        env.path.push_back(dir);
        int t = ida_dfs(ns, g + 1, bound, dir, env);
        if (t == -1) return -1;
        if (t < minNext) minNext = t;
        env.path.pop_back();
    }
    return minNext;
}

static pair<vec<int>, Metrics> solve_ida(ull start) {
    IDAEnv env{start, {}, {}};
    auto t0 = chrono::steady_clock::now();
    int bound = heuristic(start);
    for (;;) {
        int t = ida_dfs(start, 0, bound, -1, env);
        if (t == -1) break;
        if (t == INT_MAX) {
            env.path.clear();
            break;
        }
        bound = t;
    }
    auto t1 = chrono::steady_clock::now();
    env.met.seconds = chrono::duration<double>(t1 - t0).count();
    return {env.path, env.met};
}

enum class Algo {
    BFS, DFS, IDS, ASTAR, IDA
};
struct Options {
    Algo algo = Algo::ASTAR;
    string hex;
    bool toFile = false;
    string outFile;
    int dfsDepth = 80;
};

static void usage() {
    cerr << "Usage: fifteen --algo bfs|dfs|ids|astar|ida --state HEX16 [--dfs-depth N] [--out file:NAME|console]\n";
}

static bool parse_args(int argc, char **argv, Options &opt) {
    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == "--algo" && i + 1 < argc) {
            string v = argv[++i];
            if (v == "bfs") opt.algo = Algo::BFS;
            else if (v == "dfs") opt.algo = Algo::DFS;
            else if (v == "ids")
                opt.algo = Algo::IDS;
            else if (v == "astar") opt.algo = Algo::ASTAR; else if (v == "ida") opt.algo = Algo::IDA; else return false;
        } else if (a == "--state" && i + 1 < argc) { opt.hex = argv[++i]; }
        else if (a == "--dfs-depth" && i + 1 < argc) { opt.dfsDepth = stoi(argv[++i]); }
        else if (a == "--out" && i + 1 < argc) {
            string v = argv[++i];
            if (v.rfind("file:", 0) == 0) {
                opt.toFile = true;
                opt.outFile = v.substr(5);
            } else opt.toFile = false;
        } else return false;
    }
    return !opt.hex.empty();
}

int run(int argc, char **argv) {
    Options opt;
    if (!parse_args(argc, argv, opt)) {
        string hex;
        cout << "Введите начальное состояние (16 шестнадцатеричных символов, 0 — пусто)\n> ";
        if (!(cin >> hex)) return 0;
        opt.hex = hex;
        cout << "Алгоритм [bfs/dfs/ids/astar/ida] > ";
        string alg;
        cin >> alg;
        if (alg == "bfs") opt.algo = Algo::BFS;
        else if (alg == "dfs") opt.algo = Algo::DFS;
        else if (alg == "ids")
            opt.algo = Algo::IDS;
        else if (alg == "ida") opt.algo = Algo::IDA; else opt.algo = Algo::ASTAR;
        cout << "Вывод [console/file:<name>] > ";
        string out;
        if (cin >> out) {
            if (out.rfind("file:", 0) == 0) {
                opt.toFile = true;
                opt.outFile = out.substr(5);
            }
        }
    }

    ull start;
    if (!parse_hex_state(opt.hex, start)) {
        cerr << "Некорректная hex-строка.\n";
        return 1;
    }
    if (!solvable(start)) {
        cout << "Позиция нерешаема — выход.\n";
        return 0;
    }

    vec<int> path;
    Metrics met;
    string title;
    if (opt.algo == Algo::BFS) {
        tie(path, met) = solve_bfs(start);
        title = "BFS";
    } else if (opt.algo == Algo::DFS) {
        tie(path, met) = solve_dfs(start, opt.dfsDepth);
        title = "DFS (depth-limited)";
    } else if (opt.algo == Algo::IDS) {
        tie(path, met) = solve_ids(start);
        title = "IDS";
    } else if (opt.algo == Algo::ASTAR) {
        tie(path, met) = solve_astar(start);
        title = "A* (Manhattan+LC)";
    } else {
        tie(path, met) = solve_ida(start);
        title = "IDA* (Manhattan+LC)";
    }

    ostream *osp = &cout;
    ofstream fout;
    if (opt.toFile) {
        fout.open(opt.outFile);
        if (!fout) {
            cerr << "Не могу открыть файл для записи: " << opt.outFile << "\n";
            return 1;
        }
        osp = &fout;
    }

    *osp << title << " — результат" << "\n";
    *osp << "Состояний расширено: " << met.expanded << "\nВремя: " << fixed << setprecision(6) << met.seconds
         << " сек\n\n";

    if (path.empty() && start != GOAL) { *osp << "Решение не найдено (в заданных ограничениях).\n"; }
    else { emit_solution(*osp, start, path); }

    if (opt.toFile) { cout << "Готово. Решение записано в файл '" << opt.outFile << "'.\n"; }
    return 0;
}
