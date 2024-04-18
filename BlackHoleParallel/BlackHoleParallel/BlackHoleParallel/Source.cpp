#include <iostream>
#include <vector>
#include <mpi.h>
using namespace std;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    if (rank == 0) {
        cout << "\nNumber of top? : ";
        cin >> n;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<vector<int>> matrix(n, vector<int>(n));
    if (rank == 0) {
        printf("Fill in the incidence matrix:");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cin >> matrix[i][j];
            }
        }
    }
    MPI_Bcast(&matrix[0][0], n * n, MPI_INT, 0, MPI_COMM_WORLD);

    vector<bool> black_hole(n);
    for (int i = rank; i < n; i += size) {
        bool is_black_hole = true;
        int cnt_outgoing_edges = 0;
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == 1) {
                is_black_hole = false;
                break;
            }
            else if (matrix[i][j] == -1) {
                cnt_outgoing_edges++;
            }
        }
        if (is_black_hole && cnt_outgoing_edges > 0) {
            black_hole[i] = true;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = rank; i < n; i += size) {
        if (black_hole[i]) {
            bool is_second_grade = true;
            for (int j = 0; j < n; j++) {
                if (matrix[j][i] == 1) {
                    is_second_grade = false;
                    break;
                }
            }
            if (is_second_grade) {
                if (rank == 0) {
                    printf("Number of BlackHole second range=%d\n", i);
                }
            }
        }
    }

    MPI_Finalize();
    return 0;
}