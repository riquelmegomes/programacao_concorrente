/*
Programa concorrente para somar os elementos de um vetor de floats, 
considerando que a entrada será lida dos arquivos gerados.
*/

#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>

#define TESTE

float *vetor; //vetor de entrada

//define o tipo de dado de entrada das threads
typedef struct {
   int id; //id da thread
   long int tamBloco; //tamanho do bloco (cada thread processa um bloco)
   float somaLocal; //soma de alguns elementos do vetor
} tArgs;


//fluxo das threads
void *somaVetor(void * arg) {
   tArgs *args = (tArgs *) arg; 
   long int ini = args->id * args->tamBloco; //elemento inicial do bloco da thread
   long int fim = ini + args->tamBloco; //elemento final(nao processado) do bloco da thread

   for(long int i=ini; i<fim; i++) {
      args->somaLocal += vetor[i];
   }   

   pthread_exit((void *) args);  
}


//fluxo principal
int main(int argc, char *argv[]) {
   float somaGeral = 0; //resultado encontrado pela solucao concorrente
   long int dim; //dimensao do vetor de entrada
   long int tamBloco; //tamanho do bloco de cada thread 
   int nthreads; //numero de threads que serao criadas
   int resto; // resto da divisao de dim por nthreads
   pthread_t *tid; //vetor de identificadores das threads no sistema
   tArgs *retorno; //valor de retorno das threads

   //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   if(argc < 2) {
       fprintf(stderr, "Digite: %s <numero de threads>\n", argv[0]);
       return 1; 
   }
   nthreads = atoi(argv[1]);
   printf("\nNumero de threads = %d\n", nthreads); 

   //carrega o vetor de entrada
   scanf("%ld", &dim); //primeiro pergunta a dimensao do vetor
   printf("Numero de elementos do vetor = %ld", dim); 

   //aloca o vetor de entrada
   vetor = (float*) malloc(sizeof(float)*dim);
   if(vetor == NULL) {
      fprintf(stderr, "\nERRO--malloc\n");
      return 2;
   }

   //preenche o vetor de entrada
   for(long int i=0; i<dim; i++) {
      scanf("%f", &vetor[i]);
   }


   tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
   if(tid == NULL) {
      fprintf(stderr, "\nERRO--malloc\n");
      return 3;
   }

   //calcula o tamanho do bloco de cada thread
   tamBloco = dim/nthreads;  
   if(!tamBloco) {
      printf("\nA quantidade de threads eh maior que a quantidade de elementos, a execucao sera sequencial!");
   }

#ifdef TESTE
    float somaSequencial; //valor de referencia para os testes
    scanf("%f", &somaSequencial);

   // printa o vetor de floats
    printf("\n\n"); 
    for(long int i=0; i<dim; i++) {
      printf("%f ", vetor[i]); 
    }
    printf("\n\nSomatorio sequencial: %f\n", somaSequencial);
#endif  

   //cria as threads
   for(int i=0; i<nthreads; i++) {
       //aloca e preenche argumentos para thread
       tArgs *args = (tArgs*) malloc(sizeof(tArgs));
       if (args == NULL) {
          printf("\n--ERRO: malloc()\n"); 
          pthread_exit(NULL);
       }
       args->id = i; 
       args->tamBloco = tamBloco; 
       args->somaLocal = 0;

       if(pthread_create(tid+i, NULL, somaVetor, (void*) args)){
        fprintf(stderr, "\nERRO--pthread_create\n");
          return 5;
       }
   }

   //processa a parte restante do vetor na main
   resto = dim%nthreads;
   if(resto) {
      for(long int i=dim-(resto); i<dim; i++) {
         somaGeral += vetor[i];
      }
   } 

   //aguarda o termino das threads
   for(int i=0; i<nthreads; i++) {
      retorno = (tArgs*) malloc(sizeof(tArgs));
      if(retorno == NULL) {
         printf("\n--ERRO: malloc()\n"); 
         pthread_exit(NULL);
      }

      if(pthread_join(*(tid+i), (void**) &retorno)){
         fprintf(stderr, "\nERRO--pthread_join\n");
         return 6;
      }
      somaGeral += retorno->somaLocal;
   }

   //exibe o valor do somatorio encontrado de forma concorrente
   printf("Somatorio encontrado: %f\n", somaGeral);

   //libera as areas de memoria alocadas
   free(vetor);
   free(tid);
   free(retorno);

   return 0;
}