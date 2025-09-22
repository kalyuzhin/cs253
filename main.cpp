#include <ios>
#include <queue>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <iostream>
#include <unordered_set>

#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC target("avx2,popcnt,lzcnt,abm,bmi,bmi2,fma,tune=native")

using namespace std;

#define endln "\n"
#define HASH_SIZE 100000
#define MICROSECONDS_TO_SECONDS 1000000.0
#define NUM_THREADS 2
#define ll long long
#define ull unsigned long long
#define vll vector<ll>
#define vull vector<ull>

template<typename T>
using vec = vector<T>;


// Даны два целых числа – например, 2 и 100, а также две операции – «прибавить 3» и «умножить на 2».
// Найти минимальную последовательность операций, позволяющую получить из первого числа второе.
void subTask1(ll a, ll b) {
    ll res;
    if (a > b) {
        cout << "Невозможно получить из первого числа второе" << endl;
    }
    res = 0;
    ll cur = b;

    for (;;) {
        if (cur % 2 == 0 && (cur / 2 < cur - 3 || cur - 3 < a)) {
            cur /= 2;
        } else {
            cur -= 3;
        }
        ++res;
        if (cur == a || cur <= 0) {
            break;
        }
    }

    cout << "Мин кол-во шагов: " << res << endln;
}

// То же самое, что и в пункте 1, однако добавляется операция «вычесть 2».
void subTask2(ll a, ll b) {
    ll res, cntVerts, tmpVal, tmpCnt, cnt;
    if (a > b) {
        cout << "Невозможно получить из первого числа второе" << endl;
    }

    res = 0, cntVerts = 0, cnt = 0;
    queue<pair<ll, ll>> q;
    q.push(make_pair(a, 0));

    unordered_set<ll> visited;
    visited.reserve(HASH_SIZE);
    visited.insert(a);


    for (; !q.empty();) {
        tmpVal = q.front().first, tmpCnt = q.front().second;
        q.pop();
        ++cntVerts;
        if (tmpVal == b) {
            res = tmpCnt;
            break;
        }
        for (ll i = 0; i != 3; ++i) {
            ll nextVal = tmpVal;
            cnt = tmpCnt;
            if (i == 0) {
                nextVal += 3;
                ++cnt;
            } else if (i == 1) {
                nextVal *= 2;
                ++cnt;
            } else {
                nextVal -= 2;
                ++cnt;
            }

            if (nextVal < 0 && b >= 0) {
                continue;
            }
            if (nextVal > b * 2 && b > 0) {
                continue;
            }

            if (visited.find(nextVal) == visited.end()) {
                visited.insert(nextVal);
                q.push(make_pair(nextVal, cnt));
            }
        }


    }

    cout << "Кол-во посещенных вершин: " << cntVerts << endln;
    cout << "Мин кол-во шагов: " << res << endln;
}

//Реализовать задание из пункта 1 методом обратного поиска – от целевого состояния к начальному.
// Сравнить эффективность.
void subTask3WithSubstraction(ll a, ll b) {
    ll res, cntVerts, tmpVal, tmpCnt, cnt;
    if (a > b) {
        cout << "Невозможно получить из первого числа второе" << endl;
    }

    res = 0, cntVerts = 0, cnt = 0;
    queue<pair<ll, ll>> q;
    q.push(make_pair(b, 0));

    unordered_set<ll> visited;
    visited.reserve(HASH_SIZE);
    visited.insert(b);


    for (; !q.empty();) {
        tmpVal = q.front().first, tmpCnt = q.front().second;
        q.pop();
        ++cntVerts;
        if (tmpVal == a) {
            res = tmpCnt;
            break;
        }
        for (ll i = 0; i != 3; ++i) {
            ll nextVal = tmpVal;
            cnt = tmpCnt;
            if (i == 0) {
                nextVal -= 3;
                ++cnt;
            } else if (i == 1 && nextVal % 2 == 0) {
                nextVal /= 2;
                ++cnt;
            } else if (i == 2) {
                nextVal += 2;
                ++cnt;
            }

            if (nextVal < a && a >= 0) {
                continue;
            }
            if (nextVal > b * 2 && b > 0) {
                continue;
            }

            if (visited.find(nextVal) == visited.end()) {
                visited.insert(nextVal);
                q.push(make_pair(nextVal, cnt));
            }
        }


    }

    cout << "Кол-во посещенных вершин: " << cntVerts << endln;
    cout << "Мин кол-во шагов: " << res << endln;
}

void subTask3WOSubstraction(ll a, ll b) {
    ll res, cntVerts, tmpVal, tmpCnt, cnt;
    if (a > b) {
        cout << "Невозможно получить из первого числа второе" << endl;
    }

    res = 0, cntVerts = 0, cnt = 0;
    queue<pair<ll, ll>> q;
    q.push(make_pair(b, 0));

    unordered_set<ll> visited;
    visited.reserve(HASH_SIZE);
    visited.insert(b);


    for (; !q.empty();) {
        tmpVal = q.front().first, tmpCnt = q.front().second;
        q.pop();
        ++cntVerts;
        if (tmpVal == a) {
            res = tmpCnt;
            break;
        }
        for (ll i = 0; i != 2; ++i) {
            ll nextVal = tmpVal;
            cnt = tmpCnt;
            if (i == 0) {
                nextVal -= 3;
                ++cnt;
            } else if (i == 1 && nextVal % 2 == 0) {
                nextVal /= 2;
                ++cnt;
            }

            if (nextVal < a && a >= 0) {
                continue;
            }
            if (nextVal > b * 2 && b > 0) {
                continue;
            }

            if (visited.find(nextVal) == visited.end()) {
                visited.insert(nextVal);
                q.push(make_pair(nextVal, cnt));
            }
        }


    }

    cout << "Кол-во посещенных вершин: " << cntVerts << endln;
    cout << "Мин кол-во шагов: " << res << endln;
}

