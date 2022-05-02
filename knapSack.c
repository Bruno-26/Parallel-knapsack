#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define ROOT 0

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int knapSack(int n, int W, int val[], int wt[], int my_rank, int numproc, int size)
{
    int rfinal = 0;
    int *sizeini =  (int *)calloc(size, sizeof(int));
    int *sizefim =  (int *)calloc(size, sizeof(int));
    int *tam =      (int *)calloc(size, sizeof(int));

    // Ajusta o os tamanhos e indices que cada nucleo ira utilizar nos calculos
    if (my_rank == ROOT){
        int inicio;
        int fim;
        for (int i = 0; i < size; i++){
            int tamaux;

            if(i == (size-1))
                fim = W;
            else
                fim = (i + 1) * numproc;

            inicio = i * numproc;
            tamaux = fim - inicio;

            if(i == 0){
                sizeini[i] = inicio;
                tamaux++;
            }
            else
                sizeini[i] = inicio + 1;

            sizefim[i] = fim;
            tam[i]= tamaux;
        }
    }

    MPI_Bcast(sizeini, size, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(sizefim, size, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(tam, size, MPI_INT, ROOT, MPI_COMM_WORLD);

    int *data =         (int *)malloc((W + 1) * sizeof(int));
    int *result =       (int *)malloc((tam[my_rank]) * sizeof(int));
    int *resultaux =    (int *)malloc((tam[my_rank]) * sizeof(int));

    for (int i = 0; i < tam[my_rank]; i++)
        result[i]=0;

    for (int i = 0; i < W + 1; i++)
        data[i] = 0;

    for (int i = 1; i <= n; i++)
    {
        for (int w = 0; w < tam[my_rank]; w++)
        {
            if (wt[i - 1] <= w+sizeini[my_rank])
                result[w] = max(val[i - 1] + data[(w+sizeini[my_rank]) - wt[i - 1]], data[w+sizeini[my_rank]]);
            else
                result[w] = data[w+sizeini[my_rank]];
        }

        // Todos enviam seus calculos do result para o ROOT
        if (my_rank != ROOT)
            MPI_Send(result, tam[my_rank], MPI_INT, ROOT, my_rank, MPI_COMM_WORLD);
        else{
            // ROOT junta todos os results recebidos no vetor Data
            for (int k = 1; k < size; k++)
            {
                MPI_Recv(resultaux, tam[k], MPI_INT, MPI_ANY_SOURCE, k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int aux = 0;
                for (int j = sizeini[k]; j <= sizefim[k]; j++){
                    data[j] = resultaux[aux];
                    aux++;
                }
            }
        }

        // ROOT atualiza o data com seus dados
        if(my_rank == ROOT)
            for (int k = 0; k <= sizefim[0]; k++)
                data[k] = result[k];

        // Sincroniza todos os nucleos
        MPI_Bcast(data, W+1, MPI_INT, ROOT, MPI_COMM_WORLD);
    }

    // Resultado final estara no root
    if (my_rank == ROOT)
        rfinal = data[W];

    free(result);
    free(data);
    return rfinal;
}

void input(int n, int *val, int *wt, int my_rank)
{
    if (my_rank == ROOT)
        for (int i = 0; i < n; ++i)
            int temp = scanf("%d %d", &(val[i]), &(wt[i]));
}

int main(int argc, char **argv)
{
    double time = MPI_Wtime();
    int my_rank, n_procs;
    int n, W;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    // Entradas do arquivo
    if (my_rank == ROOT)
        int temp = scanf("%d %d", &n, &W);
    MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&W, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    int *val = (int *)calloc(n, sizeof(int));
    int *wt = (int *)calloc(n, sizeof(int));
    input(n, val, wt, my_rank);
    MPI_Bcast(val, n, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(wt, n, MPI_INT, ROOT, MPI_COMM_WORLD);
    // -------------------

    // Calculo da divisao de tamanho para os processos
    int tamproc = W / n_procs + ((W % n_procs) > 0 ? n_procs : 0);

    // Calculo KnapSack
    int result = knapSack(n, W, val, wt, my_rank, tamproc, n_procs);

    MPI_Finalize();

    if (my_rank == ROOT){
        printf("\n\tResult: %d\n", result);
        printf("\n\tTime: %fs\n\n", MPI_Wtime() - time);
    }

    free(val);
    free(wt);

    return 0;
}
