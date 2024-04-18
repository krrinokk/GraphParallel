﻿#include <iostream>
#include <vector>
#include <fstream> // библиотека для работы с файлами
#include <omp.h>
using namespace std;

const int INF = 2000000000; // "бесконечность"

struct Edge {
    int from, to, weight;
};

vector<int> bellmanFordAlgorithm(vector<Edge>& edges, int n, int s) {
    vector<int> d(n, INF); // массив расстояний
    vector<int> p(n, -1); // массив предков
    d[s] = 0;

    for (int i = 1; i <= n - 1; i++) {
        for (int j = 0; j < edges.size(); j++) {
            int u = edges[j].from;
            int v = edges[j].to;
            int w = edges[j].weight;
            if (d[u] != INF && d[u] + w < d[v]) {
                d[v] = d[u] + w;
                p[v] = u;
            }
        }
    }

    // Проверка на наличие отрицательного цикла
    for (int j = 0; j < edges.size(); j++) {
        int u = edges[j].from;
        int v = edges[j].to;
        int w = edges[j].weight;
        if (d[u] != INF && d[u] + w < d[v]) {
            cout << "Отрицательный цикл обнаружен!" << endl;
            exit(0);
        }
    }

    return d;
}

int main() {
    setlocale(LC_ALL, "Russian");
    int n, m;
    ifstream fin("input.txt"); // открытие файла для чтения
    fin >> n >> m;

    vector<Edge> edges(m);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        fin >> u >> v >> w;
        edges[i] = { u, v, w };
    }
    fin.close(); // закрытие файла

    int s;
    cout << "Введите вершину, от которой будут высчитываться расстояния: ";
    cin >> s;
    if (s < 0 || s >= n) {
        cout << "Введенная вершина не существует!" << endl;
        return 0;
    }
    double start_time = omp_get_wtime();
    vector<int> d = bellmanFordAlgorithm(edges, n, s);

    // вывод расстояний на экран и в файл
    ofstream fout("output.txt"); // открытие файла для записи
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
    double end_time = omp_get_wtime();
    double time = end_time - start_time;
    fout << "Execution time: " << time << " sec\n";
    fout.close(); // закрытие файла

    return 0;
}