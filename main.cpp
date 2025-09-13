#include <ios>
#include <unordered_map>
#include <vector>
#include <iostream>

using namespace std;

using ll = long long;
using ull = unsigned long long;
using vll = vector<ll>;
using vull = vector<ull>;

template<typename T>
using vec = vector<T>;

// Даны два целых числа – например, 2 и 100, а также две операции – «прибавить 3» и «умножить на 2».
// Найти минимальную последовательность операций, позволяющую получить из первого числа второе.
void subTask1() {
    ll a, b, res;
    cin >> a >> b;
    if (a > b) {
        cout << "Невозможно получить из первого числа второе" << endl;
    }
    res = 0;
    ll cur = b;

    for (;;) {
        if (cur % 2 == 0 && (cur / 2 < cur - 3 || cur - 3 < a)) {
            cur /= 2;
            ++res;
        } else {
            cur -= 3;
            ++res;
        }
        if (cur == a || cur <= 0) {
            break;
        }
    }

    cout << res << endl;
}

// То же самое, что и в пункте 1, однако добавляется операция «вычесть 2».
void subTask2() {
    ll a, b, res;
    cin >> a >> b;
    if (a > b) {
        cout << "Невозможно получить из первого числа второе" << endl;
    }
    res = 0;
    ll cur = a;
    unordered_map<ll, ll> attended;
    attended.reserve(10000);

    for (;;) {

        if (cur == b) {
            break;
        }
    }

    for
}

//Реализовать задание из пункта 1 методом обратного поиска – от целевого состояния к начальному.
// Сравнить эффективность.
void subTask3() {

}

// Дополнительное задание. Реализовать метод двунаправленного поиска для решения задачи из пункта 1.
void subTask4() {

}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    subTask1();

    return 0;
}