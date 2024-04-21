/* Programa que, dada uma sequencia consecutiva de numeros naturais de 1 a N, 
encontra (concorrentemente) a quantidade total de numeros primos na sequencia */

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<math.h>
#include<pthread.h>

// comentar o "define" abaixo caso não queira imprimir todas as informações do resultado
#define PRINT_COMPLETO

long long int N;  // numero de elementos da sequencia consecutiva
long long int i_global = 1; // variavel compartilhada entre as threads
pthread_mutex_t mutex; // variavel de lock para exclusao mutua

// funcao que verifica a primalidade de um numero
int ehPrimo(long long int n) {
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for(int i=3; i<sqrt(n)+1; i+=2)
		if(n%i==0) return 0;
	return 1;
}

// funcao executada pelas threads
void *conta_primos(void *arg) {
    long long int i_local = 0, *total;
    total = (long long int *) malloc(sizeof(long long int));
    if(total == NULL) {fprintf(stderr, "\nERRO -- malloc()\n");}
    
    *total = 0;
    while(i_local <= N) {
        pthread_mutex_lock(&mutex); // entrada na SC
        i_local = i_global;  // acessa variavel compartilhada
        i_global++; 
        pthread_mutex_unlock(&mutex); // saida da SC

        if(ehPrimo(i_local)) {(*total)++;}
        i_local++;
    }
    pthread_exit((void *) total);
}

int main(int argc, char *argv[]) {
    int nThreads;
    long long int *retorno, contador = 0;
    pthread_t *tid;
    struct timeval inicio, fim;
    double delta;

    // leitura e avaliacao dos parametros de entrada
    if(argc < 3) {
        printf("\nDigite %s <Numero de elementos> <Numero de threads>\n\n", argv[0]);
        return 1;
    }
    N = atoll(argv[1]);
    nThreads = atoi(argv[2]);
    if(nThreads > N) nThreads = N; // evita criar mais threads que o necessário

    // alocacao das estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nThreads);
    if(tid == NULL) {fprintf(stderr, "\nERRO -- malloc()\n"); return 1;}

    // PROCESSAMENTO
    {
    gettimeofday(&inicio, NULL);
    pthread_mutex_init(&mutex, NULL);  // inicilaiza o mutex (lock de exclusao mutua)

    // criacao das threads
    for(int i=0; i<nThreads; i++) {
        if(pthread_create(tid+i, NULL, conta_primos, NULL)) {
            fprintf(stderr, "\nERRO -- pthread_create()\n"); return 2;
        }
    }

    // espera pelo termino das threads
    for(int i=0; i<nThreads; i++) {
        if(pthread_join(*(tid+i), (void **) &retorno)) {
            fprintf(stderr, "\nERRO -- pthread_join()\n"); return 3;
        }
        contador += *retorno;
        //printf("%ld ", *retorno); // printa o retorno de cada thread
    }
    
    gettimeofday(&fim, NULL);
    delta = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    }

    // liberacao da memoria 
    pthread_mutex_destroy(&mutex);
    free(tid);
    free(retorno);

// printa todas as informações do resultado obtido
#ifdef PRINT_COMPLETO  
    printf("\nNumero de threads: %d\n", nThreads);
    printf("Numero de elementos: %lld\n", N);
    printf("Tempo de PROCESSAMENTO: %lf\n", delta);
    printf("Numero de primos encontrados: %lld\n\n", contador);
#endif

#ifndef PRINT_COMPLETO
    printf("\nNumero de primos encontrados: %lld\n\n", contador);
#endif
    
    return 0;
}