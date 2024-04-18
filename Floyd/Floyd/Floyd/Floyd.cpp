#include <iostream>
#include <fstream>
#include <omp.h>
#include <chrono>
#include <iomanip>
using namespace std;
#define INF 999

using namespace std;


int main() {
    int nV;
    ifstream file("matrix.txt");
    if (!file) {
        cout << "File not found!\n";
        return 1;
    }
    file >> nV;
    int graph[10][10];
    for (int i = 0; i < nV; i++) {
        for (int j = 0; j < nV; j++) {
            file >> graph[i][j];
            if (graph[i][j] == -1) // заменяем -1 на INF
                graph[i][j] = INF;
        }
    }
    file.close();
    int matrix[10][10], i, j, k;

    for (i = 0; i < nV; i++)
        for (j = 0; j < nV; j++)
            matrix[i][j] = graph[i][j];

    ofstream out("result.txt");

    out << "Initial weight matrix:\n";
    //  cout << "Weight matrix:\n";
    for (int i = 0; i < nV; i++) {
        for (int j = 0; j < nV; j++) {
            if (matrix[i][j] == INF)
                out << "INF ";
            else
                out << matrix[i][j] << " ";
        }
        out << "\n";
    }

    auto start = chrono::high_resolution_clock::now();

#pragma omp parallel for private(k, i, j) shared(matrix)
    for (k = 0; k < nV; k++) {
        for (i = 0; i < nV; i++) {
            for (j = 0; j < nV; j++) {
                if (matrix[i][k] != INF && matrix[k][j] != INF && matrix[i][k] + matrix[k][j] < matrix[i][j])
                    matrix[i][j] = matrix[i][k] + matrix[k][j];
            }
        }
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::duration<double>>(stop - start);

    out << "Weight matrix after Floyd-Warshall algorithm:\n";
    //  cout << "Weight matrix:\n";
    for (int i = 0; i < nV; i++) {
        for (int j = 0; j < nV; j++) {
            if (matrix[i][j] == INF)
                out << "INF ";
            else
                out << matrix[i][j] << " ";
        }
        out << "\n";
    }

    out << "Execution time: " << fixed << setprecision(10) << duration.count() << " seconds\n";

    int vertex;
    cout << "Enter vertex number: ";
    cin >> vertex;
    vertex--;

    if (vertex < 0 || vertex >= nV) {
        cout << "Invalid vertex number!\n";
        return 0;
    }

    out << "Shortest paths from vertex " << vertex << ":\n";
    for (int i = 0; i < nV; i++) {
        if (matrix[vertex][i] != INF && i != vertex) {
            out << "Path to vertex " << i << ": ";
            int curr = i;
            while (curr != vertex) {
                out << curr << " <- ";
                curr = k;
                for (k = 0; k < nV; k++) {
                    if (matrix[vertex][k] != INF && matrix[k][curr] != INF && matrix[vertex][k] + matrix[k][curr] == matrix[vertex][curr])
                        break;
                }
            }
            out << vertex << "\n";
            out << "Weight: " << matrix[vertex][i] << "\n";
        }
        else if (matrix[vertex][i] == INF && i != vertex) {
            out << "No path from vertex " << vertex << " to vertex " << i << "\n";
        }
    }

    out.close();
    cout << "Results saved to result.txt\n";
    return 0;
}