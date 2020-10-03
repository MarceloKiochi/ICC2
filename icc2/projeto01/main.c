/*
    Trabalho 1 de ICC2 - Octree
    programa que recebe coordenadas de um objeto e de um ponto e verifica se houve colisao entre os dois
    imprime 1 para colisao ou 0 para nao colisao

    Marcelo Kiochi Hatanaka 10295645
*/


#include<stdio.h>
#include<stdlib.h>

typedef struct caixa_ou_octante caixa;

typedef struct entrada{ //variaveis de entrada

	int semcaixa; // 1 para implementar caixa e 0 para nao implementar
	int nivel_octree;
	int n_triang; //numero de triangulos

}input;

typedef struct coord_ponto{

	double x;
	double y;
	double z;

}ponto;

typedef struct triangulo{ //struct para triangulos com ponteiro para pontos (3 pts cada triangulo)

	ponto* pontos_t;

}triang;

typedef struct caixa_ou_octante{

	ponto* pontos_c; //vertices
	double maiorX; //maiores e menores valores de x y e z
	double maiorY;
	double maiorZ;
	double menorX;
	double menorY;
	double menorZ;
	caixa* octante; //"nodos" da octree

}caixa;

void le_entrada(input** in, ponto** colisor, caixa** caixa_d, triang** tri){
    int i, j;

     *in = (input*)malloc(sizeof(input));
    scanf("%d%d%d", &(*in)->semcaixa, &(*in)->nivel_octree, &(*in)->n_triang); //le as 3 primeiras variaveis

	*colisor = (ponto*)malloc(sizeof(ponto));
	scanf("%lf%lf%lf", &(*colisor)->x, &(*colisor)->y, &(*colisor)->z); //le a coordenada do ponto colisor

	*caixa_d = (caixa*)malloc(sizeof(caixa));
	(*caixa_d)->pontos_c = (ponto*)malloc(8*sizeof(ponto));
	*tri = (triang*)malloc((*in)->n_triang*sizeof(triang));


	for(i = 0; i < (*in)->n_triang ; i++){                         //le os triangulos

		(*tri)[i].pontos_t = (ponto*)malloc(3*sizeof(ponto));

		for(j=0;j<3;j++){

			scanf("%lf %lf %lf", &(*tri)[i].pontos_t[j].x, &(*tri)[i].pontos_t[j].y, &(*tri)[i].pontos_t[j].z);

			if((*in)->semcaixa && i==0 && j==0){                 //define as coordenadas do primeiro ponto como menor e maior
				(*caixa_d)->maiorX = (*tri)[0].pontos_t[0].x;    //para ser comparado com os outros
				(*caixa_d)->menorX = (*tri)[0].pontos_t[0].x;    //só faz isso se for preciso implementar a caixa
				(*caixa_d)->maiorY = (*tri)[0].pontos_t[0].y;
				(*caixa_d)->menorY = (*tri)[0].pontos_t[0].y;
				(*caixa_d)->maiorZ = (*tri)[0].pontos_t[0].z;
				(*caixa_d)->menorZ = (*tri)[0].pontos_t[0].z;
			}

			else if((*in)->semcaixa){   //compara as coordenadas para definir os maiores e menores valores (para definir a caixa delimitadora)

				if((*tri)[i].pontos_t[j].x > (*caixa_d)->maiorX)
                    (*caixa_d)->maiorX = (*tri)[i].pontos_t[j].x;

				else if((*tri)[i].pontos_t[j].x < (*caixa_d)->menorX)
                    (*caixa_d)->menorX = (*tri)[i].pontos_t[j].x;

				if((*tri)[i].pontos_t[j].y > (*caixa_d)->maiorY)
                    (*caixa_d)->maiorY = (*tri)[i].pontos_t[j].y;

				else if((*tri)[i].pontos_t[j].y < (*caixa_d)->menorY)
                    (*caixa_d)->menorY = (*tri)[i].pontos_t[j].y;

				if((*tri)[i].pontos_t[j].z > (*caixa_d)->maiorZ)
                    (*caixa_d)->maiorZ = (*tri)[i].pontos_t[j].z;

				else if((*tri)[i].pontos_t[j].z < (*caixa_d)->menorZ)
                    (*caixa_d)->menorZ = (*tri)[i].pontos_t[j].z;
			}
		}
	}


	if(!(*in)->semcaixa){ //le os vertices da caixa delimitadora se for necessario

		for(i=0;i<8;i++){
			scanf("%lf %lf %lf", &(*caixa_d)->pontos_c[i].x, &(*caixa_d)->pontos_c[i].y, &(*caixa_d)->pontos_c[i].z);
		}

        //pega os maiores e menores valores de x y e z
		(*caixa_d)->menorX = (*caixa_d)->pontos_c[0].x; //o primeiro ponto sempre tera os menores valores
		(*caixa_d)->menorY = (*caixa_d)->pontos_c[0].y;
		(*caixa_d)->menorZ = (*caixa_d)->pontos_c[0].z;
		(*caixa_d)->maiorX = (*caixa_d)->pontos_c[7].x;
		(*caixa_d)->maiorY = (*caixa_d)->pontos_c[7].y;
		(*caixa_d)->maiorZ = (*caixa_d)->pontos_c[7].z; //o ultimo ponto sempre tera os maiores valores
	}
}

