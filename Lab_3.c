#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<limits.h>
#include<pthread.h>
#define LONG long long int


float *vetor; 
int nthreads;
LONG n, lim_inf, lim_sup;


/*
    Autor: Pedro Poppolino
    
    Tarefa:
    Contar, de forma concorrente e sequencial, quantos 
    elementos de um vetor possuem valor entre o limiar 
    inferior e superior passado.
*/



// Pega o tempo atual em segundos
double GET_TIME(){
    struct timespec tempo;
    
    if( clock_gettime(CLOCK_MONOTONIC, &tempo) ){
        printf("Erro durante a medição do tempo.\n\n");
        exit(3);
    }
    
    return tempo.tv_sec + tempo.tv_nsec/1000000000.0;
}




// Inicializa um vetor de floats com valores aleatórios
void inicializa(float vet[], LONG tam){
    static int semente = 0;


    /* Define uma vez a semente usada pelo gerador pseudo-aleatório */
    if(!semente){
        LONG tempo_seg = (LONG) GET_TIME();
        srand(tempo_seg % UINT_MAX);
        semente = 1;
    }   


    /* Preenche o vetor com valores reais aleatórios */
    for(LONG pos=0; pos < tam; pos++)
        vet[pos] = (rand() % 10000000) / 13.1;
}




// Realiza a contagem sequencial de elementos do vetor que tenham valores no intervalo dos limiares, e retorna o tempo gasto
double conta_sequencial(LONG *conta, float vet[], LONG tam, LONG lim_inf, LONG lim_sup){
    double inicio, fim;

    inicio = GET_TIME();


    *conta = 0;

    for(LONG pos=0; pos < tam; pos++){
        if(lim_inf < vet[pos] && vet[pos] < lim_sup)
            (*conta)++;
    }


    fim = GET_TIME();

    return fim - inicio;
}




// Função executada por cada thread que calcula quantos elementos passados do vetor estão na faixa de valores dos limiares
void *cont_val(void *info){
    int ini = *((int *) info);
    LONG *conta;


    /* Tenta alocar memória. Se houver erro, termina a execução */
    conta = (LONG *) malloc(sizeof(LONG));

    if(conta == (LONG *)NULL){
        printf("Erro na alocação de memória.\n\n");
        exit(2);
    }

    *conta = 0; 

    /* Contabiliza os elementos na faixa dos limiares */
    for(LONG pos=ini; pos < n; pos += nthreads){
         if(lim_inf < vetor[pos] && vetor[pos] < lim_sup)
            (*conta)++;   
    }

    /* Retorna a conta */
    pthread_exit((void *) conta);
}




// Realiza a contagem concorrente de elementos do vetor que tenham valores no intervalo dos limiares, e retorna o tempo gasto
double conta_concorrente(LONG *conta, int nthreads){
    pthread_t *tid;
    double inicio, fim;
    int erro, *info_thread;
    LONG *retorno;
    

    /* Aloca memória para os vetores auxiliares */
    tid = (pthread_t *) malloc(nthreads * sizeof(pthread_t));
    info_thread = (int *) malloc(nthreads * sizeof(int));

    /* Se houve erro na alocação de memória, termina a execução */
    if(tid == (pthread_t *)NULL || info_thread == (int *)NULL){
        printf("Erro na alocação de memória.\n\n");
        exit(2);
    }


    inicio = GET_TIME();

    /* Cria as threads */
    for(int i=0; i < nthreads; i++){
        info_thread[i] = i;

        erro = pthread_create(&tid[i], NULL, cont_val, (void *) &info_thread[i]);

        if(erro){
            printf("Erro na criação das threads.\n\n");
            exit(erro);
        }
    }


    *conta = 0;

    /* Aguarda todas as threads serem concluídas, somando o valor contado */
    for(int thread=0; thread < nthreads; thread++){
        erro = pthread_join(tid[thread], (void **) &retorno);

        if(erro){
            printf("Erro na espera das threads.\n\n");
            exit(erro); 
        }

        *conta += *retorno; 
        free(retorno);
    }


    fim = GET_TIME();


    /* Libera a memória utilizada */
    free(tid);
    free(info_thread);

    return fim - inicio;
}





int main(int argc, char *argv[]){
    LONG conta_seq, conta_conc;
    double tempo_seq, tempo_conc;


    /* Encerra o programa se tiver sido executado com pouca informação */
    if(argc < 5){
        printf("Escreva o padrão: %s <Dimensão do Vetor> <Número de Threads> <Limiar Inferior> <Limiar Superior>\n\n", argv[0]);
        return 1;
    }

    /* Converte os dados de entrada para valores inteiros */
    n = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    lim_inf = atoll(argv[3]);
    lim_sup = atoll(argv[4]);


    /* Encerra o programa caso tenha passado valores negativos, nulos ou não-numéricos */
    if(nthreads<=0 || n<=0){
        printf("Entrada inválida. Por favor, entre com tamanho do vetor e número de threads maiores que zero.\n\n");
        return 1;
    }


    /* Termina o programa caso tenha se passado um limiar inferior maior ou igual ao superior */
    if(lim_inf >= lim_sup){
        printf("Entrada inválida. O limiar inferior deve ser menor que o superior.\n\n");
        return 1;
    }


    /* Evita criar mais threads que posições do vetor */
    if(nthreads > n)
        nthreads = n;


    /* Aloca memória para o vetor do problema */
    vetor = (float *) malloc(n * sizeof(float));

    /* Se houve erro na alocação de memória, termina a execução */
    if(vetor == (float *)NULL){
        printf("Erro na alocação de memória.\n\n");
        return 2;
    }


    /* Inicializa o vetor de floats com valores aleatórios */
    inicializa(vetor, n);


    /* Conta quantos elementos satisfazem a condição de forma sequencial e concorrente */
    tempo_seq = conta_sequencial(&conta_seq, vetor, n, lim_inf, lim_sup);
    tempo_conc = conta_concorrente(&conta_conc, nthreads);


    /* Verifica se o cálculo foi feito adequadamente */
    if(conta_seq == conta_conc)
        printf("A conta está correta! :)\n\n");
    else
        printf("A conta está errada.. :(\n\n");


    /* Escreve o tempo gasto para o algoritmo sequencial e para o concorrente */
    printf("Tempo do Algoritmo Sequencial: %lf\n", tempo_seq);
    printf("Tempo do Algoritmo Concorrente: %lf\n\n", tempo_conc);


    /* Libera a memória alocada */
    free(vetor);

    return 0;
}