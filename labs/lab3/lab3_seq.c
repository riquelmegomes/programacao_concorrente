/* Programa que faz multiplicacao de matrizes de modo sequencial
considerando que: A(lin, N) * B(N, col) = C(lin, col)  */

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

// descomentar o "define" abaixo caso deseje imprimir as matrizes envolvidas
//#define IMPRIME_MATRIZES

int main(int argc, char *argv[]) {
    float *matA, *matB; // matrizes de entrada
    float *matC; // matriz de saida
    int lin, N, n, col;  // dimensoes das matrizes
    long long int tamA, tamB, tamC; // quantidade de elementos nas matrizes
    FILE *descritorArquivo; // descritor do arquivo de saida
    size_t ret; // retorno da funcao de escrita no arquivo de saida
    struct timeval inicio, fim;
    double delta[3];


    // INICIALIZACAO 
    {
    gettimeofday(&inicio, NULL);

    // leitura e avaliacao dos parametros de entrada
    if(argc < 4) {
        printf("\nDigite %s <nome arquivo 1> <nome arquivo 2> <nome arquivo 3>\n\n", argv[0]);
        return 1;
    }

    // ARQUIVO 1  (leitura binaria)
    {descritorArquivo = fopen(argv[1], "rb");
    if(!descritorArquivo) {fprintf(stderr, "\nERRO -- abertura do arquivo 1\n"); return 2;}
    ret = fread(&lin, sizeof(int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- leitura do numero de linhas da matriz A\n"); return 3;}
    ret = fread(&N, sizeof(int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- leitura do numero de colunas da matriz A\n"); return 3;}
    
    // aloca memoria para a matriz A
    tamA = lin * N;
    matA = (float *) malloc(sizeof(float) * tamA);
    if(matA == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 4;}

    // carrega a matriz de elementos do arquivo 1
    ret = fread(matA, sizeof(float), tamA, descritorArquivo);
    if(ret < tamA) {fprintf(stderr, "\nERRO -- leitura dos elementos da matriz A\n"); return 5;}}

    // ARQUIVO 2  (leitura binaria)          
    {descritorArquivo = fopen(argv[2], "rb");
    if(!descritorArquivo) {fprintf(stderr, "\nERRO -- abertura do arquivo 2\n"); return 2;}
    ret = fread(&n, sizeof(int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- leitura do numero de linhas da matriz B\n"); return 3;}
    ret = fread(&col, sizeof(int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- leitura do numero de colunas da matriz B\n"); return 3;}

    // aloca memoria para a matriz B
    if(n != N) {fprintf(stderr, "\nERRO -- Essas matrizes nao podem ser multiplicadas nessa ordem\n"); return 7;}
    tamB = N * col;
    matB = (float *) malloc(sizeof(float) * tamB);
    if(matB == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 4;}

    // carrega a matriz de elementos do arquivo 2
    ret = fread(matB, sizeof(float), tamB, descritorArquivo);
    if(ret < tamB) {fprintf(stderr, "\nERRO -- leitura dos elementos da matriz B\n"); return 5;}}

    // aloca memoria para a matriz C (saida)
    {tamC = lin * col;
    matC = (float *) malloc(sizeof(float) * tamC);
    if(matC == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 4;}

    // inicializa matriz C com zeros
    for(int i=0; i<lin; i++) {
        for(int j=0; j<col; j++) {
            matC[i*col + j] = 0;
        }
    }}
    
    gettimeofday(&fim, NULL);
    delta[0] = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    }


    // PROCESSAMENTO
    {
    gettimeofday(&inicio, NULL);

    for(int i=0; i<lin; i++) { 
        for(int j=0; j<col; j++) {
            for(int k=0; k<N; k++) {
                matC[i*col + j] += matA[i*N + k] * matB[k*col + j]; // multiplicacao das matrizes
            }
        }
    }

    gettimeofday(&fim, NULL);
    delta[1] = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    }


    // FINALIZACAO
    {
    gettimeofday(&inicio, NULL);

    // abre o arquivo para escrita da matriz de saida (C)
    descritorArquivo = fopen(argv[3], "wb");
    if(!descritorArquivo) {fprintf(stderr, "\nERRO -- abertura do arquivo 3\n"); return 2;}

    // escreve numero de linhas e de colunas
    ret = fwrite(&lin, sizeof(int), 1, descritorArquivo);
    ret = fwrite(&col, sizeof(int), 1, descritorArquivo);

    // escreve os elementos da matriz C
    ret = fwrite(matC, sizeof(float), tamC, descritorArquivo);
    if(ret < tamC) {fprintf(stderr, "ERRO -- escrita no arquivo\n"); return 6;}
    

#ifdef IMPRIME_MATRIZES  // imprime as matrizes A, B e C na saida padrao
    puts("\nMatriz de entrada (A): ");
    for(int i=0; i<lin; i++) {
        for(int j=0; j<N; j++) {
            fprintf(stdout, "%f ", matA[i*N + j]);
        }
        puts("");
    }
    puts("\nMatriz de entrada (B): ");
    for(int i=0; i<N; i++) {
        for(int j=0; j<col; j++) {
            fprintf(stdout, "%f ", matB[i*col + j]);
        }
        puts("");
    }
    puts("\nMatriz de saida (C): ");
    for(int i=0; i<lin; i++) {
        for(int j=0; j<col; j++) {
            fprintf(stdout, "%f ", matC[i*col + j]);
        }
        puts("");
    }
    puts("");
#endif

    // liberacao da memoria 
    fclose(descritorArquivo);
    free(matA);
    free(matB);
    free(matC);
    
    gettimeofday(&fim, NULL);
    delta[2] = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    }

    // printa o tempo de execução de cada parte do programa
    printf("Tempo TOTAL:            %lf\n", delta[0]+ delta[1] + delta[2]);
    printf("Tempo de INICIALIZACAO: %lf\n", delta[0]);
    printf("Tempo de PROCESSAMENTO: %lf\n", delta[1]);
    printf("Tempo de FINALIZACAO:   %lf\n", delta[2]);

    return 0;
}