#include <ios>
#include <queue>
#include <vector>
#include <chrono>
#include <iostream>
#include <unordered_set>

#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC target("avx2,popcnt,lzcnt,abm,bmi,bmi2,fma,tune=native")

using namespace std;

#define endln "\n"
#define HASH_SIZE 100000
#define MICROSECONDS_TO_SECONDS 1000000.0
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

    cout << res << endln;
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

}

void countTime(ll a, ll b, void f(ll a, ll b)) {
    chrono::time_point start = chrono::high_resolution_clock::now();
    f(a, b);
    chrono::time_point end = chrono::high_resolution_clock::now();
    chrono::microseconds microseconds = chrono::duration_cast<chrono::microseconds>(end - start);
    double seconds = microseconds.count() / MICROSECONDS_TO_SECONDS;
    cout << "Время: " << seconds << " секунд" << endl;
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    ll a = 2, b = 100;
    countTime(a, b, subTask3WithSubstraction);

    return 0;
}