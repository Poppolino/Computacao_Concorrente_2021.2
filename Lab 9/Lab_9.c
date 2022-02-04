#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#define NUM_THR 5


int cont = 0;
char **mensagem;
sem_t exclusao, meio, fim; 


    /*
        Autor: Pedro Poppolino
        
        Atividade:
        Fazer um programa que escreve mensagens na tela a partir de threads 
        que precisam seguir uma determinada ordem. 
        Essa ordem deve ser dada usando semáforo.
    */




// Inicializa o vetor de mensagens das threads
void inicializa(char *mensagem[]){

    /* Aqui as mensagens são fixas, mas poderiam ser lidas do teclado, por exemplo */
    mensagem[0] = "Volte sempre!";
    mensagem[1] = "Fique a vontade.";
    mensagem[2] = "Sente-se por favor.";
    mensagem[3] = "Aceita um copo d’água?";
    mensagem[4] = "Seja bem-vindo!";
}





// Função de escrita de mensagem na tela, e é executada por cada thread
void * escreve(void *arg){
    int id = *((int *) arg);    

    /* A Thread 1 vai aguardar todas as outras terem sido executadas */
    if(id == 1)
        sem_wait(&fim);
    

    /* As Threads de 2 a 4 vão esperar a 5 executar */
    if(1<id && id<5)
        sem_wait(&meio);


    /* Escreve a mensagem da Thread na tela */
    printf("%s\n", mensagem[id-1]);


    /* A Thread 5 libera todas as Threads intermediárias */
    if(id == 5){
        for(int i=0; i<3; i++)
            sem_post(&meio);
    }


    /* 
        É feita uma exclusão mútua usando semáforo.
        Se 4 Threads tiverem executado, a Thread 1 é finalmente liberada.
    */
    sem_wait(&exclusao);

    if(++cont == 4)
        sem_post(&fim);

    sem_post(&exclusao);


    pthread_exit(NULL);
}





int main(){
    int erro, *id;
    pthread_t *tid;


    /* Realiza a alocação de memória das estruturas utilizadas e retorna erro caso não seja possível */
    id = (int *) malloc(NUM_THR * sizeof(int));
    tid = (pthread_t *) malloc(NUM_THR * sizeof(pthread_t));
    mensagem = (char **) malloc(NUM_THR * sizeof(char *));


    if(id == (int *)NULL || tid == (pthread_t *)NULL || mensagem == (char **)NULL){
        printf("Erro na alocação de memória.\n");
        return 1;
    }


    /* Inicializa as variáveis de semáforo */
    sem_init(&exclusao, 0, 1);
    sem_init(&meio, 0, 0);
    sem_init(&fim, 0, 0);


    /* Define as mensagens que cada thread vai escrever */
    inicializa(mensagem);


    /* Cria todas as threads e retorna erro caso não seja possível */
    for(int thread=0; thread < NUM_THR; thread++){
        id[thread] = thread + 1;

        erro = pthread_create(&tid[thread], NULL, escreve, (void *) &id[thread]);

        if(erro){
            printf("Erro na criação das threads.\n");
            return erro;
        }
    }


    /* Aguarda as threads terminarem para poder liberar a memória utilizada. Se der algum problema, retorna erro. */
    for(int thread=0; thread < NUM_THR; thread++){
        erro = pthread_join(tid[thread], NULL);

        if(erro){
            printf("Erro na espera das threads.\n");
            return erro;
        }
    }   



    /* Libera a memória utilizada */
    
    //for(int i=0; i<NUM_THR; i++)
    //  free(mensagem[i]);
    free(id);
    free(tid);
    free(mensagem); 
    sem_destroy(&exclusao);
    sem_destroy(&meio);
    sem_destroy(&fim);


    return 0;
}
