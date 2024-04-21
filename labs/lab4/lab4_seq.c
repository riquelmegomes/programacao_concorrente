/* Programa que, dada uma sequencia consecutiva de numeros naturais
de 1 a N, retorna a quantidade total de numeros primos na sequencia */

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<math.h>

#define PRINT_COMPLETO 

// funcao que verifica a primalidade de um numero
int ehPrimo(long long int n) {
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for(int i=3; i<sqrt(n)+1; i+=2)
		if(n%i==0) return 0;
	return 1;
}

int main(int argc, char *argv[]) {
    long long int N, contador = 0;
    struct timeval inicio, fim;
    double delta;

    // leitura e avaliacao dos parametros de entrada
    if(argc < 2) {
        printf("\nDigite %s <Numero de elementos>\n\n", argv[0]);
        return 1;
    }
    N = atoll(argv[1]);
    
    // PROCESSAMENTO
    {
    gettimeofday(&inicio, NULL);

    // Parte de processamento da sequência
    for(long long int i=1; i<=N; i++) {
        contador += ehPrimo(i);
    }
    
    gettimeofday(&fim, NULL);
    delta = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    }

// printa todas as informações do resultado obtido
#ifdef PRINT_COMPLETO  
    printf("\nNumero de elementos: %lld\n", N);
    printf("Tempo de PROCESSAMENTO: %lf\n", delta);
    printf("Numero de primos encontrados: %lld\n\n", contador);
#endif

#ifndef PRINT_COMPLETO
    printf("\nNumero de primos encontrados: %lld\n\n", contador);
#endif

    return 0;
}