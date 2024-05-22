/* Programa que cria arquivo (binário) contendo uma sequencia de N valores inteiros positivos, gerados aleatoriamente 
 * Entrada: tamanho N da sequencia de valores e nome do arquivo de saida
 * Saida: arquivo binario contendo a sequencia de valores  */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

// descomentar o "define" abaixo caso deseje imprimir a sequencia gerada
//#define IMPRIME_SEQUENCIA

#define MAX_RANGE 1000 // limite de valor maximo para os inteiros aleatorios

// funcao que verifica a primalidade de um numero
int ehPrimo(long long int n) {
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for(int i=3; i<sqrt(n)+1; i+=2)
		if(n%i==0) return 0;
	return 1;
}

int main(int argc, char*argv[]) {
    long int N; // quantidade de valores na sequencia
    long int qtdePrimos = 0; // quantidade de primos na sequencia
    int valorInteiro; // valor aleatorio a entrar na sequencia
    FILE *descritorArquivo; // descritor do arquivo de saida
    size_t ret; // retorno da funcao de escrita no arquivo de saida
   
    // recebe os argumentos de entrada
    if(argc < 3) {fprintf(stderr, "Digite: %s <tamanho da sequencia> <nome do arquivo saida>\n", argv[0]); return 1;}
    N = atol(argv[1]); 

    // abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[2], "wb");
    if(!descritorArquivo) {fprintf(stderr, "\nERRO -- abertura do arquivo.\n"); return 2;}

    // escreve N no arquivo
    ret = fwrite(&N, sizeof(long int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- escrita de N no arquivo.\n"); return 3;}

    // imprime tamanho da sequencia N
    printf("\nTAMANHO DA SEQUENCIA (N): %ld\n", N);


#ifdef IMPRIME_SEQUENCIA
    printf("SEQUENCIA: ");
#endif    

    // escreve sequencia no arquivo
    srand(time(NULL));
    for(long int i=0; i<N; i++) {
        valorInteiro = (rand() % MAX_RANGE); // randomiza a sequencia de inteiros 
        if(ehPrimo(valorInteiro)) qtdePrimos++; // guarda quantidade de primos
        ret = fwrite(&valorInteiro, sizeof(int), 1, descritorArquivo); // escreve inteiro gerado no arquivo
        if(!ret) {fprintf(stderr, "\nERRO -- escrita do valor no arquivo.\n"); return 3;}

        #ifdef IMPRIME_SEQUENCIA // imprime a sequencia gerada na saida padrao
            printf("%d ", valorInteiro);
        #endif
    }

#ifdef IMPRIME_SEQUENCIA 
    puts("");
#endif 


    // escreve quantidade total de primos da sequencia no arquivo
    ret = fwrite(&qtdePrimos, sizeof(long int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- escrita de qtdePrimos no arquivo.\n"); return 3;}

    // imprime quantidade de primos presentes na sequencia
    printf("QUANTIDADE DE PRIMOS: %ld\n", qtdePrimos);

    // finaliza o uso das variaveis
    fclose(descritorArquivo);
    return 0;
}