// Дополнительное задание. Реализовать метод двунаправленного поиска для решения задачи из пункта 1.
void subTask4(ll a, ll b) {
    if (a > b) {
        cout << "Невозможно получить из первого числа второе" << endln;
        return;
    }
    queue<pair<ll, ll>> q1, q2;
    unordered_map<ll, ll> visited1, visited2;
    visited1.reserve(HASH_SIZE / 2);
    visited2.reserve(HASH_SIZE / 2);

    q1.push({a, 0});
    visited1[a] = 0;

    q2.push({b, 0});
    visited2[b] = 0;

    ll cntVertsF = 0, cntVertsB = 0, res = -1, cnt = 0;

    while (!q1.empty() || !q2.empty()) {
        if (!q1.empty()) {
            ll tmpValF = q1.front().first, tmpCntF = q1.front().second;
            q1.pop();
            ++cntVertsF;
            for (ll i = 0; i != 3; ++i) {
                ll nextValF = tmpValF;
                cnt = tmpCntF;
                if (i == 0) {
                    nextValF += 3;
                    ++cnt;
                } else if (i == 1) {
                    nextValF *= 2;
                    ++cnt;
                } else {
                    nextValF -= 2;
                    ++cnt;
                }

                if (visited1.find(nextValF) == visited1.end()) {
                    visited1[nextValF] = cnt;
                    q1.push(make_pair(nextValF, cnt));
                }
                if (visited2.count(nextValF)) {
                    res = visited1[nextValF];
                    res += visited2[nextValF];
                    cout << "Кол-во посещенных вершин: " << cntVertsF + cntVertsB << endln;
                    cout << "Мин кол-во шагов: " << res << endln;

                    return;
                }
            }
        }
        if (!q2.empty()) {
            ll tmpValB = q2.front().first, tmpCntB = q2.front().second;
            q2.pop();
            for (ll i = 0; i != 3; ++i) {
                ll nextValB = tmpValB;
                cnt = tmpCntB;
                if (i == 0) {
                    nextValB -= 3;
                    ++cnt;
                } else if (i == 1 && nextValB % 2 == 0) {
                    nextValB /= 2;
                    ++cnt;
                } else if (i == 2) {
                    nextValB += 2;
                    ++cnt;
                }

                if (visited2.find(nextValB) == visited2.end()) {
                    visited2[nextValB] = cnt;
                    q2.push(make_pair(nextValB, cnt));
                }
                if (visited1.count(nextValB)) {
                    res = visited1[nextValB];
                    res += visited2[nextValB];
                    cout << "Кол-во посещенных вершин: " << cntVertsF + cntVertsB << endln;
                    cout << "Мин кол-во шагов: " << res << endln;

                    return;
                }
            }
        }
    }
}


void countTime(ll a, ll b, void f(ll a, ll b)) {
    cout << "Входные данные: a=" << a << " b=" << b << endln;
    chrono::time_point start = chrono::high_resolution_clock::now();
    f(a, b);
    chrono::time_point end = chrono::high_resolution_clock::now();
    chrono::microseconds microseconds = chrono::duration_cast<chrono::microseconds>(end - start);
    double seconds = microseconds.count() / MICROSECONDS_TO_SECONDS;
    cout << fixed << setprecision(8);
    cout << "Время: " << seconds << " секунд" << endln << endln;
}

void test() {
    vec<pair<ll, ll>> testCasesFirst{{1, 100},
                                     {2, 55},
                                     {2, 100},
                                     {1, 97},
                                     {2, 1000},
                                     {2, 10000001}};
    vec<pair<ll, ll>> testCasesSecond{{1, 100},
                                      {2, 55},
                                      {2, 100},
                                      {1, 97},
                                      {2, 1000},
                                      {2, 10000001},
                                      {3, 1001},
                                      {3, 3001}};
    cout << "Первое задание:" << endln;
    for (pair<ll, ll> t: testCasesFirst) {
        ll a = t.first;
        ll b = t.second;
        countTime(a, b, subTask1);
    }

    cout << "=================================\n";

    cout << "Второе задание:" << endln;
    for (pair<ll, ll> t: testCasesSecond) {
        ll a = t.first;
        ll b = t.second;
        countTime(a, b, subTask2);
    }

    cout << "=================================\n";

    cout << "Третье задание (без вычетания):" << endln;
    for (pair<ll, ll> t: testCasesFirst) {
        ll a = t.first;
        ll b = t.second;
        countTime(a, b, subTask3WOSubstraction);
    }

    cout << "=================================\n";

    cout << "Третье задание (с вычетанием):" << endln;
    for (pair<ll, ll> t: testCasesSecond) {
        ll a = t.first;
        ll b = t.second;
        countTime(a, b, subTask3WithSubstraction);
    }

    cout << "=================================\n";

    cout << "Четвертое задание:" << endln;
    for (pair<ll, ll> t: testCasesFirst) {
        ll a = t.first;
        ll b = t.second;
        countTime(a, b, subTask4);
    }

    cout << "=================================\n";
}

void extraTask() {

}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    test();

    return 0;
}