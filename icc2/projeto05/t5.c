#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define primoAUX 569	//numero primo auxiliar para hashing

typedef struct{	//variaveis das operacoes
	char oi;	//operacao
	int bi;		//banco da operacao
	int bj;		//banco destino
	int ordem;	//ordem da operacao (usado para as consultas e para o hashing)
	float val;	//valor
}operacoes;

typedef struct{	//variaveis dos bancos
	float Mpos;	//moeda positiva
	float Mneg;	
	float Tpos;
	float Tneg;	//transferencia negativa
	int cod;	//codigo do banco
}bancos;

typedef struct{	//variaveis dos terminais
	operacoes** op;	//vetor de operacoes para tabela hash
	int tam;		//tamanho da tabela
	int a, b;		//coeficientes aleatorios para hashing
	operacoes* erros;	//vetor de erros
	int nerros;		//numero de erros
	int nt;			//numero de tranferencias
	int nc;			//consultas
	int ns;			//saques
	int nd;			//depositos

	//as informacoes abaixo serao armazenadas no terminal apenas depois de receber todas as operacoes (final do dia)
	bancos* bc;		//vetor de bancos
	int nbc;		//numero de bancos
	float luc;		//lucro
}terminal;

typedef struct{	//variaveis da entrada
	int ti;
	int bi;
	int bj;
	int nop;	//numero de operacoes (auditoria)
	char oi;
	float val;
}variaveis;

int hashing(int ind, int num, terminal trm){	//recebe tipo e ordem da operacao
												//faz conta com os coeficientes e tamanho do terminal passado
	int hash;
		
	hash = ((ind + trm.a * num + trm.b) % primoAUX) % trm.tam;		
	
	return hash;
}

void insere_op(terminal* trm, variaveis* var, int erro){	//insercao das operacoes na tabela hash

	int ti = var->ti, bi = var->bi, bj = var->bj;
	char oi = var->oi;			//cria essas variaveis para clarear o codigo;
	float val = var->val;
	int hash=0;
	int i=0;
	int cont;
	
	if(erro){	//se houver erro, insere no vetor de erros
				//nao precisa ser hash, pois nao havera consultas de erros
		i = trm[ti].nerros;	
		trm[ti].erros = (operacoes*)realloc(trm[ti].erros, (i+1)*sizeof(operacoes));
		trm[ti].erros[i].oi = oi;
		trm[ti].erros[i].bi = bi;
		trm[ti].erros[i].bj = bj;
		trm[ti].erros[i].val = val;
		trm[ti].nerros++;

		i = trm[0].nerros;	//guarda no terminal de total tbm
		trm[0].erros = (operacoes*)realloc(trm[0].erros, (i+1)*sizeof(operacoes));
		trm[0].erros[i].oi = oi;
		trm[0].erros[i].bi = bi;
		trm[0].erros[i].bj = bj;
		trm[0].erros[i].val = val;
		trm[0].nerros++;
	}	

	else{				//sem erro, insere na tabela hash
						//transforma o tipo e a ordem da operacao no indice
		if(oi == 'S'){

			trm[ti].ns++;

			hash = hashing(0, trm[ti].ns, trm[ti]);
		}
		else if(oi == 'D'){

			trm[ti].nd++;

			hash = hashing(1, trm[ti].nd, trm[ti]);
		}
		else if(oi == 'C'){

			trm[ti].nc++;

			hash = hashing(2, trm[ti].nc, trm[ti]);
		}
		else if(oi == 'T'){

			trm[ti].nt++;

			hash = hashing(3, trm[ti].nt, trm[ti]);
		}

		cont=0;
		while(trm[ti].op[hash] != NULL && cont < trm[ti].tam){ //trata colisoes

			hash = (hash + 1) % trm[ti].tam;
			cont++;
		}
		if(cont == trm[ti].tam) return;
		
		//insere a operacao
		trm[ti].op[hash] = (operacoes*)malloc(sizeof(operacoes));	

		trm[ti].op[hash]->oi = oi;
		trm[ti].op[hash]->bi = bi;
		trm[ti].op[hash]->bj = bj;
		trm[ti].op[hash]->val = val;

		if(oi == 'S'){
			trm[ti].op[hash]->ordem = trm[ti].ns;
		}
		else if(oi == 'D'){
			trm[ti].op[hash]->ordem = trm[ti].nd;
		}
		else if(oi == 'C'){
			trm[ti].op[hash]->ordem = trm[ti].nc;
		}
		else if(oi == 'T'){
			trm[ti].op[hash]->ordem = trm[ti].nt;
		}	
	}
			
}

