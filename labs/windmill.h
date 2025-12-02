#ifndef COMPUTINGSYSTEMS_WINDMILL_H
#define COMPUTINGSYSTEMS_WINDMILL_H

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

namespace windmill {
    struct Move {
        int from;
        int to;
        int remove1;
        int remove2;
    };

    struct MoveRecord {
        Move move;
        int player;
        int prevPiecesInHand[2];
        int prevPieceCount[2];
        int newMills[2];
        int newMillsCount;
    };

    struct Board {
        static constexpr int EMPTY = -1;

        static const array<string, 24> indexToCoord;
        static const array<array<int, 3>, 16> mills;
        static const array<vector<int>, 24> neighbours;

        int cells[24];
        int piecesInHand[2];
        int pieceCount[2];
        bool millUsed[2][16];
        vector<MoveRecord> history;

        Board() {
            reset();
        }

        void reset() {
            for (int i = 0; i < 24; ++i) cells[i] = EMPTY;
            piecesInHand[0] = piecesInHand[1] = 9;
            pieceCount[0] = pieceCount[1] = 0;
            for (int p = 0; p < 2; ++p)
                for (int m = 0; m < 16; ++m)
                    millUsed[p][m] = false;
            history.clear();
        }

        static int coordToIndex(const string &coord) {
            if (coord.size() != 2) return -1;
            char file = static_cast<char>(tolower(coord[0]));
            char rank = coord[1];
            if (file < 'a' || file > 'g' || rank < '1' || rank > '7') return -1;
            for (int i = 0; i < 24; ++i) {
                if (indexToCoord[i][0] == file && indexToCoord[i][1] == rank) return i;
            }
            return -1;
        }

        static string moveToString(const Move &m) {
            string s;
            if (m.from == -1) {
                s += indexToCoord[m.to];
            } else {
                s += indexToCoord[m.from];
                s.push_back(' ');
                s += indexToCoord[m.to];
            }
            if (m.remove1 != -1) {
                s.push_back(' ');
                s += indexToCoord[m.remove1];
            }
            if (m.remove2 != -1) {
                s.push_back(' ');
                s += indexToCoord[m.remove2];
            }
            return s;
        }

        bool isInMill(int pos, int player) const {
            for (const auto &m: mills) {
                if (m[0] == pos || m[1] == pos || m[2] == pos) {
                    if (cells[m[0]] == player && cells[m[1]] == player && cells[m[2]] == player)
                        return true;
                }
            }
            return false;
        }

        int countMillsPlayer(int player) const {
            int cnt = 0;
            for (const auto &m: mills) {
                if (cells[m[0]] == player && cells[m[1]] == player && cells[m[2]] == player)
                    ++cnt;
            }
            return cnt;
        }

        int countFreshMillsPlayer(int player) const {
            int cnt = 0;
            for (int mi = 0; mi < (int) mills.size(); ++mi) {
                if (millUsed[player][mi]) continue;
                const auto &m = mills[mi];
                if (cells[m[0]] == player && cells[m[1]] == player && cells[m[2]] == player)
                    ++cnt;
            }
            return cnt;
        }

        vector<int> newMillsForMove(int pos, int player, int fromPos) const {
            vector<int> res;
            for (int mi = 0; mi < (int) mills.size(); ++mi) {
                if (millUsed[player][mi]) continue;
                const auto &t = mills[mi];
                bool contains = false;
                bool full = true;
                for (int idx: t) {
                    if (idx == pos) {
                        contains = true;
                    } else if (idx == fromPos) {
                        full = false;
                        break;
                    } else {
                        if (cells[idx] != player) {
                            full = false;
                            break;
                        }
                    }
                }
                if (contains && full) {
                    res.push_back(mi);
                }
            }
            return res;
        }

        vector<int> captureCandidates(int player) const {
            int opp = 1 - player;
            vector<int> allOpp;
            vector<int> nonMill;
            for (int i = 0; i < 24; ++i) {
                if (cells[i] == opp) {
                    allOpp.push_back(i);
                    if (!isInMill(i, opp)) nonMill.push_back(i);
                }
            }
            if (!nonMill.empty()) return nonMill;
            return allOpp;
        }

