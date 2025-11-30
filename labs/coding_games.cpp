//
// Created by Марк Калюжин on 23.10.2025.
//

#include "coding_games.h"

namespace sokoban_coding_games {
    struct Pos {
        int x{}, y{};
    };
    struct State {
        string s;
        Pos player;
    };

    struct Node {
        int f, g;
        State st;
        string path;

        bool operator<(const Node &o) const { return f > o.f; }
    };

    int idx(int x, int y, int W) { return x * W + y; }

    void find_boxes_and_goals(const string &s, int H, int W, vector<Pos> &boxes, vector<Pos> &goals,
                              vector<Pos> &boxes_on_goal) {
        boxes.clear();
        goals.clear();
        boxes_on_goal.clear();
        for (int x = 0; x < H; ++x)
            for (int y = 0; y < W; ++y) {
                char c = s[idx(x, y, W)];
                if (c == '@') boxes.push_back({x, y});
                else if (c == '$') boxes_on_goal.push_back({x, y});
                else if (c == 'X' || c == '%') goals.push_back({x, y});
            }
    }

    bool is_solved(const string &s) {
        for (char c: s) if (c == '@') return false;
        for (char c: s) if (c == 'X') return false;
        return true;
    }

    int manhattan_sum(const string &s, Pos player, int H, int W) {
        vector<Pos> boxes, goals, boxes_on_goal;
        find_boxes_and_goals(s, H, W, boxes, goals, boxes_on_goal);
        const int BIG = H * W;
        long long boxes_cost = (long long) boxes.size() * BIG;
        for (auto [bx, by]: boxes) {
            int best = INT_MAX;
            for (auto [gx, gy]: goals) best = min(best, abs(bx - gx) + abs(by - gy));
            if (best == INT_MAX) best = BIG;
            boxes_cost += best;
        }
        int player_cost = 0;
        if (!boxes.empty()) {
            int best = INT_MAX;
            for (auto [bx, by]: boxes) best = min(best, abs(bx - player.x) + abs(by - player.y));
            player_cost = best;
        }
        return (int) boxes_cost + player_cost;
    }

    bool is_deadlock(const string &s, int H, int W) {
        auto wall = [&](int x, int y) { return s[idx(x, y, W)] == '+'; };
        vector<Pos> boxes, goals, boxes_on_goal;
        find_boxes_and_goals(s, H, W, boxes, goals, boxes_on_goal);
        for (auto [bx, by]: boxes) {
            if (wall(bx, by - 1) && wall(bx - 1, by)) return true;
            if (wall(bx, by + 1) && wall(bx + 1, by)) return true;
            if (wall(bx, by + 1) && wall(bx - 1, by)) return true;
            if (wall(bx, by - 1) && wall(bx + 1, by)) return true;
        }
        for (auto [bx, by]: boxes) {
            bool left = wall(bx, by - 1);
            bool right = wall(bx, by + 1);
            bool up = wall(bx - 1, by);
            bool down = wall(bx + 1, by);
            if ((left || right) && (up || down)) return true;
        }
        return false;
    }

    pair<string, int> can_move(const string &s, int H, int W, Pos player, pair<int, int> mv) {
        auto inside = [&](int x, int y) { return 0 <= x && x < H && 0 <= y && y < W; };
        int x = player.x, y = player.y;
        int nx = x + mv.first, ny = y + mv.second;
        int bx = x + 2 * mv.first, by = y + 2 * mv.second;
        if (!inside(nx, ny)) return {"", 0};
        if (!inside(bx, by)) {}
        string ns = s;
        int cur1 = idx(x, y, W);
        int t1 = idx(nx, ny, W);
        int bt1 = idx(bx, by, W);
        int move_cost = 0;
        auto setPlayerAt = [&](int pos) {
            if (ns[pos] == '-') ns[pos] = '*';
            else if (ns[pos] == 'X') ns[pos] = '%';
        };
        auto leavePlayerFrom = [&](int pos) {
            if (ns[pos] == '*') ns[pos] = '-';
            else if (ns[pos] == '%') ns[pos] = 'X';
        };
        if (s[t1] == '+') return {"", 0};
        if (s[t1] == '-' || s[t1] == 'X') {
            leavePlayerFrom(cur1);
            setPlayerAt(t1);
            move_cost = 3;
            return {ns, move_cost};
        }
        if (s[t1] == '@' || s[t1] == '$') {
            if (s[bt1] == '+' || s[bt1] == '@' || s[bt1] == '$') return {"", 0};
            if (ns[bt1] == '-') ns[bt1] = '@';
            else if (ns[bt1] == 'X') ns[bt1] = '$';
            else return {"", 0};
            if (ns[t1] == '@') ns[t1] = '*';
            else if (ns[t1] == '$') ns[t1] = '%';
            leavePlayerFrom(cur1);
            move_cost = (ns[bt1] == '$') ? 0 : 2;
            return {ns, move_cost};
        }
        return {"", 0};
    }

