#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

#define M_threads 4 // número de threads
#define N_elementos 15 // número de elementos do vetor

int vetor[N_elementos];  // vetor de N elementos do tipo inteiro

// cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int idThread, quoc;
} t_Args;

// carrega o vetor de inteiros com valores de 1 a N_elementos
void inicializar_vetor() {
    for(int i=0; i<N_elementos; i++) {
        vetor[i] = i+1;
    }
}

void printar_vetor(int tipo) {
    if(tipo == 0) printf("VETOR ORIGINAL: ");
    else printf("VETOR ALTERADO: ");

    for(int i=0; i<N_elementos; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

// verifica se o vetor resultante está correto (elevado ao quadrado)
void verificar_vetor() {
    for(int i=0; i<N_elementos; i++) {
        if(vetor[i] != (i+1)*(i+1)) {
            printf("O resultado final esta ERRADO!\n");
            return;
        }
    }
    printf("O resultado final esta CORRETO!\n\n");
}

// eleva ao quadrado cada elemento do vetor
void *quadrado(void *arg) {
    t_Args args = *(t_Args*) arg;
    
    int aux = 0; // controla a quantidade de elementos por thread
    int posicao = args.idThread * args.quoc;
    while(posicao < N_elementos && aux < args.quoc) {
        vetor[posicao] *= vetor[posicao]; 
        posicao++;
        aux++;
    }
    
    free(arg); //libera a memoria que foi alocada na main
    pthread_exit(NULL);
}


int main(void) {
    pthread_t tid_sistema[M_threads];
    t_Args *args; // recebe os argumentos para a thread
    long int quoc, M;

    quoc = (N_elementos / M_threads); // divide os N elementos entre as M threads
    if(N_elementos % M_threads != 0) quoc += 1;  // arredonda "pra cima" se não for divisivel

    // printf("QUOCIENTE (N / M): %ld\n", quoc);

    inicializar_vetor();
    printar_vetor(0);
    
    // não criar mais threads que o necessário
    if(M_threads > N_elementos) M = N_elementos; 
    else M = M_threads;
        
    for(int i=0; i<M; i++) {
        args = malloc(sizeof(t_Args));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        args->idThread = i; 
        args->quoc = quoc;
    
        if (pthread_create(&tid_sistema[i], NULL, quadrado, (void*) args)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
        
    // Espera todas as threads terminarem
    for (int i=0; i<M; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join()\n"); exit(-1); 
        } 
    }
    
    printar_vetor(1);
    verificar_vetor();
    
    pthread_exit(NULL);
}