        vector<Move> generateMoves(int player) const {
            vector<Move> moves;
            vector<int> caps = captureCandidates(player);

            if (piecesInHand[player] > 0) {
                for (int pos = 0; pos < 24; ++pos) {
                    if (cells[pos] != EMPTY) continue;
                    auto newMs = newMillsForMove(pos, player, -1);
                    int maxCaptures = min<int>((int) newMs.size(), 2);
                    maxCaptures = min(maxCaptures, (int) caps.size());
                    if (maxCaptures <= 0) {
                        moves.push_back({-1, pos, -1, -1});
                    } else if (maxCaptures == 1) {
                        for (int c: caps) {
                            moves.push_back({-1, pos, c, -1});
                        }
                    } else {
                        for (size_t i = 0; i < caps.size(); ++i) {
                            for (size_t j = i + 1; j < caps.size(); ++j) {
                                moves.push_back({-1, pos, caps[i], caps[j]});
                            }
                        }
                    }
                }
            } else {
                bool canFly = (pieceCount[player] == 3);
                if (canFly) {
                    for (int from = 0; from < 24; ++from) {
                        if (cells[from] != player) continue;
                        for (int to = 0; to < 24; ++to) {
                            if (cells[to] != EMPTY) continue;
                            auto newMs = newMillsForMove(to, player, from);
                            int maxCaptures = min<int>((int) newMs.size(), 2);
                            maxCaptures = min(maxCaptures, (int) caps.size());
                            if (maxCaptures <= 0) {
                                moves.push_back({from, to, -1, -1});
                            } else if (maxCaptures == 1) {
                                for (int c: caps) moves.push_back({from, to, c, -1});
                            } else {
                                for (size_t i = 0; i < caps.size(); ++i)
                                    for (size_t j = i + 1; j < caps.size(); ++j)
                                        moves.push_back({from, to, caps[i], caps[j]});
                            }
                        }
                    }
                } else {
                    for (int from = 0; from < 24; ++from) {
                        if (cells[from] != player) continue;
                        for (int to: neighbours[from]) {
                            if (cells[to] != EMPTY) continue;
                            auto newMs = newMillsForMove(to, player, from);
                            int maxCaptures = min<int>((int) newMs.size(), 2);
                            maxCaptures = min(maxCaptures, (int) caps.size());
                            if (maxCaptures <= 0) {
                                moves.push_back({from, to, -1, -1});
                            } else if (maxCaptures == 1) {
                                for (int c: caps) moves.push_back({from, to, c, -1});
                            } else {
                                for (size_t i = 0; i < caps.size(); ++i)
                                    for (size_t j = i + 1; j < caps.size(); ++j)
                                        moves.push_back({from, to, caps[i], caps[j]});
                            }
                        }
                    }
                }
            }
            return moves;
        }

        void applyMove(const Move &m, int player) {
            MoveRecord rec;
            rec.move = m;
            rec.player = player;
            rec.prevPiecesInHand[0] = piecesInHand[0];
            rec.prevPiecesInHand[1] = piecesInHand[1];
            rec.prevPieceCount[0] = pieceCount[0];
            rec.prevPieceCount[1] = pieceCount[1];
            rec.newMillsCount = 0;

            if (m.from == -1) {
                piecesInHand[player]--;
                cells[m.to] = player;
                pieceCount[player]++;
            } else {
                cells[m.from] = EMPTY;
                cells[m.to] = player;
            }

            auto newMs = newMillsForMove(m.to, player, m.from);
            rec.newMillsCount = (int) newMs.size();
            if (rec.newMillsCount > 2) rec.newMillsCount = 2;
            for (int i = 0; i < rec.newMillsCount; ++i) {
                rec.newMills[i] = newMs[i];
                millUsed[player][newMs[i]] = true;
            }

            int opp = 1 - player;
            if (m.remove1 != -1) {
                cells[m.remove1] = EMPTY;
                pieceCount[opp]--;
            }
            if (m.remove2 != -1) {
                cells[m.remove2] = EMPTY;
                pieceCount[opp]--;
            }

            history.push_back(rec);
        }

