
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define KMAX 20 + 5


long long cmmdc(long long a, long long b) {
    if (b == 0) {
        return a;
    }

    return cmmdc(b, a % b);
}

// lowest common multiple
long long cmmmc(long long a, long long b) {
    return (a / cmmdc(a, b)) * (long long)b;
}


int main() {
    ifstream in("suman.in");
    ofstream out("suman.out");

    long long int N;
    int numDivisors, divisors[KMAX];
    in >> N >> numDivisors;

    for (int i = 0; i < numDivisors; ++i) {
        in >> divisors[i];
    }

    int limit_mask = (1<<numDivisors);
    long long total_suma = 0;
    for (int mask = 1; mask < limit_mask; ++mask) { // itereaza peste submultimi

        long long nr_elemente = 0;
        long long multiplu_comun = 1;
        bool too_big = false;

        for (int b = 0; b < numDivisors; ++b) { // itereaza peste posibilele elemente din submultimea curenta
            if (mask & (1 << b)) { // verifica ca elementul curent e in submultime
                nr_elemente += 1;
                multiplu_comun = cmmmc(multiplu_comun, divisors[b]);
                if (multiplu_comun > N) {
                    too_big = true;
                    break;
                }
            }
        }

        if (too_big) {
            continue;
        }

        long long cardinal_submultime = N / multiplu_comun;
        long long suma_submultime = multiplu_comun * (cardinal_submultime * (cardinal_submultime + 1) / 2);

        if (nr_elemente & 1) {
            total_suma += suma_submultime;
        }
        else {
            total_suma -= suma_submultime;
        }

        // printf("mask = %i\n", mask);
        // printf("submultime:\n");
        // for (int i = 0; i < num_submultime; ++i) {
        //     printf("%i ", submultime[i]);
        // }
        // printf("\n");
        // printf("cardinal_submultime = %llii\n", cardinal_submultime);
        // printf("suma_submultime = %llii\n", suma_submultime);
        // printf("total_suma = %lli\n\n", total_suma);
    }

    out << total_suma << '\n';
    cout << total_suma << '\n';

    return 0;
}

