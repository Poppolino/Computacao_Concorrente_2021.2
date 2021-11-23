#include<stdio.h>
#include<pthread.h>
#define TAM 10000


/*
    Autor: Pedro Poppolino
    
    Atividade: 
    Implementar um programa concorrente, com duas threads (além da thread principal), 
    para elevar ao quadrado cada elemento de um vetor de 10000 elementos.
*/


typedef struct{
    int ini, fim;
} INTERV;


int vetor[TAM];



/* Função que eleva ao quadrado cada elemento de um dado intervalo do vetor */
void *quad_vetor(void *intervalo){
    int ini, fim;

    ini = ((INTERV *) intervalo)->ini;
    fim = ((INTERV *) intervalo)->fim;

    for(int pos=ini; pos < fim; pos++)
        vetor[pos] *= vetor[pos];

    pthread_exit(NULL);
}



/* Função que verifica se os valores finais do vetor estão corretos */
int passa_teste(){
    int valor_esperado;

    for(int pos=0; pos < TAM; pos++){
        valor_esperado = pos * pos;    
        
        if(vetor[pos] != valor_esperado)
            return 0;
    }

    return 1;
}



int main(){
    int erro;
    pthread_t tid[2]; 
    INTERV interv[2];


    /* Inicializa o vetor */
    for(int pos=0; pos < TAM; pos++)
        vetor[pos] = pos;


    /* Define os intervalos de atuação de cada thread */
    interv[0].ini = 0;
    interv[0].fim = TAM/2;

    interv[1].ini = TAM/2;
    interv[1].fim = TAM;


    /* Cria as duas threads */
    for(int thread=0; thread<2; thread++){
        erro = pthread_create(&tid[thread], NULL, quad_vetor, (void *) &interv[thread]);

        // Termina a execução em caso de erro
        if(erro){
            printf("Erro na criação das threads.\n");
            return erro;
        }
    }


    /* Aguarda o término das threads */
    for(int thread=0; thread<2; thread++){ 
        erro = pthread_join(tid[thread], NULL); 
        
        // Termina a execução em caso de erro
        if(erro){
            printf("Erro na espera das threads.\n");
            return erro;        
        }
    }


    /* Escreve o vetor resultante na tela */
    /*printf("O vetor resultante terá: \n\n");
    for(int pos=0; pos < TAM; pos++)
        printf("Elemento %d: (%d)^2 = %d\n", pos, pos, vetor[pos]);
    */
    

    /* Verifica se o cálculo foi feito adequadamente */
    if(passa_teste())
        printf("\nOs valores finais do vetor estão corretos! :)\n");
    else
        printf("\nOs valores finais do vetor não estão corretos.. :(\n");

    return 0;
}
