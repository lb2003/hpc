#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <iostream>
#include <ctime>
#include <bitset>
#define gc getchar()
#define ll long long
#define il inline
#define fi first
#define se second
#define mk make_pair
#define pii pair<int, int>
#define pll pair<ll, ll>
#define pb push_back
#define rep(i, a, b) for (int i = a; i <= b; ++i)
#define repn(i, b, a) for (int i = b; i >= a; --i)
#define ite iterator
#define eps 1e-8

using namespace std;

template<class o> il void qr(o &x) {
    x = 0; char c = gc; int f = 1;
    while (c < '0' || c > '9') {if (c == '-') f = -1; c = gc;}
    while (c >= '0' && c <= '9') {x = x * 10 + (c ^ 48); c = gc;}
    x *= f;
}

template<class o> void qw(o x) {
    if(x / 10) qw(x / 10); putchar(x % 10 + 48);
}

template<class o> void pr1(o x) {
    if (x < 0) putchar('-'), x = -x;
    qw(x); putchar(' ');
}

template<class o> void pr2(o x) {
    if (x < 0) putchar('-'), x = -x;
    qw(x); puts("");
}

const int N = 16;

int main() {
    freopen("a.in", "w", stdout);
    printf("%d %d %d\n", N, N, N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%d.0 ", i * N + j + 1);
        } puts("");
    }
    puts("");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%d.0 ", i * N + j + 1);
        } 
        puts("");
    } puts("");
    return 0;
}
