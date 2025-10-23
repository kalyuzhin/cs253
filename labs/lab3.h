//
// Created by Марк Калюжин on 22.10.2025.
//

#ifndef COMPUTINGSYSTEMS_LAB3_H
#define COMPUTINGSYSTEMS_LAB3_H

#include "../provider.h"

namespace sokoban {

    struct Pos {
        int x{}, y{};
    };

    int idx(int x, int y, int W);

    struct State {
        std::string s;
        Pos player;
    };

    struct Node {
        int f, g;
        State st;
        std::string path;
        bool operator<(const Node &o) const;
    };

    void find_boxes_and_goals(const std::string &s, int H, int W,
                              std::vector<Pos> &boxes,
                              std::vector<Pos> &goals,
                              std::vector<Pos> &boxes_on_goal);

    bool is_solved(const std::string &s);
    int manhattan_sum(const std::string &s, Pos player, int H, int W);
    bool is_deadlock(const std::string &s, int H, int W);
    std::pair<std::string, int> can_move(const std::string &s, int H, int W, Pos player, std::pair<int, int> mv);
    std::string solve_astar(const State &start, int H, int W);
    int run();

}

#endif // COMPUTINGSYSTEMS_LAB3_H