void insere_banco(terminal* trm, int bi){	//insere um banco novo no vetor de bancos de um terminal
	
	bancos aux;
	trm->bc = (bancos*)realloc(trm->bc, (trm->nbc +1)* sizeof(bancos));
	trm->bc[trm->nbc].cod = bi;
	trm->bc[trm->nbc].Mpos = 0;
	trm->bc[trm->nbc].Mneg = 0;
	trm->bc[trm->nbc].Tpos = 0;
	trm->bc[trm->nbc].Tneg = 0;

	int i = trm->nbc;

	while(i>0 && trm->bc[i].cod < trm->bc[i-1].cod){	//ordena ele

		aux = trm->bc[i];
		trm->bc[i] = trm->bc[i-1];
		trm->bc[i-1] = aux;

		i--;				
	}
	trm->nbc++;
}

int acha_banco(terminal* trm, int bi){	//acha o indice de um banco
	int i=0;

	while(i<trm->nbc && trm->bc[i].cod != bi){
		i++;
	}

	if(i == trm->nbc){

		insere_banco(trm, bi);	//se o banco nao estiver no vetor, insere ele

		i=0;
		while(i<trm->nbc && trm->bc[i].cod != bi){
			i++;
		}
	}
	return i;
}

void gera_relatorio(terminal* trm){
	
	int i,j;

	int bi, bj;		//variaveis para clarear codigo
	char oi;
	float val;
	int k0, k1, k2, k3;	//auxiliares para indices de bancos

	int erroC, erroD, erroS, erroT;	//contadores de erros

	for(i=1;i<5;i++){
		erroC=0;
		erroD=0;
		erroS=0;
		erroT=0;

		for(j=0;j<trm[i].tam;j++){	//passa pela tabela hash

			if(trm[i].op[j]!=NULL){				

				bi = trm[i].op[j]->bi;
				bj = trm[i].op[j]->bj;
				oi = trm[i].op[j]->oi;
				val = trm[i].op[j]->val;

				if(oi == 'S' || oi == 'D' || (oi == 'T' && bi!=bj)){

					//acha indice do banco (no vetor de bancos do terminal)
					//esse vetor sera criado na funcao acha_banco

					if(oi == 'T' && bi!=bj){	//tranferencia precisa achar banco destino
						k2 = acha_banco(&trm[i], bj);
						k3 = acha_banco(&trm[0], bj);
					}					

					k0 = acha_banco(&trm[i], bi);
					k1 = acha_banco(&trm[0], bi);
				}		

				if(oi=='S'){	//muda os valores
					trm[i].bc[k0].Mneg+=val;
					trm[0].bc[k1].Mneg+=val;	//muda no terminal auxiliar para o total tbm
				}
				else if(oi == 'D'){
					trm[i].bc[k0].Mpos+=val;
					trm[0].bc[k1].Mpos+=val;
				}
				else if(oi == 'T' && bi != bj){
					
					trm[i].bc[k0].Tneg+=val;
					trm[i].bc[k2].Tpos+=val;
					trm[0].bc[k1].Tneg+=val;
					trm[0].bc[k3].Tpos+=val;
				}
				
				trm[i].luc+=3;
				trm[0].luc+=3;
				
			}
		}

		for(j=0;j<trm[i].nerros;j++){	//conta os erros no vetor de erros
			if(trm[i].erros[j].oi == 'S'){
				erroS++;
			}
			else if(trm[i].erros[j].oi == 'D'){
				erroD++;
				
			}
			else if(trm[i].erros[j].oi == 'C'){
				erroC++;
				
			}
			else if(trm[i].erros[j].oi == 'T'){
				erroT++;
				
			}
		}
	

		printf("===TERMINAL %d===\n", i);

		for(j=0;j<trm[i].nbc;j++){

			printf("Banco %d: Moeda +%.2f -%.2f", trm[i].bc[j].cod, trm[i].bc[j].Mpos, trm[i].bc[j].Mneg);
			printf(" Transferencia +%.2f -%.2f\n", trm[i].bc[j].Tpos, trm[i].bc[j].Tneg);
		}

		printf("Lucro obtido: %.2f\n", trm[i].luc);	

		if(erroS>0) printf("Erros de saque: %d\n", erroS);		
		if(erroD>0) printf("Erros de deposito: %d\n", erroD);	
		if(erroC>0) printf("Erros de consulta: %d\n", erroC);
		if(erroT>0) printf("Erros de transferencia: %d\n", erroT);	
		if(trm[i].nerros>0) printf("Total de erros: %d\n", trm[i].nerros);
	}

	//conta os erros no terminal do total

	for(j=0;j<trm[0].nerros;j++){
		if(trm[0].erros[j].oi == 'S'){
			erroS++;
		}
		else if(trm[0].erros[j].oi == 'D'){
			erroD++;
			
		}
		else if(trm[0].erros[j].oi == 'C'){
			erroC++;
			
		}
		else if(trm[0].erros[j].oi == 'T'){
			erroT++;
			
		}
	}

	//imprime o total
	printf("===TOTAL===\n");

	for(j=0;j<trm[0].nbc;j++){

		printf("Banco %d: Moeda +%.2f -%.2f", trm[0].bc[j].cod, trm[0].bc[j].Mpos, trm[0].bc[j].Mneg);
		printf(" Transferencia +%.2f -%.2f\n", trm[0].bc[j].Tpos, trm[0].bc[j].Tneg);
	}

	printf("Lucro obtido: %.2f\n", trm[0].luc);

	if(erroS>0) printf("Erros de saque: %d\n", erroS);	
	if(erroD>0) printf("Erros de deposito: %d\n", erroD);
	if(erroC>0) printf("Erros de consulta: %d\n", erroC);	
	if(erroT>0) printf("Erros de transferencia: %d\n", erroT);	
	if(trm[0].nerros>0) printf("Total de erros: %d\n", trm[0].nerros);

}

