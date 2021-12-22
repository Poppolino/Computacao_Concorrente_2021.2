#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define NUM_THR 5


int cont = 0;
char **mensagem;
pthread_mutex_t tranca;
pthread_cond_t cond_ordem;


    /*
        Autor: Pedro Poppolino
        
        Atividade:
        Fazer um programa que escreve mensagens na tela a partir de threads 
        que precisam seguir uma determinada ordem. 
        Essa ordem deve ser dada usando variáveis de condição.
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

    /* Faz o lock por entrar em uma seção crítica */
    pthread_mutex_lock(&tranca);


    /* A thread aguarda (bloqueada) até que as condições sejam propícias */
    while((cont==0 && id!=5) || (cont<4 && id==1))
        pthread_cond_wait(&cond_ordem, &tranca);


    /* Escreve a mensagem e contabiliza essa mensagem para o total */
    cont++;
    printf("%s\n", mensagem[id-1]);
    

    /* Realiza o unlock (por sair da seção crítica), libera todos os bloqueados pela condição e sai da thread */
    pthread_mutex_unlock(&tranca);  
    pthread_cond_broadcast(&cond_ordem);
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


    /* Inicializa a variável de exclusão mútua e a de condição */
    pthread_mutex_init(&tranca, NULL);
    pthread_cond_init(&cond_ordem, NULL);


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
    pthread_mutex_destroy(&tranca);
    pthread_cond_destroy(&cond_ordem);


    return 0;
}