#include <iostream>
#include <fstream>
#include <vector>
#include "mpi.h"

using namespace std;


#define MPIPrintf(format, ...) printf("[%i]: " format, rank, ##__VA_ARGS__); fflush(stdout)

void __MPIAssert(int rank, bool condition, const char * const cond_str, const char * const func, int line) {
    if (!condition) {
        MPIPrintf("Assert condition [ %s ] failed at (%s):%i. Aborting...\n", cond_str, func, line);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
}

#define MPIAssert(condition) __MPIAssert(rank, (condition), #condition, __FUNCTION__, __LINE__)
#define MPIPv(var) cout << "[" << rank << "]: " << #var << " = " << var << std::flush
#define MPIPn cout << endl
#define MASTER_RANK 0


// algoritmul lui Euclid
int cmmdc(int a, int b) {
    if (b == 0) {
        return a;
    }

    return cmmdc(b, a % b);
}

// cel mai mare multiplu comun a 2 numere
long long cmmmc(int a, int b) {
    return (long long)(a / cmmdc(a, b)) * b;
}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPIAssert(argc == 2);
    int debug = atoi(argv[1]);

    long long int N;
    int num_divs;
    int *divs = NULL;
    if (rank == MASTER_RANK) {
        ifstream in("suman.in");

        in >> N >> num_divs;
        divs = (int*)malloc(sizeof(int) * num_divs);

        for (int i = 0; i < num_divs; ++i) {
            in >> divs[i];
        }

        if (debug) {
            MPIPrintf("N = %lli; num_divs = %i\n", N, num_divs);
            for (int i = 0; i < num_divs; ++i) {
                MPIPrintf("divs[%i] = %i\n", i, divs[i]);
            }
        }

        in.close();
    }


    // broadcast input
    struct primary_input {
        long long int N;
        int num_divs;
    };

    primary_input inp;
    inp.N = N;
    inp.num_divs = num_divs;
    MPI_Bcast(&inp, sizeof(inp), MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
    N = inp.N;
    num_divs = inp.num_divs;

    if (rank != MASTER_RANK) {
        divs = (int*)malloc(sizeof(int) * num_divs);
    }

    MPI_Bcast(divs, num_divs, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);


    int limit = 1<<num_divs;
    MPIAssert(limit % size == 0);
    int chunkSize = limit / size;
    int proc_start = rank * chunkSize;
    int proc_end = proc_start + chunkSize;
    long long suma_locala = 0;

    if (debug) {
        MPIPv(limit); MPIPn;
        MPIPv(chunkSize); MPIPn;
        MPIPv(proc_start); MPIPn;
        MPIPv(proc_end); MPIPn;
        MPIPn;
    }

    for (int mask = max(proc_start, 1); mask < proc_end; ++mask) { // itereaza peste submultimi
        int nr_elemente = 0;
        long long multiplu_comun = 1;

        bool too_big = false;
        for (int b = 0; b < num_divs; ++b) { // itereaza peste posibilele elemente din submultimea curenta
            if (mask & (1 << b)) { // verifica ca elementul curent e in submultime
                nr_elemente += 1;
                multiplu_comun = cmmmc(multiplu_comun, divs[b]);
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

        if (debug > 2) {
            MPIPv(nr_elemente); MPIPn;
            MPIPv(multiplu_comun); MPIPn;
            MPIPv(cardinal_submultime); MPIPn;
            MPIPv(suma_submultime); MPIPn;
            MPIPn;
        }

        if (nr_elemente & 1) { // impar
            suma_locala += suma_submultime;
        }
        else { // par
            suma_locala -= suma_submultime;
        }
    }

    long long suma_totala = 0;
    MPI_Reduce(&suma_locala, &suma_totala, 1, MPI_LONG_LONG_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);

    if (debug) {
        MPIPrintf("suma_locala = %lli\n", suma_locala);
    }

    if (rank == MASTER_RANK) {
        MPIPrintf("suma_totala = %lli\n", suma_totala);
        ofstream out("suman.out");
        out << suma_totala << '\n';
        out.close();
    }

    free(divs);
    MPI_Finalize();
    return 0;
}


