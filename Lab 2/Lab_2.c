#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<limits.h>
#include<pthread.h>
#define LONG long long int


typedef struct{
    int id, num_thr, dim_mat; 
} INFO_T;


int *mat_A, *mat_B, *mat_res; 



// Pega o tempo atual em segundos
double GET_TIME(){
    struct timespec tempo;
    
    if( clock_gettime(CLOCK_MONOTONIC, &tempo) ){
        printf("Erro durante a medição do tempo.\n\n");
        exit(3);
    }
    
    return tempo.tv_sec + tempo.tv_nsec/1000000000.0;
}



// Inicializa uma matriz quadrada com valores inteiros aleatórios
void inicializa(int mat[], int dim){
    static int semente = 0;
    int pos;


    /* Define uma vez a semente usada pelo gerador pseudo-aleatório */
    if(!semente){
        LONG tempo_seg = (LONG) GET_TIME();
        srand(tempo_seg % UINT_MAX);
        semente = 1;
    }   


    /* Preenche a matriz com valores aleatórios de 0 a 9999 */
    for(int lin=0; lin < dim; lin++){
        for(int col=0; col < dim; col++){
            pos = lin*dim + col;
            mat[pos] = rand() % 10000;
        }
    }
}




// Calcula a multiplicação de duas matrizes apenas nas linhas que resultam dos passos a partir de um início especificado
void multiplica_matrizes(int mat_A[], int mat_B[], int result[], int dim, int ini, int passo){
    int pos_A, pos_B, pos_res;


    for(int lin = ini; lin < dim; lin += passo){
        for(int col=0; col < dim; col++){
            pos_res = lin*dim + col;
            result[pos_res] = 0;
            
            for(int aux=0; aux < dim; aux++){
                pos_A = lin*dim + aux;
                pos_B = aux*dim + col;
                result[pos_res] += mat_A[pos_A] * mat_B[pos_B];
            }
        }
    }
}




// Realiza a multiplicação de matrizes quadradas de forma sequencial e retorna o tempo gasto
double multiplica_sequencial(int A[], int B[], int res[], int dim){
    double inicio, fim;

    inicio = GET_TIME();
    multiplica_matrizes(A, B, res, dim, 0, 1);  
    fim = GET_TIME();

    return fim - inicio;
}




// Função executada por cada thread que calcula a multiplicação de matrizes para certas linhas da matriz resultante
void *mult_mat(void *info){
    int id, num_thr, dim_mat;

    id = ((INFO_T *) info)->id;
    num_thr = ((INFO_T *) info)->num_thr;
    dim_mat = ((INFO_T *) info)->dim_mat;


    multiplica_matrizes(mat_A, mat_B, mat_res, dim_mat, id, num_thr);

    pthread_exit(NULL);
}




// Realiza a multiplicação de matrizes quadradas de forma concorrente e retorna o tempo gasto
double multiplica_concorrente(int dim_mat, int num_thr){
    int erro;
    pthread_t *tid;
    INFO_T *info_thread;
    double inicio, fim;
    

    /* Aloca memória para os vetores auxiliares */
    tid = (pthread_t *) malloc(num_thr * sizeof(pthread_t));
    info_thread = (INFO_T *) malloc(num_thr * sizeof(INFO_T));

    /* Se houve erro na alocação de memória, termina a execução */
    if(tid == (pthread_t *)NULL || info_thread == (INFO_T *)NULL){
        printf("Erro na alocação de memória.\n\n");
        exit(2);
    }


    inicio = GET_TIME();

    /* Cria as threads */
    for(int i=0; i < num_thr; i++){
        info_thread[i].id = i;
        info_thread[i].num_thr = num_thr;
        info_thread[i].dim_mat = dim_mat;

        erro = pthread_create(&tid[i], NULL, mult_mat, (void *) &info_thread[i]);

        if(erro){
            printf("Erro na criação das threads.\n\n");
            exit(erro);
        }
    }


    /* Aguarda todas as threads serem concluídas */
    for(int thread=0; thread < num_thr; thread++){
        erro = pthread_join(tid[thread], NULL);

        if(erro){
            printf("Erro na espera das threads.\n\n");
            exit(erro); 
        }
    }

    fim = GET_TIME();


    /* Libera a memória utilizada */
    free(tid);
    free(info_thread);

    return fim - inicio;
}




