#include <iostream>
#include <fstream>
#include <mpi.h>
#include <omp.h>
#define INF 999

using namespace std;

void printMatrix(int matrix[][10], int nV, ofstream& out) {
    for (int i = 0; i < nV; i++) {
        for (int j = 0; j < nV; j++) {
            if (matrix[i][j] == INF)
                out << "INF ";
            else
                out << matrix[i][j] << " ";
        }
        out << "\n";
    }
}

void floydWarshall(int graph[][10], int nV, int rank, int size) {
    int matrix[10][10], i, j, k;

    for (i = 0; i < nV; i++)
        for (j = 0; j < nV; j++)
            matrix[i][j] = graph[i][j];

    ofstream out("result.txt");

    if (rank == 0) {
        out << "Initial weight matrix:\n";
        printMatrix(graph, nV, out);
    }

    int chunkSize = nV / size;
    int start = rank * chunkSize;
    int end = start + chunkSize;
    if (rank == size - 1) {
        end = nV;
    }
    double start_time = omp_get_wtime();
    for (k = 0; k < nV; k++) {
        MPI_Bcast(&matrix[k], nV, MPI_INT, k % size, MPI_COMM_WORLD);

        for (i = start; i < end; i++) {
            for (j = 0; j < nV; j++) {
                if (matrix[i][k] != INF && matrix[k][j] != INF && matrix[i][k] + matrix[k][j] < matrix[i][j])
                    matrix[i][j] = matrix[i][k] + matrix[k][j];
            }
        }

        MPI_Allgather(MPI_IN_PLACE, nV * chunkSize, MPI_INT, matrix, nV * chunkSize, MPI_INT, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        out << "Weight matrix after Floyd-Warshall algorithm:\n";
        printMatrix(matrix, nV, out);

        int vertex;
        cout << "Enter vertex number: ";
        cin >> vertex;
        vertex--;

        if (vertex < 0 || vertex >= nV) {
            cout << "Invalid vertex number!\n";
            return;
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
        double end_time = omp_get_wtime();
        double time = end_time - start_time;
        out << "Execution time: " << time << " sec\n";
        out.close();
        cout << "Results saved to result.txt\n";
    }
}

int main(int argc, char** argv) {
    int nV, rank, size;
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
            if (graph[i][j] == -1)
                graph[i][j] = INF;
        }
    }
    file.close();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    floydWarshall(graph, nV, rank, size);

    MPI_Finalize();
    return 0;
}