void auditoria(terminal* trm, variaveis* var){	//consulta da auditoria
	int i, hash;
	int ind;
	int ti = var->ti;
	int cont;

	if(var->oi == 'S'){
		printf("===SAQUE TERMINAL %d===\n", ti);
		ind =0;
	}
	else if(var->oi == 'D'){
		printf("===DEPOSITO TERMINAL %d===\n", ti);
		ind =1;
	}
	else if(var->oi == 'C'){
		printf("===CONSULTA TERMINAL %d===\n", ti);
		ind =2;
	}
	else if(var->oi == 'T'){
		printf("===TRANSFERENCIA TERMINAL %d===\n", ti);
		ind =3;
	}
	printf("Mostrando primeiros %d resultados\n", var->nop);

	for(i=0;i<var->nop;i++){	//procura as operacoes de ordem 1 ate N passado
								
		hash = hashing(ind, i+1, trm[ti]);	//usei a ordem no hashing para n precisar ordenar nada nas operacoes
		cont = 0;

		while(trm[ti].op[hash] != NULL && (trm[ti].op[hash]->oi != var->oi || trm[ti].op[hash]->ordem != i+1)){

			hash = (hash + 1) % trm[ti].tam;
			cont++;
			if(cont>trm[ti].tam) break;
		}	//procura em caso de colisao
		
		if(trm[ti].op[hash] == NULL){

			if(i==0) printf("Sem resultados\n");
			return;
		}
		else if(trm[ti].op[hash] != NULL){

			if(ind == 0 || ind == 1){

				printf("%d- Banco %d %.2f\n", i+1, trm[ti].op[hash]->bi, trm[ti].op[hash]->val);
			}

			else if(ind == 3){

				printf("%d- Banco origem %d Banco destino %d %.2f\n", i+1, trm[ti].op[hash]->bi, 
						trm[ti].op[hash]->bj, trm[ti].op[hash]->val);
			}	

		}
	}
	
}