// Verifica se duas matrizes quadradas são iguais 
int igualdade(int mat_A[], int mat_B[], int dim){
    int pos;

    for(int lin=0; lin < dim; lin++){
        for(int col=0; col < dim; col++){
            pos = lin*dim + col;
            
            if(mat_A[pos] != mat_B[pos])
                return 0; 
        }
    }

    return 1;
}




// Escreve os elementos de uma matriz quadrada na tela
void escreve_matriz(int matriz[], int dim, char *nome){
    int pos;
    
    printf("Elementos da Matriz %s:\n", nome);


    for(int lin=0; lin < dim; lin++){
        for(int col=0; col < dim; col++){
            pos = lin*dim + col;
            printf("%d ", matriz[pos]);
        }

        printf("\n");
    }


    printf("\n");
}





int main(int argc, char *argv[]){
    int num_thr, dim_mat, *mat_seq;
    double tempo_seq, tempo_conc;


    /* Encerra o programa se tiver sido executado com pouca informação */
    if(argc < 3){
        printf("Escreva o padrão: %s <Dimensão da Matriz> <Número de Threads>\n\n", argv[0]);
        return 1;
    }

    /* Converte os dados de entrada para valores inteiros */
    dim_mat = atoi(argv[1]);
    num_thr = atoi(argv[2]);


    /* Encerra o programa caso tenha passado valores negativos, nulos ou não-numéricos */
    if(num_thr<=0 || dim_mat<=0){
        printf("Entrada inválida. Por favor, entre com números inteiros maiores que zero.\n\n");
        return 1;
    }


    /* Evita criar mais threads que linhas da matriz */
    if(num_thr > dim_mat)
        num_thr = dim_mat;


    /* Aloca memória para as matrizes do problema */
    mat_A = (int *) malloc(dim_mat * dim_mat * sizeof(int));
    mat_B = (int *) malloc(dim_mat * dim_mat * sizeof(int));
    mat_res = (int *) malloc(dim_mat * dim_mat * sizeof(int));
    mat_seq = (int *) malloc(dim_mat * dim_mat * sizeof(int));

    /* Se houve erro na alocação de memória, termina a execução */
    if(mat_A == (int *)NULL || mat_B == (int *)NULL || mat_res == (int *)NULL || mat_seq == (int *)NULL){
        printf("Erro na alocação de memória.\n\n");
        return 2;
    }


    /* Inicializa as matrizes com valores aleatórios */
    inicializa(mat_A, dim_mat);
    inicializa(mat_B, dim_mat);


    /* Calcula a multiplicação de matrizes de modo sequencial e concorrente */
    tempo_seq = multiplica_sequencial(mat_A, mat_B, mat_seq, dim_mat);
    tempo_conc = multiplica_concorrente(dim_mat, num_thr);


    /* Verifica se o cálculo foi feito adequadamente */
    if(igualdade(mat_res, mat_seq, dim_mat)){
        printf("As matrizes foram calculadas corretamente! :)\n\n");
    }
    else{
        printf("As matrizes não foram calculadas corretamente.. :(\n\n");
    }


    /* Escreve o tempo gasto para o algoritmo sequencial e para o concorrente */
    printf("Tempo do Algoritmo Sequencial: %lf\n", tempo_seq);
    printf("Tempo do Algoritmo Concorrente: %lf\n\n", tempo_conc);


    /* Escreve as matrizes na tela */
    //escreve_matriz(mat_A, dim_mat, "A");
    //escreve_matriz(mat_B, dim_mat, "B");
    //escreve_matriz(mat_seq, dim_mat, "resultante sequencial");
    //escreve_matriz(mat_res, dim_mat, "resultante concorrente");


    /* Libera a memória alocada */
    free(mat_A);
    free(mat_B);
    free(mat_res);
    free(mat_seq);

    return 0;
}