int verifica_vertices(input* in, triang* tri, caixa* caixa_d){ //verifica interseccao dos vertices dos triangulos no octante
    //os parametros sao as variaveis de entrada, os triangulos e o octante

    int i, j;

    for(i=0; i < in->n_triang; i++){ //passa pelos triangulos

		for(j=0; j<3; j++){ //passa pelos 3 pontos dos triangulos

			if(tri[i].pontos_t[j].x >= caixa_d->menorX && tri[i].pontos_t[j].x <= caixa_d->maiorX
                && tri[i].pontos_t[j].y >= caixa_d->menorY && tri[i].pontos_t[j].y <= caixa_d->maiorY
                && tri[i].pontos_t[j].z >= caixa_d->menorZ && tri[i].pontos_t[j].z <= caixa_d->maiorZ){
                        return 1; //retorna 1 se algum vertice estiver dentro do octante
            }
        }
	}
	return 0; //retorna 0 se não achar nenhum vertice dentro do octante
}

int verifica_triangulo(input* in, triang* tri, caixa* caixa_d){ //verifica interseccao dos pontos internos do triangulo com o octante
    int i, j, k;

    ponto* vetor = (ponto*)malloc(2*sizeof(ponto)); //aloca espaço para dois vetores
                                                    //tem coordenadas como de um ponto, por isso é do tipo "ponto"
	ponto* ponto_test = (ponto*)malloc(sizeof(ponto));

    for(i=0;i< in->n_triang;i++){ //passa pelos triangulos

        for(j=0;j<2;j++){ //dois vetores

            vetor[j].x = tri[i].pontos_t[j+1].x - tri[i].pontos_t[j].x; //cria dois vetores que auxiliam no "mapeamento" dos pontos
            vetor[j].y = tri[i].pontos_t[j+1].y - tri[i].pontos_t[j].y; //um vetor do primeiro para o segundo ponto
            vetor[j].z = tri[i].pontos_t[j+1].z - tri[i].pontos_t[j].z; //outro vetor do segundo para o terceiro ponto
        }

        for(j=0;j<=100;j++){ //regula o tamanho do primeiro vetor
            for(k=0;k<=j;k++){ //regula o segundo vetor

                ponto_test->x = tri[i].pontos_t[0].x + (vetor[0].x * j * 0.01) + (vetor[1].x * k * 0.01); //soma o primeiro ponto com os vetores para obter
                ponto_test->y = tri[i].pontos_t[0].y + (vetor[0].y * j * 0.01) + (vetor[1].y * k * 0.01); //um ponto interior do triangulo
                ponto_test->z = tri[i].pontos_t[0].z + (vetor[0].z * j * 0.01) + (vetor[1].z * k * 0.01); //tambem verifica arestas


                if(ponto_test->x >= caixa_d->menorX && ponto_test->x <= caixa_d->maiorX
                    && ponto_test->y >= caixa_d->menorY && ponto_test->y <= caixa_d->maiorY
                    && ponto_test->z >= caixa_d->menorZ && ponto_test->z <= caixa_d->maiorZ){
                        free(vetor);
                        free(ponto_test);
                        return 1; //retorna 1 se achar um ponto_test dentro do octante
                }
            }
        }
    }

    free(vetor);
    free(ponto_test);

    return 0; //retorna 0 se nao achar
}

