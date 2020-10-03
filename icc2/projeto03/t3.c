/*
	Marcelo Kiochi Hatanaka 10295645
	Trabalho 3 de ICC2 - JSON Parser
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEU 1
#define NAO_LEU 0

#define OBJECT 0
#define ARRAY 1
#define PAIR 2
#define STRING 3
#define NUMBER 4
#define TRUE_ 5
#define FALSE_ 6
#define NULL_ 7

void le_value(int* n, char* linha, int* numbers);

int le_null(int* n, char* linha, int* numbers){ //recebe a string e a posicao onde esta sendo lida

	int t = strlen(linha);

	if(t-(*n) > 3 && linha[(*n)]=='n' && linha[(*n)+1]=='u' && linha[(*n)+2]=='l' && linha[(*n)+3]=='l'){ //se as proximas letras
																										  //corresponderem a palavra

		if(t-(*n) > 4 && linha[(*n)+4]!=']' && linha[(*n)+4]!='}' && linha[(*n)+4]!=',') return NAO_LEU;  //se o char posterior a palavra
																										  //nao for um desses
		(*n)+=4;		 //se estiver ok, atualiza a posicao de leitura									  //nao le a palavra											  
		numbers[NULL_]++;	 //aumenta o contador correspondente

		return LEU;  //retorna 1 se ler a palavra corretamente
	}
	return NAO_LEU; //retorna 0 se nao ler
}


int le_true(int* n, char* linha, int* numbers){ //mesma coisa que o le_null

	int t = strlen(linha);

	if(t-(*n) > 3 && linha[(*n)]=='t' && linha[(*n)+1]=='r' && linha[(*n)+2]=='u' && linha[(*n)+3]=='e'){

		if(t-(*n) > 4 && linha[(*n)+4]!=']' && linha[(*n)+4]!='}' && linha[(*n)+4]!=',') return NAO_LEU;  

		(*n)+=4;																						 
		numbers[TRUE_]++;	

		return LEU;
	}
	return NAO_LEU;
}


int le_false(int* n, char* linha, int* numbers){ //mesma coisa que o le_true

	int t = strlen(linha);

	if(t-(*n) > 4 && linha[(*n)]=='f' && linha[(*n)+1]=='a' && linha[(*n)+2]=='l' && linha[(*n)+3]=='s' && linha[(*n)+4]){

		if(t-(*n) > 5 && linha[(*n)+5]!=']' && linha[(*n)+5]!='}' && linha[(*n)+5]!=',') return NAO_LEU;

		(*n)+=5;
		numbers[FALSE_]++;

		return LEU;
	}
	return NAO_LEU;
}



int le_exp(int* n, char*linha){ //le o expoente de um numero
								//praticamente igual ao le_int

	int t = strlen(linha);

	int i = *n;

	while(i < t && (((linha[i]=='-' || linha[i]=='+') && i== *n) || (linha[i]>47 && linha[i]<58))){
		i++;
	}

	if(i==*n) return NAO_LEU;	

	if(linha[i-1]=='-' || linha[i-1]=='+') return NAO_LEU;

	*n=i;

	return LEU;
}


int le_frac(int* n, char*linha){ //le a parte fracionaria de um numero
								 //praticamente igual ao le_int

	int t = strlen(linha);

	int i = *n;

	while(i < t && (linha[i]>47 && linha[i]<58)){
		i++;
	}

	if(i==*n) return NAO_LEU;

	*n=i;

	return LEU;
}


int le_int(int* n, char*linha){ // recebe a string e a posicao de leitura 
								// se ler corretamente, atualiza a posicao de leitura (n)
	int t = strlen(linha);

	int i = *n;

	while(i < t && (((linha[i]=='-' || linha[i]=='+') && i== *n) || (linha[i]>47 && linha[i]<58))){ 
		i++;					//le o sinal (se tiver) e os numeros da parte inteira de um numero e vai avancando na string
								//para quando achar um nao-numero
	}

	if(i==*n) return NAO_LEU; //se nao avancar nenhuma posicao, e' porque nao tem numero	 

	if(linha[i-1]=='-' || linha[i-1]=='+') return NAO_LEU; //se avancar uma posicao, mas leu somente o sinal, nao conta

	*n=i; //se ler corretamente, atualiza a posicao de leitura

	return LEU;
}

int le_number(int* n, char* linha, int* numbers){ // le um numero (inteiro ou real)
	int i = *n;

	int t = strlen(linha);

	if(le_int(&i, linha)== NAO_LEU) return NAO_LEU;	//se nao ler um inteiro, nao conta numero
		
	if(i<t && linha[i]=='.'){	//se ler um ponto depois, verifica uma parte fracionaria	
		i++;
		if(le_frac(&i, linha)== NAO_LEU) return NAO_LEU; //se tiver ponto e nao tiver fracionaria, nao conta numero
	}

	if(i<t && (linha[i]=='e' || linha[i]=='E')){ //se tiver um E, verifica se tem expoente
		i++;
		if(le_exp(&i, linha)== NAO_LEU) return NAO_LEU; //se nao tiver um numero para expoente, nao conta numero
	}

	*n=i; //nao deu nada errado, atualiza a posicao de leitura da string
	numbers[NUMBER]++; //aumenta o contador

	return LEU;	
}




void le_element(int* n, char* linha, int* numbers){

	int i = *n;

	int t = strlen(linha);

	i--;

	do{		
		i++;		
		le_value(&i, linha, numbers);		
	}while(linha[i]==',' && i<t);	//le uma sequencia de values entre virgulas
									//se value ler alguma coisa, atualiza a posicao
									//senao, continua na mesma

	*n = i;

	return;
}


int le_array(int* n, char* linha, int* numbers){

	int i = *n;	
	int t = strlen(linha);

	if(i<t && linha[i]!='[') return NAO_LEU; //array possui []

	i++;

	le_element(&i, linha, numbers);	//array possui um element (ou nada)
									//vai atualizar posicao ate onde deve fechar o []

	if(i<t && linha[i]!=']') return NAO_LEU; //se nao fechar [], nao conta array
											//tambem trata o caso de ter value invalido entre []

	i++;
	*n = i;
	numbers[ARRAY]++;

	return LEU;

}

int le_string(int* n, char* linha, int* numbers){

	int t = strlen(linha);

	int i = *n;

	if(i<t && linha[i]!='"') return NAO_LEU; //para ler uma string, precisa comecar com aspas

	i++;
	
	while(i < t){ //percorre a string

		if(i<t-1 && linha[i]=='\\'){ //se achar um barra invertido

			if(linha[i+1]=='b' || linha[i+1]=='f' || linha[i+1]=='n' || linha[i+1]=='r'
				 || linha[i+1]=='t' || linha[i+1]=='u' || linha[i+1]=='\\' || linha[i+1]=='"') //nao pode ser nenhum desses depois dele
			return NAO_LEU; //se for um desses, nao conta string
		}

		if(i<t && linha[i]=='"'){ //se achar aspas

			if(linha[i+1]!=',' && linha[i+1]!=']' && linha[i+1]!=':' && linha[i+1]!='}') //se estiver no meio da string, nao conta string
				return NAO_LEU;

			else break; //se nao, e' o final da string, para na posicao
		}

		i++;
	}

	if(i<t && linha[i]!='"') return NAO_LEU; //se percorreu tudo e nao achou aspas de fechamento, nao conta string

	i++;
	*n = i;
	numbers[STRING]++; //se achou, atualiza as coisas

	return LEU;
}


int le_pair(int *n, char* linha, int* numbers){

	int i = *n;	
	int t = strlen(linha);
	int aux;

	if(le_string(&i, linha, numbers)== NAO_LEU) return NAO_LEU; //cada pair tem uma string

	if(i<t && linha[i]!=':') return NAO_LEU; //cada pair tem : depois da string

	i++;
	aux=i;

	le_value(&i, linha, numbers); //depois tem um value

	if(i==aux) return NAO_LEU; // se a posicao nao mudar, e' porque nao tem nenhum value, ai nao conta pair

	*n=i;
	numbers[PAIR]++;

	return LEU;
}


int le_member(int* n, char* linha, int* numbers){

	int i = *n;
	int t = strlen(linha);

	i--;
	do{

		i++;
		if(le_pair(&i, linha, numbers)== NAO_LEU) return NAO_LEU; //se nao le pair, nao conta member

	}while(i<t && linha[i]==','); //le uma sequencia de pairs entre virgulas

	*n=i;

	return LEU;
}


int le_object(int* n, char* linha, int* numbers){

	int i = *n;	
	int t = strlen(linha);

	if(i<t && linha[i]!='{') return NAO_LEU; //comeca com {

	i++;

	if(i<t && linha[i]=='}'){ //object pode ser apenas {}
		i++;
		*n=i;
		return LEU;
	}
	//se passar daqui, tem algo entre {}

	if(le_member(&i, linha, numbers)== NAO_LEU) return NAO_LEU; //se nao ler member, nao conta object

	if(i<t && linha[i]!='}') return NAO_LEU; //tem que fechar {} para contar

	i++;
	*n=i;
	numbers[OBJECT]++;

	return LEU;
}


void le_value(int* n, char* linha, int* numbers){ //le um dos tipos de value
													//cada funcao atualiza a posicao de leitura
													//le apenas um dos tipos
													//atualiza a posicao para a main somente se ler um value corretamente
	int i=*n;
	//em todas as funcoes (inclusive nas que compoem algumas dessas)
	//a posicao de leitura (n) e' enviada pela variavel criada na funcao (i)
	//so atualiza a variavel da main se toda a leitura de uma funcao der certo
	
	if(le_null(&i, linha, numbers)== LEU)
		*n = i;			
		
	else if(le_true(&i, linha, numbers)== LEU)
		*n = i;							
	
	else if(le_false(&i, linha, numbers)== LEU)
		*n = i;		
	
	else if(le_number(&i, linha, numbers)== LEU)
		*n = i;						
	
	else if(le_string(&i, linha, numbers)== LEU)
		*n = i;		
	
	else if(le_array(&i, linha, numbers)== LEU)
		*n = i;		
	
	else if(le_object(&i, linha, numbers)== LEU)
		*n = i;	
			
	return;
}


void le_linha(char** linha, int* fim){ //le uma linha da entrada, char por char

	int i =0;

	do{
		(*linha) = (char*)realloc((*linha), (i+1)*sizeof(char));

		if(scanf("%c", &(*linha)[i]) == EOF){ //se encontrar o final da entrada, para o loop de ler linhas (fim)
			(*linha)[i] = '\n';
			*fim = 1;
		}

		if((*linha)[i]==' ') i--; //tira os espacos da entrada (para facilitar)

		i++;

	}while((*linha)[i-1]!='\n' ); //le ate a quebra de linha

	(*linha)[i-1] = '\0';

	if((*linha)[i-2]=='\r') (*linha)[i-2]='\0'; //conserta bug do windows
}


void imprime_res(int fim, int nlinha, int* numbers){		

	if(fim==-1) printf("Error line %d\n", nlinha);		//fim e' -1 se ocorreu um erro
	printf("Number of Objects: %d\n", numbers[OBJECT]);
	printf("Number of Arrays: %d\n", numbers[ARRAY]);
	printf("Number of Pairs: %d\n", numbers[PAIR]);
	printf("Number of Strings: %d\n", numbers[STRING]);
	printf("Number of Numbers: %d\n", numbers[NUMBER]);
	printf("Number of Trues: %d\n", numbers[TRUE_]);
	printf("Number of Falses: %d\n", numbers[FALSE_]);
	printf("Number of Nulls: %d\n", numbers[NULL_]);
}


void desaloca(char* linha, int* numbers){
	
	free(linha);
	free(numbers);
}


int main(){

	char* linha = NULL; //recebe cada value
	int i = 0;		//posicao de leitura do value
	int fim = 0;	//determina o final das linhas (EOF)
	int t;
	int nlinha=0;	//guarda o indice da linha

	int* numbers = (int*)calloc(8, sizeof(int)); //contadores	

	do{
		le_linha(&linha, &fim); //pega a linha da entrada
		
		le_value(&i, linha, numbers); //le o value correspondente a linha lida
									  //para ler um value corretamente, "i" precisa percorrer toda a string sem dar erros

		t = strlen(linha);	

		if(i!=t) fim=-1; //se a posicao de leitura nao chegar ao final, houve algum erro. Para a leitura de linhas

		i=0; //se chegar, zera para a proxima linha

		nlinha++;

	}while(!fim); //fim muda de valor quando chega EOF (1) ou quando ha erro (-1)

	imprime_res(fim, nlinha, numbers);

	desaloca(linha, numbers);	

	return 0;
}