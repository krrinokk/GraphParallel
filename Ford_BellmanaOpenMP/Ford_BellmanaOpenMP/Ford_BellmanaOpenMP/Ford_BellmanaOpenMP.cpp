#include <iostream>
#include <vector>
#include <fstream>
#include <omp.h>

using namespace std;

const int INF = 2000000000;

struct Edge {
    int from, to, weight;
};

int main() {
    setlocale(LC_ALL, "Russian");
    int n, m;
    ifstream fin("input.txt");
    fin >> n >> m;

    vector<Edge> edges(m);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        fin >> u >> v >> w;
        edges[i] = { u, v, w };
    }
    fin.close();

    int s;
    cout << "Введите вершину, от которой будут высчитываться расстояния: ";
    cin >> s;
    if (s < 0 || s >= n) {
        cout << "Введенная вершина не существует!" << endl;
        return 0;
    }

    vector<int> d(n, INF);
    vector<int> p(n, -1);
    d[s] = 0;

    double start_time = omp_get_wtime();

    for (int i = 1; i <= n - 1; i++) {
#pragma omp parallel for
        for (int j = 0; j < m; j++) {
            int u = edges[j].from;
            int v = edges[j].to;
            int w = edges[j].weight;
            if (d[u] != INF && d[u] + w < d[v]) {
#pragma omp critical
                {
                    d[v] = d[u] + w;
                    p[v] = u;
                }
            }
        }
    }

    for (int j = 0; j < m; j++) {
        int u = edges[j].from;
        int v = edges[j].to;
        int w = edges[j].weight;
        if (d[u] != INF && d[u] + w < d[v]) {
            cout << "Отрицательный цикл обнаружен!" << endl;
            exit(0);
        }
    }

    double end_time = omp_get_wtime();
    double time = end_time - start_time;

    ofstream fout("output.txt");
    for (int i = 0; i < n; i++) {
        cout << "Расстояние от вершины " << s << " до вершины " << i << ": ";
        fout << "Расстояние от вершины " << s << " до вершины " << i << ": ";
        if (d[i] == 2000000000) {
            cout << "INF" << endl;
            fout << "INF" << endl;
        }
        else {
            cout << d[i] << endl;
            fout << d[i] << endl;
        }
    }
    fout << "Execution time: " << time << " sec\n";
    fout.close();

    return 0;
}