    string solve_astar(const State &start, int H, int W) {
        priority_queue<Node> pq;
        unordered_set<string> seen;
        seen.reserve(1 << 20);
        auto h0 = manhattan_sum(start.s, start.player, H, W);
        pq.push(Node{h0, 0, start, ""});
        const pair<int, int> moves[4] = {{-1, 0},
                                         {1,  0},
                                         {0,  -1},
                                         {0,  1}};
        const char dirc[4] = {'U', 'D', 'L', 'R'};
        while (!pq.empty()) {
            Node cur = pq.top();
            pq.pop();
            if (!seen.insert(cur.st.s).second) continue;
            if (is_solved(cur.st.s)) return cur.path;
            for (int i = 0; i < 4; ++i) {
                auto [dx, dy] = moves[i];
                auto [ns, step_cost] = can_move(cur.st.s, H, W, cur.st.player, {dx, dy});
                if (ns.empty()) continue;
                Pos np = {cur.st.player.x + dx, cur.st.player.y + dy};
                if (seen.count(ns)) continue;
                if (is_deadlock(ns, H, W)) continue;
                int g2 = cur.g + step_cost;
                int h2 = manhattan_sum(ns, np, H, W);
                pq.push(Node{g2 + h2, g2, State{ns, np}, cur.path + dirc[i]});
            }
        }
        return "";
    }

    int run() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        int W, H, B;
        if (!(cin >> W >> H >> B)) return 0;
        vector<string> cg(H);
        for (int i = 0; i < H; ++i) cin >> cg[i];
        string base(H * W, '-');
        auto put = [&](int x, int y, char c) { base[idx(x, y, W)] = c; };
        for (int x = 0; x < H; ++x)
            for (int y = 0; y < W; ++y)
                if (cg[x][y] == '#') put(x, y, '+');
                else if (cg[x][y] == '*') put(x, y, 'X');
                else put(x, y, '-');
        string cachedPath;
        size_t stepIndex = 0;
        while (true) {
            int px, py;
            if (!(cin >> px >> py)) break;
            vector<Pos> boxes(B);
            for (int i = 0; i < B; ++i) {
                int bx, by;
                if (!(cin >> bx >> by)) return 0;
                boxes[i] = {bx, by};
            }
            string s = base;
            auto at = [&](int x, int y) { return idx(y, x, W); };
            int p1 = at(px, py);
            if (s[p1] == 'X') s[p1] = '%'; else s[p1] = '*';
            for (auto [bx, by]: boxes) {
                int id = at(bx, by);
                if (s[id] == 'X') s[id] = '$';
                else if (s[id] == '%') s[id] = '$';
                else s[id] = '@';
            }
            if (cachedPath.empty()) {
                State start{s, {py, px}};
                cachedPath = solve_astar(start, H, W);
                stepIndex = 0;
            }
            if (!isatty(fileno(stdin))) {
                char out = 'U';
                if (!cachedPath.empty() && stepIndex < cachedPath.size())
                    out = cachedPath[stepIndex++];
                cout << out << '\n' << flush;
            } else {
                if (cachedPath.empty()) {
                    State start{s, {py, px}};
                    cachedPath = solve_astar(start, H, W);
                }
                cout << cachedPath << '\n';
                return 0;
            }
        }
        return 0;
    }
}