        void undoLastMove() {
            if (history.empty()) return;
            MoveRecord rec = history.back();
            history.pop_back();

            int player = rec.player;
            int opp = 1 - player;
            Move m = rec.move;

            for (int i = 0; i < rec.newMillsCount; ++i) {
                int idx = rec.newMills[i];
                millUsed[player][idx] = false;
            }

            if (m.remove1 != -1) {
                cells[m.remove1] = opp;
            }
            if (m.remove2 != -1) {
                cells[m.remove2] = opp;
            }

            if (m.from == -1) {
                cells[m.to] = EMPTY;
            } else {
                cells[m.to] = EMPTY;
                cells[m.from] = player;
            }

            piecesInHand[0] = rec.prevPiecesInHand[0];
            piecesInHand[1] = rec.prevPiecesInHand[1];
            pieceCount[0] = rec.prevPieceCount[0];
            pieceCount[1] = rec.prevPieceCount[1];
        }

        bool hasDoubleMill(int player) const {
            for (int pos = 0; pos < 24; ++pos) {
                if (cells[pos] != player) continue;
                int millsThrough = 0;
                for (int mi = 0; mi < (int) mills.size(); ++mi) {
                    const auto &t = mills[mi];
                    if ((t[0] == pos || t[1] == pos || t[2] == pos) &&
                        cells[t[0]] == player && cells[t[1]] == player && cells[t[2]] == player) {
                        ++millsThrough;
                        if (millsThrough >= 2) return true;
                    }
                }
            }
            return false;
        }

        int countPotentialMills(int player) const {
            int cnt = 0;
            for (const auto &t: mills) {
                int p = 0, e = 0, o = 0;
                for (int idx: t) {
                    if (cells[idx] == player) ++p;
                    else if (cells[idx] == EMPTY) ++e;
                    else ++o;
                }
                if (p == 2 && e == 1) ++cnt;
            }
            return cnt;
        }

        int countBlockedPieces(int player) const {
            if (piecesInHand[player] > 0) return 0;
            if (pieceCount[player] == 3) return 0;
            int blocked = 0;
            for (int pos = 0; pos < 24; ++pos) {
                if (cells[pos] != player) continue;
                bool hasMove = false;
                for (int to: neighbours[pos]) {
                    if (cells[to] == EMPTY) {
                        hasMove = true;
                        break;
                    }
                }
                if (!hasMove) ++blocked;
            }
            return blocked;
        }

        void printBoard(ostream &os) const {
            for (int r = 7; r >= 1; --r) {
                os << r << ' ';
                for (int c = 0; c < 7; ++c) {
                    char file = static_cast<char>('a' + c);
                    string coord;
                    coord.push_back(file);
                    coord.push_back(char('0' + r));
                    int idx = coordToIndex(coord);
                    char ch = '.';
                    if (idx != -1) {
                        if (cells[idx] == 0) ch = 'X';
                        else if (cells[idx] == 1) ch = 'O';
                        else ch = '.';
                    } else {
                        ch = ' ';
                    }
                    os << ch << ' ';
                }
                os << '\n';
            }
            os << "  a b c d e f g\n";
        }

