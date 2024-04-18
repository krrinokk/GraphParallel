#include <iostream>
#include <vector>
#include <fstream>
#include <mpi.h>

using namespace std;

const int INF = 2000000000;

struct Edge {
    int from, to, weight;
};

vector<int> bellmanFordAlgorithm(vector<Edge>& edges, int n, int s, int rank, int size) {
    vector<int> d(n, INF);
    vector<int> p(n, -1);
    d[s] = 0;

    for (int i = 1; i <= n - 1; i++) {
        for (int j = rank; j < edges.size(); j += size) {
            int u = edges[j].from;
            int v = edges[j].to;
            int w = edges[j].weight;
            if (d[u] != INF && d[u] + w < d[v]) {
                d[v] = d[u] + w;
                p[v] = u;
            }
        }
        MPI_Allreduce(MPI_IN_PLACE, &d[0], n, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    }

    // Check for negative cycle
    for (int j = rank; j < edges.size(); j += size) {
        int u = edges[j].from;
        int v = edges[j].to;
        int w = edges[j].weight;
        if (d[u] != INF && d[u] + w < d[v]) {
            cout << "Negative cycle detected!" << endl;
            MPI_Finalize();
            exit(0);
        }
    }

    return d;
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    int n, m, s, rank, size;
    ifstream fin("input.txt");

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        fin >> n >> m;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<Edge> edges(m);
    if (rank == 0) {
        for (int i = 0; i < m; i++) {
            int u, v, w;
            fin >> u >> v >> w;
            edges[i] = { u, v, w };
        }
        fin.close();

        cout << "Enter the vertex from which distances will be calculated: ";
        cin >> s;
        if (s < 0 || s >= n) {
            cout << "The entered vertex does not exist!" << endl;
            MPI_Finalize();
            return 0;
        }
    }
    MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> d = bellmanFordAlgorithm(edges, n, s, rank, size);

    if (rank == 0) {
        ofstream fout("output.txt");
        for (int i = 0; i < n; i++) {
            cout << "Distance from vertex " << s << " to vertex " << i << ": ";
            fout << "Distance from vertex " << s << " to vertex " << i << ": ";
            if (d[i] == 2000000000) {
                cout << "INF" << endl;
                fout << "INF" << endl;
            }
            else {
                cout << d[i] << endl;
                fout << d[i] << endl;
            }
        }
        fout.close();
    }

    MPI_Finalize();
    return 0;
}