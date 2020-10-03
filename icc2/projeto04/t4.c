/*
	Marcelo Kiochi Hatanaka
	10295645
	Trabalho 4 de ICC2 - Simulador de Escalonamento de Processos
*/

#include  <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
typedef struct{ //variaveis de cada processo
	int cod; 		//codigo
	int inicio;		//tempo em que o processo e' reconhecido pelo escalonador (t0)
	int tempo;		//tempo que o processo leva para ser executado (tf)
	int prioridade; 
	int exe;		//indica se o processo esta em execucao no momento (1 - sim/0 - nao)
	int fim;		//indica se o processo ja foi finalizado (1 - sim /0 - nao)
}processo;

typedef struct{		//variaveis do escalonador
	int n;			//numero de processos
	int nexe;		//numero de processos em execucao
	int n_pr_max;	//numero de processos de prioridade maxima (0)
	int pr_max_exe;	//numero de processos de prioridade maxima em execucao
}escalonador;

int verif_ordem(processo pro1, processo pro2, int FIFO){ //verifica se o processo 1 deve ser executado antes do 2

	if(FIFO){
		if(pro1.inicio < pro2.inicio) return 1;
		else if(pro1.inicio > pro2.inicio) return 0;
	}

	else{
		if(pro1.prioridade > pro2.prioridade) return 1;			//retorna 1 se sim
		else if(pro1.prioridade < pro2.prioridade) return 0;	//0 se nao
	}
	

	if(pro1.cod < pro2.cod) return 1; //se prioridade for igual, compara codigo

	return 0;
}

void insertion_sort(processo* pro, int n, int FIFO){ //insere um processo no vetor de processos de forma ordenada
	processo pro_aux;
	int i = n;

	while(i>0 && verif_ordem(pro[i], pro[i-1], FIFO)){
		pro_aux = pro[i];
		pro[i] = pro[i-1];
		pro[i-1]=pro_aux;
		i--;
	}
}

void verifica_igual(processo* pro, int n){ //verifica se um processo tem o mesmo codigo de outro
	int i;								   //caso tenha, muda para o maior mais proximo
	int erro =1;
	while(erro){
		erro =0;
		for(i=0;i<n;i++){
			if(pro[i].cod==pro[n].cod){
				pro[n].cod++;
				erro=1;
			}
		}
	}
}

void inicia_var_pro(processo* pro, escalonador* esc){

	pro->fim = 0;				//inicia as variaveis do proesso
 	pro->exe = 0;				
 	if(pro->prioridade == 0){
 		pro->prioridade = 5;		//muda a prioridade de tempo real para 5, para facilitar
 		esc->n_pr_max++;
 	}
}

int le_entrada(escalonador* esc, processo** pro){ //le a lista de processos
	
	char tipo[10];
	int FIFO=0;
	

	scanf("%s", tipo);

 	if(strcmp(tipo, "f")==0){
 		FIFO=1;
 	}
 	else{
 		esc->n++;

 		(*pro) = (processo*)realloc(*pro, sizeof(processo));

 		(*pro)[0].cod = atoi(tipo);		//se nao leu 'f', a primeira string que leu e' o codigo do primeiro processo 		
 	}

 	if(FIFO){	//se leu 'f', le o primeiro processo sem a prioridade
 		*pro = (processo*)realloc(*pro, sizeof(processo));
 		scanf("%d%d%d", &(*pro)[0].cod, &(*pro)[0].inicio, &(*pro)[0].tempo);
 		(*pro)[0].prioridade =1; //se FIFO for utilizado, todas as prioridades serao 1	 		
 		esc->n++;
 	}

 	else{	
 		scanf("%d%d%d", &(*pro)[0].inicio, &(*pro)[0].tempo, &(*pro)[0].prioridade); 	 		
 	}

 	inicia_var_pro(&(*pro)[0], esc);
 
 	while(!feof(stdin)){	//le os outros processos ate o fim
 		*pro = (processo*)realloc(*pro, (1+esc->n) * sizeof(processo));

 		scanf("%d%d%d", &(*pro)[esc->n].cod, &(*pro)[esc->n].inicio, &(*pro)[esc->n].tempo);

 		verifica_igual(*pro, esc->n);

 		if(!FIFO){	//le a prioridade somente se nao utilizar FIFO
 			scanf("%d", &(*pro)[esc->n].prioridade);
 		}
 		else{
 			(*pro)[esc->n].prioridade = 1;
 		}

 		inicia_var_pro(&(*pro)[esc->n], esc);		

 		insertion_sort(*pro, esc->n, FIFO); //insere cada processo no vetor (ordenando)
 		
 		esc->n++; 
 			
 	}

 	return FIFO;//retorna se vai utilizar FIFO ou nao
}