        int evaluate(int botPlayer) const {
            int opp = 1 - botPlayer;
            const int WIN = 1000000;

            if (pieceCount[opp] <= 2 && piecesInHand[opp] == 0) return WIN;
            if (pieceCount[botPlayer] <= 2 && piecesInHand[botPlayer] == 0) return -WIN;

            auto myMoves = generateMoves(botPlayer);
            auto oppMoves = generateMoves(opp);

            if (piecesInHand[botPlayer] == 0 && myMoves.empty()) return -WIN;
            if (piecesInHand[opp] == 0 && oppMoves.empty()) return WIN;

            int totalPieces = pieceCount[botPlayer] + pieceCount[opp];

            int millsBotAll = countMillsPlayer(botPlayer);
            int millsOppAll = countMillsPlayer(opp);
            int millsBotFresh = countFreshMillsPlayer(botPlayer);
            int millsOppFresh = countFreshMillsPlayer(opp);
            int potMillsBot = countPotentialMills(botPlayer);
            int potMillsOpp = countPotentialMills(opp);
            int blockedBot = countBlockedPieces(botPlayer);
            int blockedOpp = countBlockedPieces(opp);
            bool dblBot = hasDoubleMill(botPlayer);
            bool dblOpp = hasDoubleMill(opp);

            int score = 0;

            if (piecesInHand[botPlayer] > 0 || piecesInHand[opp] > 0) {
                score += 15 * (pieceCount[botPlayer] - pieceCount[opp]);
                score += 20 * (millsBotAll - millsOppAll);
                score += 10 * (millsBotFresh - millsOppFresh);
                score += 6 * (potMillsBot - potMillsOpp);
                score += 2 * ((int) myMoves.size() - (int) oppMoves.size());
            } else if (totalPieces > 10) {
                score += 25 * (pieceCount[botPlayer] - pieceCount[opp]);
                score += 35 * (millsBotAll - millsOppAll);
                score += 15 * (millsBotFresh - millsOppFresh);
                score += 12 * (blockedOpp - blockedBot);
                score += 8 * (potMillsBot - potMillsOpp);
                score += 4 * ((int) myMoves.size() - (int) oppMoves.size());
            } else {
                score += 40 * (pieceCount[botPlayer] - pieceCount[opp]);
                score += 40 * (millsBotAll - millsOppAll);
                score += 20 * (millsBotFresh - millsOppFresh);
                score += 15 * (blockedOpp - blockedBot);
                score += 6 * ((int) myMoves.size() - (int) oppMoves.size());
            }

            if (dblBot) score += 25;
            if (dblOpp) score -= 25;

            return score;
        }
    };

    bool sameCapturePair(int a1, int a2, int b1, int b2) {
        if (a1 == -1 && a2 == -1 && b1 == -1 && b2 == -1) return true;
        if (a2 == -1 && b2 == -1) return a1 == b1;
        return ((a1 == b1 && a2 == b2) || (a1 == b2 && a2 == b1));
    }

    bool movesEqual(const Move &a, const Move &b) {
        if (a.from != b.from) return false;
        if (a.to != b.to) return false;
        return sameCapturePair(a.remove1, a.remove2, b.remove1, b.remove2);
    }

    bool blocksOppMill(const Board &b, int player, int pos) {
        int opp = 1 - player;
        for (const auto &t: Board::mills) {
            bool hasPos = false;
            int oppOthers = 0;
            for (int idx: t) {
                if (idx == pos) {
                    hasPos = true;
                } else {
                    if (b.cells[idx] == opp) ++oppOthers;
                    else if (b.cells[idx] != Board::EMPTY) {
                        oppOthers = -100;
                        break;
                    }
                }
            }
            if (hasPos && oppOthers == 2) return true;
        }
        return false;
    }

    int movePriority(const Move &m, Board &board, int player) {
        int sc = 0;
        if (m.remove1 != -1 || m.remove2 != -1) sc += 4000;
        auto newMs = board.newMillsForMove(m.to, player, m.from);
        if (!newMs.empty()) sc += 2000;
        if (blocksOppMill(board, player, m.to)) sc += 1500;
        if (board.piecesInHand[player] > 0 && m.from == -1) {
            if (Board::indexToCoord[m.to][0] == 'd') sc += 50;
        }
        return sc;
    }

    int alphaBeta(Board &board, int depth, int alpha, int beta, int currentPlayer, int botPlayer) {
        const int WIN = 1000000;
        int opp = 1 - currentPlayer;

        if (depth == 0) {
            return board.evaluate(botPlayer);
        }

        if (board.pieceCount[opp] <= 2 && board.piecesInHand[opp] == 0) {
            return (currentPlayer == botPlayer) ? WIN : -WIN;
        }
        if (board.pieceCount[currentPlayer] <= 2 && board.piecesInHand[currentPlayer] == 0) {
            return (currentPlayer == botPlayer) ? -WIN : WIN;
        }

        auto moves = board.generateMoves(currentPlayer);
        if (moves.empty()) {
            if (board.piecesInHand[currentPlayer] == 0) {
                return (currentPlayer == botPlayer) ? -WIN : WIN;
            }
            return board.evaluate(botPlayer);
        }

        sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {
            return movePriority(a, board, currentPlayer) > movePriority(b, board, currentPlayer);
        });

        bool maximizing = (currentPlayer == botPlayer);
        int best = maximizing ? numeric_limits<int>::min() : numeric_limits<int>::max();