void octantes(ponto* colisor, triang* tri, caixa* caixa_d, input* in, int* colisao){//divide os octantes e verifica interseccao do objeto alvo

    if(in->nivel_octree==0){ //termina a octree
		*colisao=1; //se passou por todos os niveis, o resultado e' 1
		return;
	}

	(in->nivel_octree)--;

	caixa_d->octante = (caixa*)malloc(sizeof(caixa)); //aloca espaço para o octante

	if(colisor->x >= (caixa_d->maiorX+caixa_d->menorX)/2){               //cria o octante onde esta o ponto colisor
		caixa_d->octante->menorX = (caixa_d->maiorX+caixa_d->menorX)/2;  //se x do colisor for MAIOR que o x medio da caixa
		caixa_d->octante->maiorX = caixa_d->maiorX;                      //o MENOR x do octante sera o x medio e
	}                                                                    //o MAIOR x sera o mesmo da caixa

	else{
		caixa_d->octante->maiorX = (caixa_d->menorX+caixa_d->maiorX)/2;  //se for MENOR que o x medio
		caixa_d->octante->menorX = caixa_d->menorX;                      //o MAIOR x do octante será o x medio
	}                                                                    //e o MENOR x sera o mesmo da caixa

	if(colisor->y >=(caixa_d->maiorY+caixa_d->menorY)/2){                //o mesmo com as coordenadas y e z
		caixa_d->octante->menorY = (caixa_d->maiorY+caixa_d->menorY)/2;
		caixa_d->octante->maiorY = caixa_d->maiorY;
	}

	else{
		caixa_d->octante->maiorY = (caixa_d->menorY+caixa_d->maiorY)/2;
		caixa_d->octante->menorY = caixa_d->menorY;
	}

	if(colisor->z >= (caixa_d->maiorZ+caixa_d->menorZ)/2){
		caixa_d->octante->menorZ = (caixa_d->maiorZ+caixa_d->menorZ)/2;
		caixa_d->octante->maiorZ = caixa_d->maiorZ;
	}

	else{
		caixa_d->octante->maiorZ = (caixa_d->menorZ+caixa_d->maiorZ)/2;
		caixa_d->octante->menorZ = caixa_d->menorZ;
	}

	*colisao = verifica_vertices(in, tri, caixa_d->octante); //verifica interseccao dos vertices
                                                            // a variavel recebe 0 se nao houver interseccao
                                                            // ou 1, se houver

	if(*colisao==0)
        *colisao = verifica_triangulo(in, tri, caixa_d->octante); //se nao detectar interseccao dos vertices, verifica os pontos internos

	if(*colisao)
        octantes(colisor, tri, caixa_d->octante, in, colisao); //se detectar interseccao, entra na recursao das octantes
                                                                //o octante e' passado como a caixa delimitadora e sera' dividido de novo
	free(caixa_d->octante);
}

void desaloca(input** in, ponto** colisor, caixa** caixa_d, triang** tri){
    int i;

    for(i = 0; i < (*in)->n_triang ; i++){
        free((*tri)[i].pontos_t);
    }
    free((*caixa_d)->pontos_c);
    free(*in);
    free(*colisor);
    free(*caixa_d);
    free(*tri);
}

int main(){
	int colisao;

    input* in; //variaveis de entrada
    ponto* colisor; //coordenadas do ponto colisor
    caixa* caixa_d; //variaveis da caixa delimitadora (com octantes dentro)
    triang* tri; //pontos dos triangulos

	le_entrada(&in, &colisor, &caixa_d, &tri); //le as entradas

    colisao = 0;

	if(caixa_d->menorX <= colisor->x && caixa_d->maiorX >= colisor->x
        && caixa_d->menorY <= colisor->y && caixa_d->maiorY >= colisor->y
        && caixa_d->menorZ <= colisor->z && caixa_d->maiorZ >= colisor->z){

				octantes(colisor, tri, caixa_d, in, &colisao); //se o ponto estiver dentro da caixa
	}                                                          //comeca a octree que pode mudar o resultado (colisao)

	printf("%d\n", colisao); //imprime o resultado

	desaloca(&in, &colisor, &caixa_d, &tri);

	return 0;

}
