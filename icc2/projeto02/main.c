/*
	Trabalho 2 de ICC2 - Labirinto
	Marcelo Kiochi Hatanaka
	10295645
	Acha todos os caminhos possiveis para as saidas de um labirinto dado na entrada
	de forma nao recusiva.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct numero{ //numero de pontos, camaras, segmentos e acoes

	int P, C, S, A; 

}numero;

typedef struct pontos{

	float x,y;
	int indice;
	int vl, tes; //vl = (1)se for uma saida e (0) se nao for
				//tes = (1) se fou o ponto da camara do tesouro e (0) se nao for
				// saidas e camara do tesouro serao tratadas pelos pontos, nao pelas camaras
}pontos;

typedef struct camaras{

	int v, o; //indice e valor logico

}camaras;

typedef struct segmentos{

	int b, e;	//indices das pontas dos segmentos

}segmentos;

typedef struct indice_especial{

	int tesouro, inicial, val_tes; //indice da camara do tesouro, camara inicial e valor do tesouro

}indice_especial;

void copiavetor(float* v1, float* v2, int n){ //copia um vetor em outro

	int i;
	for(i=0;i<n;i++){
		v1[i] = v2[i];
	}
}

int verifica_repet(float* saida, int tamanho, int k, int indtes){ //verifica que ja passou por uma camara

	int i;
	int cont = 0;

	for(i=1;i<tamanho;i++){
		if(k==saida[i]) cont++;
		if(cont>1) return 1; //nao pode passar duas vezes, antes nem depois do tesouro
		if(saida[i] == indtes && k!=indtes) cont=0; //se passar pela camara do tesouro, pode repetir
	}

	return cont; //retorna (0) se puder passar ou (1) se nao puder
}

int verifica_ordem(int s1, int s2, int isaida, float* saida[isaida], int* tamsaida){//verifica se um caminho deve ser printado antes ou depois de outro
																					//retorna (-1) se o primeiro for depois
																					//(1) se o primeiro for antes
	int aux1, aux2;
	int i;
	
	aux1 = tamsaida[s1]-1;
	aux2 = tamsaida[s2]-1;	

	if(saida[s1][aux1] < saida[s2][aux2]) return -1;		//pontuacao
	else if(saida[s1][aux1] > saida[s2][aux2]) return 1;

	if(saida[s1][aux1-1] < saida[s2][aux2-1]) return -1;	//soma geodesica
	else if(saida[s1][aux1-1] > saida[s2][aux2-1]) return 1;

	if(saida[s1][0] > saida[s2][0]) return -1;				//numero de indices
	else if(saida[s1][0] < saida[s2][0]) return 1;
	
	for(i=1; i< aux1-1; i++){
		if(saida[s1][i] > saida[s2][i]) return -1;			//indice por indice
	}

	return 1;
}
void le_pontos(numero** N, pontos** pts){

	int i;

	*N = (numero*)malloc(sizeof(numero));
	scanf("%d", &(*N)->P);

	*pts = (pontos*)malloc((*N)->P * sizeof(pontos));

	for(i=0; i < (*N)->P; i++){

		scanf("%f%f", &(*pts)[i].x, &(*pts)[i].y);
		(*pts)[i].indice = i+1; //salva o indice
		(*pts)[i].vl = 0;		//inicia sem valor logico e sem tesouro
		(*pts)[i].tes = 0;
	}	
}


void cria_mat_adj(int*** mat, numero* N){
	int i, j;

	*mat = (int**)malloc(N->P*sizeof(int*));

	for(i=0;i<N->P;i++){

		(*mat)[i] = (int*)malloc(N->P * sizeof(int));
	}

	for(i=0;i<N->P;i++){

		for(j=0;j<N->P;j++){

			(*mat)[i][j] = 0; //inicia tudo sem passagem
		}
	}
}

int le_camaras(camaras** cam, numero* N, pontos* pts){

	int i, aux1;

	scanf("%d", &N->C);

	*cam = (camaras*)malloc(N->C * sizeof(camaras));

	for(i = 0; i < N->C; i++){

		scanf("%d%d", &(*cam)[i].v, &(*cam)[i].o);

		if((*cam)[i].v <= 0 || (*cam)[i].v >N->P) return 1;	//retorna (1) se houver erro na entrada

		aux1 = (*cam)[i].v -1;
		pts[aux1].vl = (*cam)[i].o;	//aplica o valor logico aos pontos
	}

	return 0;
}
int le_segmentos(segmentos** seg, numero* N, int* mat[N->P]){

	int i, aux1, aux2;

	scanf("%d", &N->S);

	*seg = (segmentos*)malloc(N->S * sizeof(segmentos));

	for(i = 0; i < N->S; i++){

		scanf("%d%d", &(*seg)[i].b, &(*seg)[i].e);

		aux1 = sqrt(pow((*seg)[i].b, 2)) -1;
		aux2 = sqrt(pow((*seg)[i].e, 2)) -1;

		if(aux1==-1 || aux1 == aux2 || aux2 > N->P-1) return 1;

		if((*seg)[i].e>0 && (*seg)[i].b>0){
			mat[aux1][aux2] = 1; //coloca passagem na matriz das passagens
			mat[aux2][aux1] = 1;
		}		
	}

	return 0;
}
int le_tesouro_e_inicio(indice_especial** ind, segmentos** acoes, numero* N, pontos* pts){

	int i, aux1;

	*ind = (indice_especial*)malloc(sizeof(indice_especial));

	scanf("%d%d", &(*ind)->tesouro, &(*ind)->val_tes);

	if((*ind)->tesouro <= 0 || (*ind)->tesouro > N->P) return 1;

	aux1 = (*ind)->tesouro -1;
	pts[aux1].tes =1; //salva o ponto que contem o tesouro
	
	scanf("%d", &N->A);

	*acoes = (segmentos*)malloc(N->A * sizeof(segmentos));

	for(i=0; i < N->A; i++){

		scanf("%d%d", &(*acoes)[i].b, &(*acoes)[i].e);
	}

	scanf("%d", &(*ind)->inicial);

	if((*ind)->inicial <= 0 || (*ind)->inicial > N->P) return 1;

	return 0;
}

void cria_saida(int** tamsaida, int**ordem, float***saida){

	*tamsaida = malloc(sizeof(int)); //guarda o tamanho de cada caminho

	(*tamsaida)[0]=2;

	*ordem = (int*)malloc(sizeof(int)); //guarda a ordem da saida

	(*ordem)[0]=0;

	*saida = (float**)malloc(sizeof(float*)); //guarda os caminhos

	(*saida)[0] = (float*)malloc(2*sizeof(float));
}
void atualiza_posicao(int* valor_mat_adj, int* ant, int* atual, int* post, int i, int*tamsaida,
					 int isaida, float**saida[isaida], int* ipts){

	int aux1;

	(*valor_mat_adj) *= 2;	//marca a passagem na matriz	

	*ant = *atual;			//atualiza a posicao anterior

	*post = -1;				//posterior sera "zerada"

	*atual = i;				//atualiza posicao atual

	tamsaida[isaida]++;		//aumenta o tamanho do caminho
	aux1 = tamsaida[isaida];				

	(*saida)[isaida] = (float*)realloc((*saida)[isaida], aux1*sizeof(float));

	(*ipts)++;
	(*saida)[isaida][*ipts] = (*atual)+1; //salva a posicao no caminho
}

void acha_tesouro(int* camara_tesouro, indice_especial* ind, segmentos* acoes, numero* N, int* pontuacao,
					int* mat[N->P]){ //entrou na camara do tesouro

	int i, aux1, aux2;

	if(*camara_tesouro==0){ //se for a primeira vez que chega

		*pontuacao = ind->val_tes; 

		for(i=0; i < N->A; i++){ //executa as acoes

			if(acoes[i].b < 0 || acoes[i].e<0){

				aux1 = sqrt(pow(acoes[i].b, 2)) -1;
				aux2 = sqrt(pow(acoes[i].e, 2)) -1;

				if(mat[aux1][aux2]==1){ //muda para valores diferentes de 0
					mat[aux1][aux2]=-3; //para serem identificados caso precisem retornar ao estado anterior
					mat[aux2][aux1]=-3; 
				}
				else if(mat[aux1][aux2]==2){
					mat[aux1][aux2]=-6;
					mat[aux2][aux1]=-6; 
				}
				
			}
			else{						
				aux1 = acoes[i].b-1;
				aux2 = acoes[i].e-1;

				if(mat[aux1][aux2]==0){
					mat[aux1][aux2] = -1;
					mat[aux2][aux1] = -1;
				}
			}
		}	
	}
	
	(*camara_tesouro)++;	
}

void acha_saida(int* isaida, float** saida[*isaida], int** tamsaida, int** ordem, float* somaG, pontos* pts,
				int pontuacao, int ipts){

	int i, aux1, aux2;

	(*saida)[(*isaida)][0]=ipts;	//salva o numero de indices		

	(*tamsaida)[(*isaida)]+=2;
	aux1 = (*tamsaida)[(*isaida)];
	(*saida)[(*isaida)] = (float*)realloc((*saida)[(*isaida)], aux1*sizeof(float));	//aumenta a matriz para novos caminhos

	(*somaG) = 0;
	for(i=1;i<(*tamsaida)[(*isaida)]-3;i++){ //faz a soma geodesica
		aux1 = (*saida)[(*isaida)][i]-1;
		aux2 = (*saida)[(*isaida)][i+1]-1;
		(*somaG) = (*somaG) + sqrt((pow((pts[aux1].y - pts[aux2].y), 2)) + pow((pts[aux1].x - pts[aux2].x), 2));
	}						

	//salva os dois ultimos valores do vetor de saida
	aux1 = (*tamsaida)[(*isaida)]-1;
	(*saida)[(*isaida)][aux1-1] = (*somaG); 
	(*saida)[(*isaida)][aux1] = pontuacao - (*somaG); 

	//ordena o vetor que guarda a ordem das saidas
	(*ordem) = (int*)realloc((*ordem), ((*isaida)+1)*sizeof(int));

	(*ordem)[(*isaida)] = (*isaida);

	aux1 = (*ordem)[(*isaida)];

	i=(*isaida);
	if(i>0){				
		aux2 = verifica_ordem(aux1, (*ordem)[i-1], (*isaida), (*saida), (*tamsaida));
		while(i>0 && aux2>0){	//se o novo vetor for antes do outro, troca de posicao				
			(*ordem)[i]=(*ordem)[i-1];
			i--;
			aux2 = verifica_ordem(aux1, (*ordem)[i-1], (*isaida), (*saida), (*tamsaida));
		}
		(*ordem)[i]=aux1;				
	}//insertion sort		
	
	//cria mais um caminho
	(*isaida)++;
	(*saida) = (float**)realloc((*saida), ((*isaida)+1)*sizeof(float*));
	(*tamsaida) = (int*)realloc((*tamsaida), ((*isaida)+1)*sizeof(int));
	(*tamsaida)[(*isaida)] = (*tamsaida)[(*isaida)-1]-2;
	(*saida)[(*isaida)] = (float*)malloc((*tamsaida)[(*isaida)]*sizeof(float));				

	copiavetor((*saida)[(*isaida)], (*saida)[(*isaida)-1], ((*tamsaida)[(*isaida)]));	
}

void volta_posicao(pontos* pts, int* camara_tesouro, int* pontuacao, numero* N, int* mat[N->P], int* voltando,
				int* ipts, int* posterior, int* atual, int* anterior, int isaida, float* saida[isaida], int* tamsaida){

	int i, j;

	if(pts[(*atual)].tes == 1){ //se estiver voltando da camara do tesouro

		(*camara_tesouro)--; 

		if((*camara_tesouro) == 0){ //se agora ele nao passou mais pela camara do tesouro

			(*pontuacao) = 0;

			for(i=0; i < N->P; i++){

				for(j=0;j<N->P;j++){

					if(mat[i][j]<0) mat[i][j] = mat[i][j] / (-3); //volta as mudancas da camara do tesouro
				}
			}
		}
	}
	(*voltando) = 1;

	//atualiza a posicao para o anterior				
	(*ipts)--;
	(*posterior) = (*atual);
	(*anterior) = saida[isaida][(*ipts)-1]-1;
	(*atual) = saida[isaida][(*ipts)]-1;

	if(mat[(*atual)][(*posterior)]==2) mat[(*atual)][(*posterior)]=1;           //desmarca o caminho andado
	else if(mat[(*atual)][(*posterior)]==-2) mat[(*atual)][(*posterior)]=-1;
	else if(mat[(*atual)][(*posterior)]==4) mat[(*atual)][(*posterior)]=2;
	else if(mat[(*atual)][(*posterior)]==-4) mat[(*atual)][(*posterior)]=-2;

	tamsaida[isaida]--;	
}

void imprime_saida(indice_especial* ind, int isaida, int*ordem, int*tamsaida, float* saida[isaida]){

	int aux1, i, j;
	
	printf("%d %d\n", ind->tesouro, ind->val_tes);
	
	for(i=0;i<isaida;i++){		
		aux1 = ordem[i];
		for(j=0;j<tamsaida[aux1];j++){			
			printf("%.0f ", roundf(saida[aux1][j]));
		}
		printf("\n");
	}
}

void desaloca(int n, int isaida, int* mat[n], float* saida[isaida], int tamsaida[isaida], int*ordem, numero* N,
				pontos* pts, camaras* cam, segmentos* seg, indice_especial* ind, segmentos* acoes){

	int i;
	for(i=0;i<n;i++){
		free(mat[i]);
	}
	free(mat);
	for(i=0;i<isaida;i++){
		free(saida[i]);
	}
	free(saida);
	free(tamsaida);
	free(ordem);
	free(N);
	free(pts);
	free(cam);
	free(seg);
	free(ind);
	free(acoes);
}
int main(){
	int i, aux1; //variaveis auxiliares
	
	int voltando = 0; //indica se esta voltando(1) ou nao(0)

	float somaG;

	int pontuacao = 0;

	int camara_tesouro=0; //quantas vezes passa pela camara do tesouro
	
	int atual, anterior=-1, posterior=-1; //guardam posicoes uteis do labirinto

	//indices para a matriz da saida
	int isaida = 0; //qual caminho
	int ipts = 0; //qual ponto do caminho

	int* tamsaida; //vetor que guarda o tamanho de cada caminho
	int* ordem; //vetor que guarda a ordem dos caminhos a serem printados
	float** saida; //matriz de saida
	cria_saida(&tamsaida, &ordem, &saida);

	numero* N; //struct que guarda os 'N'
	pontos* pts; 
	le_pontos(&N, &pts);

	int** mat; //matriz de adjacencia para verificar se ha passagens
	cria_mat_adj(&mat, N);

	camaras* cam; 
	if(le_camaras(&cam, N, pts)) return 0;

	segmentos* seg;
	if(le_segmentos(&seg, N, mat)) return 0;	

	indice_especial* ind; //guarda indice e valor do tesouro e o indice inicial
	segmentos* acoes;
	if(le_tesouro_e_inicio(&ind, &acoes, N, pts)) return 0;	

	atual = ind->inicial - 1;
	ipts++;	
	saida[isaida][ipts] = atual+1; //salva o inicio na saida


	do{		
		if(pts[atual].tes ==1 && !voltando){	//se chegar na camara do tesouro e nao estiver voltando
			
			acha_tesouro(&camara_tesouro, ind, acoes, N, &pontuacao, mat);
		}	
		

		if(pts[atual].vl ==1 && !voltando){	//se achar uma saida e nao estiver voltando

			acha_saida(&isaida, &saida, &tamsaida, &ordem, &somaG, pts, pontuacao, ipts);						
		}			
		
		for(i=posterior+1;i<N->P;i++){ //procura uma passagem para andar
										//se estiver voltando, o posterior e' a camara de onde veio
										//comeca a procurar a partir dele, para nao repetir caminho
										//se estiver avancando, e' -1 (comeca do 0 mesmo)

			aux1 = verifica_repet(saida[isaida], tamsaida[isaida], i+1, ind->tesouro); //verifica se ja passou por essa camara

			//muda a posicao se:
			// 	- houver passagem ainda nao explorada, se nao for a camara anterior e se nao houver repeticao;
			// 	- ou se houver passagem ainda nao explorada, se for a camara anterior e estiver na camara do tesouro pela primeira vez;
			//	- ou se houver passagem ja explorada, nao for a camara anterior e ja tiver passado pelo tesouro 
			// 		(tambem nao pode repetir duas vezes depois de ter passado pelo tesouro)
			if((mat[atual][i]==1 || mat[atual][i]==-1) && i!=anterior && aux1==0){				
				atualiza_posicao(&mat[atual][i], &anterior, &atual, &posterior, i, tamsaida, isaida, &saida, &ipts);
				break;
			}
			else if((mat[atual][i]==1 || mat[atual][i]==-1) && i==anterior && pts[atual].tes==1 && camara_tesouro==1){				
				atualiza_posicao(&mat[atual][i], &anterior, &atual, &posterior, i, tamsaida, isaida, &saida, &ipts);
				break;
			}
			else if((mat[atual][i]==2 || mat[atual][i]==-2) && i!=anterior && camara_tesouro>0 && aux1==0){				
				atualiza_posicao(&mat[atual][i], &anterior, &atual, &posterior, i, tamsaida, isaida, &saida, &ipts);
				break;
			}
		}			
		
		voltando = 0;
		if(i==N->P){ //se nao achar nenhuma passagem, volta uma posicao
			volta_posicao(pts,&camara_tesouro,&pontuacao,N,mat,&voltando,&ipts,&posterior,&atual,&anterior,isaida,saida,tamsaida);
					
		}				
	}while(tamsaida[isaida]>2);	//para quando voltar ao inicio
								//significa que nao ha mais passagens nao exploradas para nenhum lugar

	imprime_saida(ind, isaida, ordem, tamsaida, saida);		

	desaloca(N->P, isaida+1, mat, saida, tamsaida, ordem, N, pts, cam, seg, ind, acoes);

	return 0;
}