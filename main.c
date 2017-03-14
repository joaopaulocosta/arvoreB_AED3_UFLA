#include <stdio.h>
#include <string.h>
#include "arvoreB.h"

int main(){
	char comando[2];
	carregarArquivos();
	do{
		scanf("%s",comando);
		if(comando[0] == 'I' && comando[1] == 'R'){
			inserirDados();
		}
		else if(comando[0] == 'R' && comando[1] == 'R'){
			char *chave = lerRegistro();
			removerRegistro(chave);
			free(chave);
		}
		else if(comando[0] == 'B' && comando[1] == 'R'){
			busca();
		}
		else if(comando[0] == 'I' && comando[1] == 'A'){
			FILE *arqIndice = fopen("index.dat","r+b");
			long int RRNcorrente;
			fread(&RRNcorrente,sizeof(int long),1,arqIndice);
			imprimirArvore(RRNcorrente);
			fclose(arqIndice);
		}
		else if(comando[0] == 'F' && comando[1] == 'M'){
		}
	}while(comando[0] != 'F' && comando[1] != 'M');
}