int verifica_int(char* string){	//verifica se uma string representa um inteiro ou um float
	int t = strlen(string);
	int i;
	for(i=0;i<t;i++){
		if(string[i]=='.') return 0;
	}
	return 1;
}

terminal* cria_terminais(){
	terminal* trm = (terminal*)malloc(5 * sizeof(terminal));
	//5 terminais, 4 e mais um para guardar o total
	int i;

	trm[1].tam= 131;
	trm[2].tam= 137;
	trm[3].tam= 139;
	trm[4].tam= 149;	//divisoes diferentes para os terminais

	for(i=0;i<5;i++){
		trm[i].nbc = 0;	
		trm[i].bc = NULL;
		trm[i].nt = 0;
		trm[i].ns = 0;
		trm[i].nc = 0;
		trm[i].nd = 0;
		trm[i].luc =0;
		trm[i].a = (rand() % primoAUX)+1;	//coeficientes diferentes para o hashing dos terminais
		trm[i].b = (rand() % primoAUX);
		trm[i].erros = NULL;
		trm[i].nerros = 0;	
		if(i!=0) trm[i].op = (operacoes**)calloc(trm[i].tam, sizeof(operacoes*));	
	}			
	
	return trm;
}

void desaloca(terminal* trm, variaveis* var){
	int i,j;
	for(i=0;i<5;i++){
		for(j=0;j<trm[i].tam;j++){
			if(trm[i].op[j] != NULL) free(trm[i].op[j]);
		}
		free(trm[i].op);
		free(trm[i].bc);		
	}
	free(trm);
	free(var);
}

int main(){
	//srand(time(NULL));
	int gerou = 0;	//indica se ja gerou relatorio
	int flag = 1;	//indica se deve ler o numero do terminal
	int erro;

	terminal* trm = cria_terminais();

	variaveis* var = (variaveis*)malloc(sizeof(variaveis));

	char string[10];	//string auxiliar para leitura da entrada

	while(feof(stdin)==0){
		if(flag)			
			scanf("%d", &var->ti);	//qual terminal
		else flag=1;

		scanf("%s", string);	//prox informacao

		if(string[0]!='C' && string[0]!='D' && string[0]!='S' && string[0]!='T'){	//se nao for um desses, eh uma operacao
			
			var->bi = atoi(string);	//string leu o codigo do banco

			scanf(" %c", &var->oi);	//le a operacao

			if(var->oi == 'T'){	//se for tranferencia

				scanf("%s", string);

				if(verifica_int(string)){	//se prox for inteiro, eh entre bancos diferentes

					var->bj = atoi(string);	//string leu banco destino

					scanf("%f", &var->val);	//pega o valor
				}
				else{	//senao, mesmo banco
					var->bj = var->bi;

					var->val = atof(string);	//a string leu o valor
				}
			}
			else if(var->oi != 'C'){	//se for saque ou deposito

				scanf("%f", &var->val);	//le o valor				
			}			

			if(!feof(stdin)) scanf("%s", string);	//ve se houve erro

			if(strcmp(string, "ERRO")!=0){	//se nao houve erro

				erro=0;
				insere_op(trm, var, erro);	//insere op sem erro

				var->ti = atoi(string);	//string leu numero do terminal

				flag = 0;	//indica que nao deve ler numero de terminal na prox iteracao
			}
			else{	//se houve erro

				erro = 1;	//deve ler numero de terminal

				insere_op(trm, var, erro);	//insere op com erro
			}				
					
		}
		else{	//se a segunda info for uma op, deve fazer consultas

			if(!gerou){	//se ainda nao gerou relatorio

				gera_relatorio(trm);

				gerou = 1;	//nao ira mais gerar relatorio

				printf("===AUDITORIA===\n");
			}

			var->oi = string[0];	//string leu a operacao da consultas

			scanf("%d", &var->nop);	//pega o numero de operacoes

			auditoria(trm, var);	//imprime os resultados
		}
		
	}
	if(!gerou){	//talvez nao haja consultas
				//se acabou entrada e ainda nao gerou relatorio, gera

		gera_relatorio(trm);
	}

	desaloca(trm, var);

	return 0;
}