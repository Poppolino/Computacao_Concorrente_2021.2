import java.util.concurrent.atomic.AtomicInteger;


// Monitor que implementa a logica do padrao leitores/escritores
class MonitorLeitEsc{
    private int leit, escr;  
  
    // Construtor
    MonitorLeitEsc() { 
        this.leit = 0; //leitores lendo (0 ou mais)
        this.escr = 0; //escritor escrevendo (0 ou 1)
    } 
  

    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
        while (this.escr > 0) {
            //System.out.println ("le.leitorBloqueado("+id+")");
            
            try { 
                wait();  //bloqueia pela condicao logica da aplicacao 
            } catch (InterruptedException e) {}
        }

        this.leit++;  //registra que ha mais um leitor lendo
        //System.out.println ("le.leitorLendo("+id+")");
    }   
    

    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
        this.leit--; //registra que um leitor saiu
        
        if (this.leit == 0) 
            this.notify(); //libera escritor (caso exista escritor bloqueado)
        
        //System.out.println ("le.leitorSaindo("+id+")");
    }
    

    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
        while ((this.leit > 0) || (this.escr > 0)) {
            //System.out.println ("le.escritorBloqueado("+id+")");
            
            try { 
                wait();  //bloqueia pela condicao logica da aplicacao 
            } catch (InterruptedException e) {}
        }

        this.escr++; //registra que ha mais um escritor escrevendo
        //System.out.println ("le.escritorEscrevendo("+id+")");
    }
  

    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
        this.escr--; //registra que o escritor saiu
        notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
        //System.out.println ("le.escritorSaindo("+id+")");
    }
}





class Leitor extends Thread{
    private int id;
    private AtomicInteger base; 
    private MonitorLeitEsc monitor;
    private static final int TEMPO = 2000;

    
    /* Construtor da Classe */
    Leitor(int id, AtomicInteger base, MonitorLeitEsc monitor){
        this.id = id;
        this.base = base;
        this.monitor = monitor;
    }


    /* Método a ser executado pela Thread */    
    public void run(){
        int num;

        while(true){
            monitor.EntraLeitor(id);

            /* Imprime se o número lido é par ou ímpar */
            num = base.get();
            
            if(num%2 == 0)
                System.out.println("A variável possui valor par: " + num);
            else
                System.out.println("A variável possui valor ímpar: " + num);
            
            monitor.SaiLeitor(id);

            
            /* Consumo de tempo para gerar um log mais enxuto e variado */
            try{
                sleep(TEMPO);
            } catch(InterruptedException e){}
        }
    }
}





class Escritor extends Thread{
    private int id;
    private AtomicInteger base;
    private MonitorLeitEsc monitor;
    private static final int TEMPO = 2000;


    /* Construtor da Classe */
    Escritor(int id, AtomicInteger base, MonitorLeitEsc monitor){
        this.id = id;
        this.base = base;
        this.monitor = monitor;
    }


    /* Método a ser executado pela Thread */    
    public void run(){
        while(true){

            /* Escreve o identificador da Thread na Base de Dados */
            monitor.EntraEscritor(id);
            base.set(id);
            monitor.SaiEscritor(id);


            /* Consumo de tempo para gerar um log mais enxuto e variado */
            try{
                sleep(TEMPO);
            } catch(InterruptedException e){}
        }
    }
}





class LeitorEscritor extends Thread{
    private int id;
    private AtomicInteger base;
    private MonitorLeitEsc monitor;
    private static final int TEMPO = 2000;


    /* Construtor da Classe */
    LeitorEscritor(int id, AtomicInteger base, MonitorLeitEsc monitor){
        this.id = id;
        this.base = base;
        this.monitor = monitor;
    }


    /* Método a ser executado pela Thread */    
    public void run(){
        int bobo = 0;

        while(true){
            
            /* Lê o valor da base e imprime na tela */
            monitor.EntraLeitor(id);
            System.out.println("Valor da Variável: " + base.get());
            monitor.SaiLeitor(id);


            /* Processamento bobo qualquer */
            for(int i=0; i<1000000; i++){
                bobo += i; 
            }


            /* Incrementa o valor da base em 1 */
            monitor.EntraEscritor(id);
            base.incrementAndGet();
            monitor.SaiEscritor(id);        
    

            /* Consumo de tempo para gerar um log mais enxuto e variado */      
            try{
                sleep(TEMPO);
            } catch(InterruptedException e){}
        }
    }
}





// Classe Principal
class ProblemaLeitorEscritor{
    /* Definem as quantidades de cada tipo de Thread */
    static final int L = 2;
    static final int E = 7;
    static final int LE = 4;


    public static void main(String[] args){

        /* Cria os vetores das threads Leitoras, Escritoras e Leitoras/Escritoras */
        Leitor[] leit = new Leitor[L];
        Escritor[] escrit = new Escritor[E];
        LeitorEscritor[] leitEsc = new LeitorEscritor[LE];

        /* Cria o monitor de sincronização das threads */
        MonitorLeitEsc monitor = new MonitorLeitEsc();
        
        /* Define a Base de dados compartilhada entre as threads */
        AtomicInteger base = new AtomicInteger(0);


        /* Cria e inicia as threads Leitoras */
        for(int i=0; i < leit.length; i++){
            leit[i] = new Leitor(i+1, base, monitor);
            leit[i].start();
        }
        
        /* Cria e inicia as threads Escritoras */
        for(int i=0; i < escrit.length; i++){
            escrit[i] = new Escritor(L+i+1, base, monitor);
            escrit[i].start();
        }
        
        /* Cria e inicia as threads Leitoras/Escritoras */
        for(int i=0; i < leitEsc.length; i++){
            leitEsc[i] = new LeitorEscritor(L+E+i+1, base, monitor);
            leitEsc[i].start();
        }
    }
}