/* Programa concorrente onde UMA thread PRODUTORA carrega de um arquivo binario 
uma sequencia de N numeros inteiros e os deposita, um de cada vez, em um buffer 
de tamanho M que sera compartilhado com threads CONSUMIDORAS. AS threads 
CONSUMIDORAS retiram os numeros e avaliam sua primalidade. 
 * Entrada: quantidade de threads consumidoras, tamanho do buffer e nome do arquivo de entrada
 * Saida: quantidade total de numeros primos encontrados e a thread consumidora que encontrou a maior
 * quantidade de primos.  */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include<semaphore.h>

// comentar o "define" abaixo caso nao queira imprimir os dados da execucao
#define IMPRIME_DADOS

// variaveis globais
sem_t slotCheio, slotVazio; // condicao
sem_t mutexCons; // exclusao mutua
long int N; // quantidade de valores na sequencia
long int consumidos = 0; // numero total de itens consumidos 
int M; // tamanho do buffer
int *buffer; // buffer que sera compartilhado com threads consumidoras
int *sequencia; // sequencia de inteiros recebida do arquivo
long int *contagemPrimos; // guarda o numero de primos por thread

void Insere(int item);
int Retira(void);

// funcao que verifica a primalidade de um numero
int ehPrimo(long long int n) {
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for(int i=3; i<sqrt(n)+1; i+=2)
		if(n%i==0) return 0;
	return 1;
}

void *produtor(void *arg) {
    int item;
    for(long int i=0; i < N; i++) {
        item = sequencia[i];
        Insere(item);
    }
    pthread_exit(NULL);
}

void Insere(int item) {
    static int in = 0; // posicao do produtor no buffer

    sem_wait(&slotVazio); // aguarda slot vazio
    buffer[in] = item;
    in = (in + 1) % M; 
    sem_post(&slotCheio); // sinaliza um slot cheio 
    //print("%d produzido<-------------------------\n", item);    
}

void *consumidor(void *arg) {
    int item;
    long int contadorPrimos = 0;
    int id = *(int *) arg;

    while(1) {
        item = Retira();
        sem_wait(&mutexCons);
        ++consumidos;
        if(consumidos >= N) {  // todos os itens foram consumidos
            sem_post(&mutexCons);
            sem_post(&slotCheio);
            break;
        }
        sem_post(&mutexCons);
        
        if(ehPrimo(item)) contadorPrimos++; // mede quantidade de primos encontrada por thread
        //printf("\n------------------------->%d consumido\n", item);
    }
    contagemPrimos[id] = contadorPrimos;
    pthread_exit(NULL);
}

int Retira(void) {
    int item;
    static int out = 0; // posicao do consumidor no buffer

    sem_wait(&slotCheio); // aguarda slot cheio
    sem_wait(&mutexCons); // exclusao mutua entre consumidores
    item = buffer[out];
    out = (out + 1) % M;
    sem_post(&mutexCons);
    sem_post(&slotVazio); // sinaliza slot vazio
    return item;
}


int main(int argc, char *argv[]) {
    int nthreads; // numero de threads consumidoras a serem criadas
    int contadorPrimos = 0; // quantidade de primos retornada por cada thread
    int maior=-1, posicao; // maior quantidade de primos encontrada e sua posicao
    long int qtdePrimos, totalPrimos = 0; // quantidades correta e encontrada de primos na sequencia, respectivamente
    FILE *descritorArquivo; // descritor do arquivo
    size_t ret; // retorno da funcao no arquivo 
    pthread_t tidProd; // identificador da thread produtora
    pthread_t *tidCons; // identificadores das threads consumidoras no sistema
    int *thread_ids; // IDs das threads consumidoras


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

    // aloca memória para os IDs das threads consumidoras
    thread_ids = (int *) malloc(sizeof(int) * nthreads);
    if (thread_ids == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 2;}

    // aloca memória para a contagem de primos
    contagemPrimos = (long int *) malloc(sizeof(long int) * nthreads);
    if (contagemPrimos == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 2;}


    // le a sequencia do arquivo
    ret = fread(sequencia, sizeof(int), N, descritorArquivo);
    if(ret < N) {fprintf(stderr, "\nERRO -- leitura da sequencia\n"); return 4;}}

    // le o a quantidade correta de primos do arquivo
    ret = fread(&qtdePrimos, sizeof(long int), 1, descritorArquivo);
    if(!ret) {fprintf(stderr, "\nERRO -- leitura da qtde de primos\n"); return 4;}
    
    // inicializa semaforos
    sem_init(&slotVazio, 0, M);
    sem_init(&slotCheio, 0, 0);
    sem_init(&mutexCons, 0, 1);


#ifdef IMPRIME_DADOS
    printf("\nTamanho da sequencia (N): %ld\n", N);
    printf("Numero de threads consumidoras: %d\n", nthreads);
    printf("Tamanho do buffer (M): %d\n", M);
#endif 
    printf("\nQuantidade total correta de primos: %ld\n", qtdePrimos);
    }


    // PROCESSAMENTO   
    {

    // criacao da thread produtora e threads consumidoras
    if(pthread_create(&tidProd, NULL, produtor, NULL)) {fprintf(stderr, "\nERRO -- pthread_create()\n"); return 5;}
    for(int i=0; i<nthreads; i++) {
        thread_ids[i] = i;
        if(pthread_create(tidCons+i, NULL, consumidor, thread_ids+i)) {fprintf(stderr, "\nERRO -- pthread_create()\n"); return 5;}
    }

    // imprime tabela de qtde de primos encontrados por cada thread
    printf("\n---------------------------");
    printf("\n| Thread | Qtde de primos |\n");
    printf("---------------------------");

    // espera pelo termino das threads
    if(pthread_join(tidProd, NULL)) {fprintf(stderr, "\nERRO -- pthread_join()\n"); return 6;}
    for(int i=0; i<nthreads; i++) {
        if(pthread_join(*(tidCons+i), NULL)) {fprintf(stderr, "\nERRO -- pthread_join()\n"); return 6;}
       
        contadorPrimos = contagemPrimos[i];
        totalPrimos += contadorPrimos; // obtem a quantidade total de primos na sequencia
        if(contadorPrimos > maior) { // encontra a thread vencedora
            maior = contadorPrimos;
            posicao = i;
        }
        printf("\n|   %d    |       %d        |", i, contadorPrimos); // imprime conteudo da tabela
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
    free(sequencia);
    free(buffer);
    free(tidCons);
    free(thread_ids);
    free(contagemPrimos);

    return 0;
    }
}