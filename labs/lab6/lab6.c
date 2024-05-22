/* Programa concorrente onde UMA thread PRODUTORA carrega de um arquivo binario 
uma sequencia de N numeros inteiros e os deposita, um de cada vez, em um buffer 
de tamanho M que sera compartilhado com threads CONSUMIDORAS. AS threads 
CONSUMIDORAS retiram os numeros e avaliam sua primalidade. 
 * Entrada: quantidade de threads consumidoras, tamanho M do buffer e nome do arquivo de entrada
 * Saida: arquivo binario contendo a sequencia de valores  */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include<semaphore.h>

// comentar o "define" abaixo caso nao queira imprimir os dados da execucao
#define IMPRIME_DADOS

// variaveis globais
sem_t slotCheio, slotVazio; // condicao
sem_t mutex; // exclusao mutua
long int N; // quantidade de valores na sequencia
long int consumidos = 0; // numero total de itens consumidos 
int M; // tamanho do buffer
int *buffer; // buffer que sera compartilhado com threads consumidoras
int *sequencia; // sequencia de inteiros recebida do arquivo

// funcao que verifica a primalidade de um numero
int ehPrimo(long long int n) {
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for(int i=3; i<sqrt(n)+1; i+=2)
		if(n%i==0) return 0;
	return 1;
}

// 
void *produtor(void *arg) {
    static int in = 0; // posicao do produtor no buffer

    for(long int i=0; i < N; i++) {
        sem_wait(&slotVazio); // aguarda slot vazio
        buffer[in] = sequencia[i];
        in = (in + 1) % M; 
        sem_post(&slotCheio); // sinaliza um slot cheio 
        //printf("%d produzido<-------------------------\n", sequencia[i]);
    }
    pthread_exit(NULL);
}

// 
void *consumidor(void *arg) {
    static int out = 0; // posicao do consumidor no buffer
    long int contadorPrimos = 0;
    int item = 0;

    while(1) {
        sem_wait(&mutex); 
        if(consumidos >= N-1) {  // todos os itens foram consumidos
            sem_post(&mutex);
            sem_post(&slotCheio); 
            break;
        }
        sem_post(&mutex);

        sem_wait(&slotCheio); // aguarda slot cheio
        sem_wait(&mutex);
        item = buffer[out];
        out = (out + 1) % M;
        consumidos++;
        sem_post(&mutex);
        sem_post(&slotVazio);

        //printf("\n------------------------->%d consumido\n", item);
        if(ehPrimo(item)) contadorPrimos++; // mede quantidade de primos encontrada por thread
    }
    pthread_exit((void *) contadorPrimos);
}

int main(int argc, char *argv[]) {
    int nthreads; // numero de threads consumidoras a serem criadas
    int contadorPrimos; // quantidade de primos retornada por cada thread
    int maior=-1, posicao; // maior quantidade de primos encontrada e sua posicao
    long int qtdePrimos, totalPrimos = 0; // quantidades correta e encontrada de primos na sequencia, respectivamente
    FILE *descritorArquivo; // descritor do arquivo
    size_t ret; // retorno da funcao no arquivo 
    pthread_t tidProd; // identificador da thread produtora
    pthread_t *tidCons; // identificadores das threads consumidoras


    // INICIALIZACAO 
    {

    // leitura e avaliacao dos parametros de entrada
    if(argc < 4) {
        printf("\nDigite %s <qtde de threads consumidoras> <tamanho do buffer> <nome do arquivo>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]); // le o numero de threads consumidoras
    M = atoi(argv[2]); // le tamanho do buffer (M)


     // abre o arquivo (leitura binaria)
    {descritorArquivo = fopen(argv[3], "rb");
    if(!descritorArquivo) {fprintf(stderr, "\nERRO -- abertura do arquivo\n"); return 3;}

    // le o tamanho da sequencia do arquivo
    ret = fread(&N, sizeof(long int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- leitura do numero de threads consumidoras\n"); return 4;}

    // aloca memoria para a sequencia de valores
    sequencia = (int *) malloc(sizeof(int) * N);
    if(sequencia == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 2;}

    // aloca memoria para o buffer
    buffer = (int *) malloc(sizeof(int) * M);
    if(buffer == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 2;}

    // aloca memoria para os ID das threads consumidoras
    tidCons = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tidCons == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 2;}

    // le a sequencia do arquivo
    ret = fread(sequencia, sizeof(int), N, descritorArquivo);
    if(ret < N) {fprintf(stderr, "\nERRO -- leitura da sequencia\n"); return 4;}}

    // le o a quantidade correta de primos do arquivo
    ret = fread(&qtdePrimos, sizeof(long int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- leitura da qtde de primos\n"); return 4;}
    
    // inicializa semaforos
    sem_init(&slotVazio, 0, M);
    sem_init(&slotCheio, 0, 0);
    sem_init(&mutex, 0, 1);


#ifdef IMPRIME_DADOS
    printf("\nTamanho da sequencia (N): %ld\n", N);
    printf("Numero de threads consumidoras: %d\n", nthreads);
    printf("Tamanho do buffer (M): %d\nSequencia: ", M);
#endif 
    printf("\nQuantidade total correta de primos: %ld\n", qtdePrimos);
    }


    // PROCESSAMENTO   
    {

    // criacao da thread produtora e threads consumidoras
    if(pthread_create(&tidProd, NULL, produtor, NULL)) {fprintf(stderr, "\nERRO -- pthread_create()\n"); return 5;}
    for(int i=0; i<nthreads; i++) {
        if(pthread_create(tidCons+i, NULL, consumidor, NULL)) {fprintf(stderr, "\nERRO -- pthread_create()\n"); return 5;}
    }

    // imprime tabela de qtde de primos encontrados por cada thread
    printf("\n---------------------------");
    printf("\n| Thread | Qtde de primos |\n");
    printf("---------------------------");

    // espera pelo termino das threads
    if(pthread_join(tidProd, NULL)) {fprintf(stderr, "\nERRO -- pthread_join()\n"); return 6;}
    for(int i=0; i<nthreads; i++) {
        if(pthread_join(*(tidCons+i), (void **) &contadorPrimos)) {fprintf(stderr, "\nERRO -- pthread_join()\n"); return 6;}
       
        totalPrimos += contadorPrimos; // obtem a quantidade total de primos na sequencia
        printf("\n|   %d    |       %d        |", i, contadorPrimos);
        if(contadorPrimos > maior) { // encontra a thread vencedora
            maior = contadorPrimos;
            posicao = i;
        }
    }
    }
     

    // FINALIZACAO
    {
    
    // imprime resultado final
    printf("\n---------------------------");
    printf("\n| TOTAL  |       %ld        |", totalPrimos);
    printf("\n---------------------------");
    printf("\n\nO resultado final esta ");
    (totalPrimos == qtdePrimos) ? printf("CORRETO!") : printf("ERRADO!");
    printf("\nA thread %d foi a VENCEDORA com %d primo(s) encontrado(s)!\n", posicao, maior);

    // liberacao da memoria 
    fclose(descritorArquivo);
    free(buffer);
    free(sequencia);
    free(tidCons);

    return 0;
    }
}