int verifica_comeco(escalonador* esc, processo* pro, int tempo, int* comecou){	//dado um instante de tempo
																		//verifica se algum processo comeca nesse instante
	int menor=-1;	//se algum processo comecar, guardara o de maior prioridade (menor indice)

	int i;
	for(i=0;i< esc->n; i++){
		if(pro[i].inicio == tempo){	

			pro[i].exe = 1;
			esc->nexe++;

			if(pro[i].prioridade==5){
				esc->pr_max_exe++;
			}

			*comecou=1;						//indica, para o escalonador, que algum processo comecou
			if(menor== -1) menor = i;			
		}
	}

	return menor;
}

int verifica_fim(escalonador* esc, processo* pro, int atual, int tempo){	//verifica se o processo atual chegou ao fim
	
	if(pro[atual].tempo==0){

		pro[atual].fim=1;
		pro[atual].exe=0;
		esc->nexe--;

		if(pro[atual].prioridade==5) esc->pr_max_exe--;

		printf("%d %d\n", pro[atual].cod, tempo-1);	//printa o tempo

		return 1;	//retorna 1 para indicar que o processo acabou
	}
	return 0;
}

int atualiza(escalonador* esc, processo* pro, int atual){	//muda de processo

	do{
		atual++;	

		if(esc->pr_max_exe == 0) atual = atual % esc->n;

		else atual = atual % esc->n_pr_max;	//se um ou mais processos tem prioridade tempo real
											//o escalonador fica girando entre eles
	}while(pro[atual].exe == 0);
	
	return atual;
}

int volta_nivel(escalonador* esc, processo* pro, int atual, int nivel){	//atualiza para o primeiro precesso de um nivel

	int i=0;

	for(i=0;i<esc->n;i++){
		if(pro[i].prioridade == nivel) return i;
	}

	return atual;

}

void processa(escalonador* esc, processo* pro, int FIFO){	//processo de escalonemanto

	int comecou = 0;	//indica se pelo menos um processo ja comecou
	int atual=-1;		//indice do processo atual em execucao
	int tempo=0;		//instante atual
	int novo_pro=-1;
	int fim_pro=-1;
	int nivel=0;		//nivel de prioridade do processo atual
	int prox=-1;		//indice de um processo novo caso tenha maior prioridade que o atual

	while(comecou==0 || esc->nexe!=0){	//executa o escalonamento enquanto algum processo ainda nao tenha finalizado

		fim_pro=0;
		novo_pro = -1;		

		if(comecou)	pro[atual].tempo--;	//diminui a contagem de tempo do processo atual

		if(comecou) fim_pro = verifica_fim(esc, pro, atual, tempo);

		novo_pro = verifica_comeco(esc, pro, tempo, &comecou);
		
		if(comecou && esc->nexe){		//muda de processo se algum ja comecou

			nivel = pro[atual].prioridade;	//guarda nivel de prioridade do atual

			if(!FIFO || fim_pro || atual<0){	//se nao utilizar FIFO, muda para o proximo em execucao do vetor

				if(FIFO) atual=-1;	//se utilizar FIFO, muda so se o processo acabou e muda para o de maior prioridade em execucao

				atual = atualiza(esc, pro, atual);
			}  

			if(novo_pro != -1 && atual != -1){
				if(pro[novo_pro].prioridade > pro[atual].prioridade && (novo_pro < prox || prox==-1))
					prox = novo_pro;	//guarda indice de um processo novo caso ele tenha maior prioridade que o atual
			}							//ou se tiver maior prioridade que o processo que seria o proximo
			
			if(pro[atual].prioridade < nivel && prox!=-1){	//se acabar o processamento de um nivel e houver processo
															//de maior prioridade para voltar
				nivel = pro[prox].prioridade;

				atual = volta_nivel(esc, pro, atual, nivel);	//volta para esse nivel

				prox=-1;
			}			
		}	

		tempo++;
	}
}

 int main(){

 	escalonador* esc = (escalonador*)malloc(sizeof(escalonador));

 	esc->n=0; 				//inicia variaveis do escalonador
 	esc->pr_max_exe = 0;
 	esc->nexe =0;

 	processo* pro = NULL;

 	int FIFO;

 	FIFO = le_entrada(esc, &pro); 

 	processa(esc, pro, FIFO);

 	free(pro);
 	free(esc);
 	
 	return 0;
 }