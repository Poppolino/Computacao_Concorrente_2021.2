# Define funções para verificar a lógica de execução de uma variação do Produtor/Consumidor


class Prod_Cons:
    
    '''Construtor'''
    def __init__(self):
        self.prod = 0
        self.cons = 0


    '''Recebe o id do consumidor e o total de elementos, verifica se pode retirar um elemento e registra que está retirando.'''
    def consumidorRetirando(self, id, total):
        if(self.cons > 0):
            print("ERRO: Consumidor " + str(id) + " está retirando um elemento com outro consumidor!")
        
        if(total == 0):
            print("ERRO: Consumidor " + str(id) + " está tentando retirar um elemento de um Buffer vazio!")
        
        self.cons += 1



    '''Recebe o id do produtor e o total de elementos, verifica se pode inserir e registra que está inserindo'''
    def produtorInserindo(self, id, total):
        if(self.prod > 0):
            print("ERRO: Produtor " + str(id) + " está preenchendo todo o Buffer ao mesmo tempo que outro produtor!")
        
        if(total > 0):
            print("ERRO: Produtor " + str(id) + " está preenchendo o Buffer sem que ele esteja completamente vazio!")
        
        self.prod += 1



    '''Recebe o id do consumidor e registra que terminou de retirar um elemento.'''
    def consumidorSaindo(self, id):
        self.cons -= 1



    '''Recebe o id do produtor e registra que terminou o preenchimento do Buffer.'''
    def produtorSaindo(self, id):
        self.prod -= 1