        for (const Move &m: moves) {
            board.applyMove(m, currentPlayer);
            int val = alphaBeta(board, depth - 1, alpha, beta, opp, botPlayer);
            board.undoLastMove();

            if (maximizing) {
                if (val > best) best = val;
                if (best > alpha) alpha = best;
                if (alpha >= beta) break;
            } else {
                if (val < best) best = val;
                if (best < beta) beta = best;
                if (alpha >= beta) break;
            }
        }
        return best;
    }

    Move findBestMove(Board &board, int botPlayer) {
        int maxDepth;
        int totalPieces = board.pieceCount[0] + board.pieceCount[1];

        if (board.piecesInHand[0] + board.piecesInHand[1] > 0) {
            maxDepth = 5;
        } else if (totalPieces > 10) {
            maxDepth = 7;
        } else {
            maxDepth = 8;
        }

        auto moves = board.generateMoves(botPlayer);
        if (moves.empty()) return {-1, -1, -1, -1};

        sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {
            return movePriority(a, board, botPlayer) > movePriority(b, board, botPlayer);
        });

        Move lastBotMove = {-1, -1, -1, -1};
        bool hasLastBotMove = false;
        for (auto it = board.history.rbegin(); it != board.history.rend(); ++it) {
            if (it->player == botPlayer) {
                lastBotMove = it->move;
                hasLastBotMove = true;
                break;
            }
        }

        Move bestMove = moves[0];
        int bestVal = numeric_limits<int>::min();
        int alpha = numeric_limits<int>::min();
        int beta = numeric_limits<int>::max();

        int opp = 1 - botPlayer;

        for (const Move &m: moves) {
            auto newMsForM = board.newMillsForMove(m.to, botPlayer, m.from);

            board.applyMove(m, botPlayer);
            int val = alphaBeta(board, maxDepth - 1, alpha, beta, opp, botPlayer);
            board.undoLastMove();

            if (hasLastBotMove) {
                bool isReverse = (m.from == lastBotMove.to && m.to == lastBotMove.from);
                if (isReverse && m.remove1 == -1 && m.remove2 == -1 && newMsForM.empty()) {
                    val -= 60;
                }
            }

            if (val > bestVal) {
                bestVal = val;
                bestMove = m;
            }
            if (bestVal > alpha) alpha = bestVal;
        }
        return bestMove;
    }

    bool parseMoveString(const string &line, Move &m, const Board &board, int player) {
        vector<string> tokens;
        string cur;
        for (char ch: line) {
            if (isspace(static_cast<unsigned char>(ch))) {
                if (!cur.empty()) {
                    tokens.push_back(cur);
                    cur.clear();
                }
            } else {
                cur.push_back(ch);
            }
        }
        if (!cur.empty()) tokens.push_back(cur);
        if (tokens.empty()) return false;

        auto getIdx = [](const string &s) -> int {
            return Board::coordToIndex(s);
        };

        bool placing = board.piecesInHand[player] > 0;

        if (placing) {
            if (tokens.size() < 1 || tokens.size() > 3) return false;
            int to = getIdx(tokens[0]);
            if (to == -1) return false;
            int r1 = -1, r2 = -1;
            if (tokens.size() >= 2) {
                r1 = getIdx(tokens[1]);
                if (r1 == -1) return false;
            }
            if (tokens.size() == 3) {
                r2 = getIdx(tokens[2]);
                if (r2 == -1) return false;
            }
            m = {-1, to, r1, r2};
            return true;
        } else {
            if (tokens.size() < 2 || tokens.size() > 4) return false;
            int from = getIdx(tokens[0]);
            int to = getIdx(tokens[1]);
            if (from == -1 || to == -1) return false;
            int r1 = -1, r2 = -1;
            if (tokens.size() >= 3) {
                r1 = getIdx(tokens[2]);
                if (r1 == -1) return false;
            }
            if (tokens.size() == 4) {
                r2 = getIdx(tokens[3]);
                if (r2 == -1) return false;
            }
            m = {from, to, r1, r2};
            return true;
        }
    }

    int run() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);

        Board board;
        const int BOT = 0;
        const int HUMAN = 1;

        while (true) {
            Move botMove = findBestMove(board, BOT);
            if (botMove.to == -1) {
                cout << "Bot has no moves. Game over.\n";
                break;
            }
            board.applyMove(botMove, BOT);
            cout << "BOT: " << Board::moveToString(botMove) << "\n";
            board.printBoard(cout);

            if (board.pieceCount[HUMAN] <= 2 && board.piecesInHand[HUMAN] == 0) {
                cout << "Bot wins.\n";
                break;
            }
            auto humanMoves = board.generateMoves(HUMAN);
            if (board.piecesInHand[HUMAN] == 0 && humanMoves.empty()) {
                cout << "Bot wins (human has no moves).\n";
                break;
            }

            while (true) {
                bool placing = board.piecesInHand[HUMAN] > 0;
                if (placing) {
                    cout << "Your move (PLACE): examples: a1, a1 b4, a1 b4 d2, or u1 to undo:\n";
                } else {
                    cout << "Your move (MOVE): examples: f2 f4, f2 f4 b6, or u1 to undo:\n";
                }
                string line;
                if (!getline(cin, line)) return 0;

                if (line == "u1" || line == "U1") {
                    if (!board.history.empty()) board.undoLastMove();
                    if (!board.history.empty()) board.undoLastMove();
                    board.printBoard(cout);
                    continue;
                }

                Move hm;
                if (!parseMoveString(line, hm, board, HUMAN)) {
                    cout << "Cannot parse move. Try again.\n";
                    continue;
                }

                auto legal = board.generateMoves(HUMAN);
                bool ok = false;
                for (const auto &lm: legal) {
                    if (movesEqual(lm, hm)) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    cout << "Illegal move. Try again.\n";
                    continue;
                }

                board.applyMove(hm, HUMAN);
                cout << "HUMAN: " << Board::moveToString(hm) << "\n";
                board.printBoard(cout);
                break;
            }

            if (board.pieceCount[BOT] <= 2 && board.piecesInHand[BOT] == 0) {
                cout << "Human wins.\n";
                break;
            }
            auto botMoves = board.generateMoves(BOT);
            if (board.piecesInHand[BOT] == 0 && botMoves.empty()) {
                cout << "Human wins (bot has no moves).\n";
                break;
            }
        }

        return 0;
    }

    const array<string, 24> Board::indexToCoord = {
            "a1", "d1", "g1",
            "b2", "d2", "f2",
            "c3", "d3", "e3",
            "a4", "b4", "c4", "e4", "f4", "g4",
            "c5", "d5", "e5",
            "b6", "d6", "f6",
            "a7", "d7", "g7"
    };

    const array<array<int, 3>, 16> Board::mills = {{
                                                           {0, 1, 2},
                                                           {3, 4, 5},
                                                           {6, 7, 8},
                                                           {9, 10, 11},
                                                           {12, 13, 14},
                                                           {15, 16, 17},
                                                           {18, 19, 20},
                                                           {21, 22, 23},
                                                           {0, 9, 21},
                                                           {3, 10, 18},
                                                           {6, 11, 15},
                                                           {1, 4, 7},
                                                           {16, 19, 22},
                                                           {8, 12, 17},
                                                           {5, 13, 20},
                                                           {2, 14, 23}
                                                   }};

    const array<vector<int>, 24> Board::neighbours = []() {
        array<vector<int>, 24> n;
        n[0] = {1, 9};
        n[1] = {0, 2, 4};
        n[2] = {1, 14};
        n[3] = {4, 10};
        n[4] = {3, 5, 1, 7};
        n[5] = {4, 13};
        n[6] = {7, 11};
        n[7] = {6, 8, 4};
        n[8] = {7, 12};
        n[9] = {0, 10, 21};
        n[10] = {9, 11, 3, 18};
        n[11] = {10, 6, 15};
        n[12] = {8, 13, 17};
        n[13] = {12, 14, 5, 20};
        n[14] = {13, 2, 23};
        n[15] = {11, 16};
        n[16] = {15, 17, 19};
        n[17] = {16, 12};
        n[18] = {10, 19};
        n[19] = {18, 20, 16, 22};
        n[20] = {19, 13};
        n[21] = {9, 22};
        n[22] = {21, 23, 19};
        n[23] = {22, 14};
        return n;
    }();
}

#endif
