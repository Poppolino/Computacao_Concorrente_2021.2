#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#define TAM 50

int buffer[TAM], sai=0, entra=0, total=0;
sem_t ocupados, exc_cons, exc_prod; 


    /*
        Autor: Pedro Poppolino
        
        Atividade:
        Implementar uma variação do padrão Produtor/Consumidor
        usando semáforos. 
    */





// Função que retira um elemento do Buffer 
int Retira(int id){
    int elemento;

    /* Espera até ter espaços no Buffer ocupados por elementos */
    sem_wait(&ocupados);


    /* Realiza a exclusão mútua entre consumidores */
    sem_wait(&exc_cons);

    printf("pc.consumidorRetirando(%d, %d)\n", id, total);


    /* Retira o Elemento e atualiza o índice de retirada */
    elemento = buffer[sai];
    sai = (sai + 1) % TAM;
    
    /*
        Atualiza o total de elementos no Buffer. Apesar de ser uma variável compartilhada, 
        dois consumidores não podem alterá-la simultaneamente por conta da exclusão mútua.
        Além disso, um produtor não pode acessá-la ao mesmo tempo que um consumidor por conta da lógica da aplicação.
    */
    total--;
    
    /* Se o Buffer esvaziar, libera uma thread produtora */
    if(total == 0)
        sem_post(&exc_prod);


    printf("pc.consumidorSaindo(%d)\n", id);

    /* Libera para que outra thread consumidora possa consumir */
    sem_post(&exc_cons);
       

    /* Retorna o elemento retirado */ 
    return elemento;
}





// Função que preenche completamente o Buffer com elementos 
void Insere(int id){

    /* Aguarda o Buffer ficar vazio e define a exclusão mútua das threads produtoras */
    sem_wait(&exc_prod);

    printf("pc.produtorInserindo(%d, %d)\n", id, total);

    /* Preenche o Buffer por completo com novos elementos */
    for(int i=0; i<TAM; i++){
        buffer[entra] = i;
        entra = (entra + 1) % TAM;
    }

    /* Atualiza o total de elementos do Buffer */
    total = TAM;

    /* Libera que todas as posições do Buffer possam ser consumidas */
    for(int i=0; i<TAM; i++)
        sem_post(&ocupados);


    printf("pc.produtorSaindo(%d)\n", id);
}





// Tarefa que as threads produtoras terão de fazer
void * Produtor(void *arg){
    int id = *((int *) arg);

    /* Fica preenchendo o Buffer por completo */
    while(1){
        // No convencional, produziria algum elemento aqui...
        Insere(id);

        /* Gasta um tempinho */
        sleep(1);
    }

    pthread_exit(NULL);
}





// Tarefa que as threads consumidoras terão de fazer
void * Consumidor(void *arg){
    int id = *((int *) arg);

    /* Fica retirando e "consumindo" elementos do Buffer */
    while(1){
        //int elemento = 
        Retira(id);
        //printf("Consome Elemento: %d\n", elemento);
        
        /* Gasta um tempinho */
        sleep(1);
    }

    pthread_exit(NULL);
}





int main(int argc, char *argv[]){
    int erro, prod, cons, *id;
    pthread_t *tid;

    /* Encerra o programa se tiver sido executado com pouca informação */
    if(argc < 3){
        printf("Escreva o padrão: %s <Número de Produtores> <Número de Consumidores>\n\n", argv[0]);
        return 1;
    }
    
    
    /* Converte os dados de entrada para valores inteiros */
    prod = atoi(argv[1]);
    cons = atoi(argv[2]);
    
    /* Se não houver threads produtoras ou consumidoras, não tem como executar o programa */
    if(prod < 1 || cons < 1){
        printf("O número de Produtores e Consumidores deve ser maior que zero.\n\n");
        return 1;
    }


    /* Realiza a alocação de memória das estruturas utilizadas e retorna erro caso não seja possível */
    id = (int *) malloc((prod+cons) * sizeof(int));
    tid = (pthread_t *) malloc((prod+cons) * sizeof(pthread_t));


    if(id == (int *)NULL || tid == (pthread_t *)NULL){
        printf("Erro na alocação de memória.\n");
        return 2;
    }


    /* Inicializa as variáveis de semáforo */
    sem_init(&ocupados, 0, 0);
    sem_init(&exc_cons, 0, 1);
    sem_init(&exc_prod, 0, 1);


    /* Inicia o log de saída em Python */
    printf("import verificaPC\n");
    printf("pc = verificaPC.Prod_Cons()\n");


    /* Cria as threads Produtoras e retorna erro caso não seja possível */
    for(int thread=0; thread < prod; thread++){
        id[thread] = thread + 1;

        erro = pthread_create(&tid[thread], NULL, Produtor, (void *) &id[thread]);

        if(erro){
            printf("Erro na criação das threads.\n");
            return erro;
        }
    }


    /* Cria as threads Consumidoras e retorna erro caso não seja possível */
    for(int thread=prod; thread < prod+cons; thread++){
        id[thread] = (thread-prod) + 1;

        erro = pthread_create(&tid[thread], NULL, Consumidor, (void *) &id[thread]);

        if(erro){
            printf("Erro na criação das threads.\n");
            return erro;
        }
    }
 

    /* Encerra as atividades da thread da main */
    pthread_exit(NULL);
    return 0;
}