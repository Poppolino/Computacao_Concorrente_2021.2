#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<limits.h>
#define LONG long long int



int n, cont = 0, *vetor;
pthread_mutex_t tranca;
pthread_cond_t cond_bar;



    /*
        Autor: Pedro Poppolino
        
        Atividade:
        Cada uma das N threads precisa somar todas as N posições do vetor 
        em cada uma das N iterações.
        Sendo que, entre as iterações, cada thread faz uma posição distinta 
        do vetor receber um valor aleatório.
    */



// Gera um número aleatório dentro do intervalo definido pelo início e fim
int num_aleatorio(int ini, int fim){
    static int semente = 0;


    /* Define uma vez a semente usada pelo gerador pseudo-aleatório */
    if(!semente){
        LONG tempo_seg = (LONG) time(NULL);
        srand(tempo_seg % UINT_MAX);
        semente = 1;
    }

    return ini + (rand() % (fim-ini+1));     
}



// Define uma sincronização coletiva entre as threads usando barreira
void barreira(){
    pthread_mutex_lock(&tranca);

    cont++;
    if(cont == n){
        cont = 0;
        pthread_cond_broadcast(&cond_bar);
    }
    else
        pthread_cond_wait(&cond_bar, &tranca);

    pthread_mutex_unlock(&tranca);
}



// Realiza, para cada thread, o somatório de todas as posições do vetor em cada iteração
void *soma_total(void *arg){
    int id = *((int *) arg);


    /* 
        Tenta alocar memória. Caso consiga, inicializa a variável. 
        Caso não consiga, retorna erro.
    */
    LONG *total = (LONG *) malloc(sizeof(LONG));

    if(total == (LONG *)NULL){
        printf("Erro na alocação de memória.\n\n");
        exit(2);
    }

    *total = 0;


    /* 
        Realiza o somatório do vetor, espera as demais threads somarem, 
        muda uma posição do vetor e espera as threads alterarem.
    */
    for(int i=0; i < n; i++){
        for(int pos=0; pos < n; pos++)
            *total += vetor[pos];       

        barreira();
        vetor[id] = num_aleatorio(0,9);
        barreira();
    }


    /* Retorna a soma acumulada total */
    pthread_exit((void *) total);
}





int main(int argc, char *argv[]){
    int erro, valores_iguais, *id;
    pthread_t *tid;
    LONG valor, *soma;


    /* Encerra o programa se tiver sido executado com pouca informação */
    if(argc < 2){
        printf("Escreva o padrão: %s <Número de Threads>\n\n", argv[0]);
        return 1;
    }

    /* Converte o dado de entrada para um valor inteiro */
    n = atoi(argv[1]);


    /* Encerra o programa caso tenha passado valor negativo, nulo ou não-numérico */
    if(n <= 0){
        printf("Entrada inválida. Por favor, entre com o número de threads maior que zero.\n\n");
        return 1;
    }


    /* Realiza a alocação de memória das estruturas utilizadas e retorna erro caso não seja possível alocar */
    vetor = (int *) malloc(n * sizeof(int));
    id = (int *) malloc(n * sizeof(int));
    tid = (pthread_t *) malloc(n * sizeof(pthread_t));


    if(vetor == (int *)NULL || id == (int *)NULL || tid == (pthread_t *)NULL){
        printf("Erro na alocação de memória.\n\n");
        return 2;
    }


    /* Inicializa a variável de exclusão mútua e a de condição da barreira */
    pthread_mutex_init(&tranca, NULL);
    pthread_cond_init(&cond_bar, NULL);


    /* Preenche o vetor com valores inteiros aleatórios de 0 a 9 */
    for(int pos=0; pos < n; pos++)
        vetor[pos] = num_aleatorio(0,9);



    /* Cria todas as threads e retorna erro caso não seja possível */
    for(int thread=0; thread < n; thread++){
        id[thread] = thread;

        erro = pthread_create(&tid[thread], NULL, soma_total, (void *) &id[thread]);

        if(erro){
            printf("Erro na criação das threads.\n");
            return erro;
        }
    }



    /* Aguarda as threads terminarem para poder escrever os somatórios recebidos. Se der algum problema, retorna erro. */
    for(int thread=0; thread < n; thread++){
        erro = pthread_join(tid[thread], (void **) &soma);

        if(erro){
            printf("Erro na espera das threads.\n");
            return erro;
        }

        printf("Soma da Thread %d: %lld\n", id[thread], *soma);


        /* Verifica se os somatórios são iguais e libera a memória utilzada */
        if(thread == 0){
            valor = *soma;
            valores_iguais = 1;
        }
        else if(valor != *soma)
            valores_iguais = 0; 

        free(soma);
    }  



    /* Indica se o cálculo foi feito adequadamente */
    if(valores_iguais)
        printf("\nTodos os somatórios deram o mesmo resultado! :)\n\n");
    else
        printf("\nCertos somatórios deram resultados diferentes... :(\n\n");


    /* Libera a memória utilizada */  
    free(id);
    free(tid);
    free(vetor);
    pthread_mutex_destroy(&tranca);
    pthread_cond_destroy(&cond_bar);

    return 0;
}