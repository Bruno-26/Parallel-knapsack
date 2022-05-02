#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int knapSack(int W, int wt[], int val[], int n)
{
    int rfinal;
    int *aux;
    int *result = (int *)malloc((W + 1) * sizeof(int));
    int *data = (int *)malloc((W + 1) * sizeof(int));
#pragma omp simd
    for (int i = 0; i < W + 1; i++)
    {
        data[i] = 0;
        result[i] = 0;
    }
    long int i, w;
    for (i = 0; i <= n; i++)
    {
#pragma omp parallel for schedule(static)
        for (w = 0; w <= W; w++)
        {
            if (wt[i - 1] <= w)
                result[w] = max(val[i - 1] + data[w - wt[i - 1]], data[w]);
            else
                result[w] = data[w];
        }
        aux = data;
        data = result;
        result = aux;
    }
    rfinal = result[W];
    free(result);
    free(data);
    return rfinal;
}

int main()
{
    double time = omp_get_wtime();
    int n, W;
    int temp = scanf("%d %d", &n, &W);
    int *val = (int *)calloc(n, sizeof(int));
    int *wt = (int *)calloc(n, sizeof(int));
    int i;
    for (i = 0; i < n; ++i)
        temp = scanf("%d %d", &(val[i]), &(wt[i]));

    printf("\n\tResult: %d\n", knapSack(W, wt, val, n));
    printf("\n Time: %fs\n", omp_get_wtime() - time);
    return 0;
}
