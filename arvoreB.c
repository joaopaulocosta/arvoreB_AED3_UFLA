#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvoreB.h"

/*****************************************************************************************************************************/

/* Função que carrega uma página do arquivo de índice para a memória principal,
 * além dos campos da página ele salva também seu RRN.
 * @param - RRN endereço da página no arquivo de índice
 * return - endereço para a página na memória principal */
struct PaginaPrimaria *carregarPagina(long int RRN){
	FILE *arqIndice = fopen("index.dat","r+b");
	struct PaginaSecundaria *newPageS = (struct PaginaSecundaria*) malloc(sizeof(struct PaginaSecundaria));
	fseek(arqIndice,RRN,0);
	fread(&newPageS->NE,sizeof(short int),1,arqIndice);
	int i,j;
	for(i=0; i < MAX; i++){
		for(j=0; j < 7; j++){
			fread(&newPageS->chaves[i].codigo[j],sizeof(char),1,arqIndice);
		}			
		fread(&newPageS->chaves[i].RRN,sizeof(long int),1,arqIndice);
	}
	for(i =0; i< MAX+1; i++)
		fread(&newPageS->filhos[i],sizeof(long int),1,arqIndice);
	fclose(arqIndice);
	struct PaginaPrimaria *newPageP = secundariaToPrimaria(newPageS);
	newPageP->RRN = RRN;
	return newPageP;
}

/*****************************************************************************************************************************/

/* Função que recebe uma página e a salva no arquivo de índice de acordo com seu RRN correspondente.
 * @param - estrutura de página primaria, q contem o RRN.
 * @return - 1 se a estrutura foi salvo com sucesso e free na página passada como parâmetro.
 * @return - 0 se a estrutura não foi salvo com sucesso e página passada como parâmetro continua na memória. */
int salvarPagina(struct PaginaPrimaria * pageP){
	FILE *arqIndice = fopen("index.dat","r+b");
	fseek(arqIndice,pageP->RRN,0);
	fwrite(&pageP->NE,sizeof(short int),1,arqIndice);
	int i,j;
	for(i=0; i < MAX; i++){
		for(j=0; j < 7; j++){
			fwrite(&pageP->chaves[i].codigo[j],sizeof(char),1,arqIndice);
		}
		fwrite(&pageP->chaves[i].RRN,sizeof(long int),1,arqIndice);
	}
	for(i =0; i< MAX+1; i++)
		fwrite(&pageP->filhos[i],sizeof(long int),1,arqIndice);
	fclose(arqIndice);
	return 1;
}

/*****************************************************************************************************************************/

/*Função que verifica se os arquivos de dados e índice ja estão criados, caso não ela cria-os. */
void carregarArquivos(){
	FILE *arqIndice,*arqDados;
	if((arqIndice = fopen("index.dat","r+b")) == NULL){
		arqIndice = fopen("index.dat","w+b");
		long int raiz = -1;
		fwrite(&raiz,sizeof(long int),1,arqIndice);
		raiz = tamArquivo(arqIndice);
		rewind(arqIndice);
		fwrite(&raiz,sizeof(long int),1,arqIndice);
		struct PaginaSecundaria * newPage = (struct PaginaSecundaria*) malloc(sizeof(struct PaginaSecundaria));
		inicializaPagina(newPage);
		fwrite(&newPage->NE,sizeof(short int),1,arqIndice);
		int i,j;
		for(i=0; i < MAX; i++){
			for(j=0; j < 7; j++)
				fwrite(&newPage->chaves[i].codigo[j],sizeof(char),1,arqIndice);
			fwrite(&newPage->chaves[i].RRN,sizeof(long int),1,arqIndice);
		}
		for(i =0; i< MAX+1; i++)
			fwrite(&newPage->filhos[i],sizeof(long int),1,arqIndice);
	}
	if((arqDados = fopen("data.dat","r+b")) == NULL){
		arqDados = fopen("data.dat","w+b");
	}
	fclose(arqDados);
	fclose(arqIndice);
}

/*****************************************************************************************************************************/

/*Função que converte uma página primária em uma página secundária.
 *@param - pageP - Página primária que será convertida e será desalocada da memória.
 *return - endereço para página secundária que foi alocada na memória. */
struct PaginaSecundaria * primariaToSecundaria(struct PaginaPrimaria *pageP){
	struct PaginaSecundaria *pageS = (struct PaginaSecundaria*) malloc(sizeof(struct PaginaSecundaria));
	pageS->NE = pageP->NE;
	int i,j;
	for(i = 0; i < MAX; i++){
		for(j = 0; j < 7; j++)
			pageS->chaves[i].codigo[j] = pageP->chaves[i].codigo[j];
		pageS->chaves[i].RRN = pageP->chaves[i].RRN;
	}
	for(i = 0; i < MAX+ 1; i++)
		pageS->filhos[i] = pageP->filhos[i];
	free(pageP);
	return pageS;
}

/*****************************************************************************************************************************/


/*Função que converte uma página secundária em uma página primária.
 *@param - pageP - Página secundária que será convertida e será desalocada da memória.
 *return - endereço para página primária que foi alocada na memória. */
struct PaginaPrimaria * secundariaToPrimaria(struct PaginaSecundaria *pageS){
	struct PaginaPrimaria *pageP = (struct PaginaPrimaria*) malloc(sizeof(struct PaginaPrimaria));
	pageP->NE = pageS->NE;
	int i,j;
	for(i = 0; i < MAX; i++){
		for(j = 0; j < 7; j++)
			pageP->chaves[i].codigo[j] = pageS->chaves[i].codigo[j];
		pageP->chaves[i].RRN = pageS->chaves[i].RRN;
	}
	for(i = 0; i < MAX+ 1; i++)
		pageP->filhos[i] = pageS->filhos[i];
	free(pageS);
	return pageP;
}

/*****************************************************************************************************************************/

/*Função que verifica se a entrada na inserção de dados está correspondente com o padrão, nos campos
 *código da disciplina que obrigatóriamente tem tamanho 6 e o código do departamento tem tamanho 3.
 *@param - entrada[1024] - string contendo todas as informações da inserção, onde os campos são separados
 *pelo caracter @.
 *return - 1 se os campos estiverem de acordo com o padrão.
 *return - 0 se os campos não estiverem de acordo com o padrão. */
int verificaEntrada(char entrada[1024]){
	int cont = 0,i = 0, contA;
	while(entrada[i] != '@' && i < 1023){
		cont++;
		i++;
	}
	if(cont != 6)
		return 0;
	cont = 0;
	i++;
	while(entrada[i] != '@' && i < 1023){
		cont++;
		i++;
	}
	cont = 0;
	i++;
	while(entrada[i] != '@' && i < 1023){
		cont++;
		i++;
	}
	cont = 0;
	i++;
	while(entrada[i] != '@' && i < 1023){
		cont++;
		i++;
	}
	if(cont != 3)
		return 0;
	return 1;
}

/*****************************************************************************************************************************/

/*Função responsável pela opção de menu inserir dados, o usuário entra com a string contendo os dados para a inserção
 *da disciplina, entram é verifica se a string está dentro do padrão com 6 caracteres para o código da disciplina e 3
 *para o código do departamento, caso não esteja é emprido erro. Em seguida a string é inserida no arquivo de dados e entam
 * é criado uma chave para inserção no índice. */ 
void inserirDados(){
	char registro[1024];
	struct Chave chave;
	int verifica = 0, i = -1;
	char aux;
	scanf("%c",&aux);
	
	do{								/*lendo caracter por caracter */
		i++;
		scanf("%c",&registro[i]);
	}while(registro[i] != '\n');
	
	if(!verificaEntrada(registro)){
		printf("ERRO\n");
		return;
	}
	else{
		registro[i] = '\0';
		for(i=0; i < 6; i++)								/*copiando a chave */
			chave.codigo[i] = registro[i];
		chave.codigo[6] = '\0';
		i = 0;
		FILE *arqDados = fopen("data.dat","a+b");
		FILE *arqIndice = fopen("index.dat","a+b");
		long int endereco = tamArquivo(arqDados);
		fseek(arqDados,0,SEEK_END);
		chave.RRN = tamArquivo(arqDados);
		while(registro[i] != '\0'){							/*gravando o registro no arquivo de dados */
			fwrite(&registro[i],sizeof(char),1,arqDados);
			i++;
		}
		char f = '#';
		fwrite(&f,sizeof(char),1,arqDados);
		fclose(arqDados);
		long int RRNraiz;
		rewind(arqIndice);
		fread(&RRNraiz,sizeof(long int),1,arqIndice);
		fclose(arqIndice);
		struct Chave chaveProm;
		long int filhoProm;
		if(inserirIndice(RRNraiz,chave, &chaveProm, &filhoProm)){ /* condição que realiza a função do procedimento driver */
			struct PaginaSecundaria *newPageS = (struct PaginaSecundaria *) malloc(sizeof(struct PaginaSecundaria));
			inicializaPagina(newPageS);
			int i;
			for(i = 0; i < 7; i++)
				newPageS->chaves[0].codigo[i] = chaveProm.codigo[i];
			newPageS->chaves[0].RRN = chaveProm.RRN;
			newPageS->filhos[0] = RRNraiz;
			newPageS->filhos[1] = filhoProm;
			newPageS->NE++;
			struct PaginaPrimaria *newPageP = secundariaToPrimaria(newPageS);
			FILE *arqIndice = fopen("index.dat","r+b");
			long int newRaiz = tamArquivo(arqIndice);
			newPageP->RRN = newRaiz;
			salvarPagina(newPageP);
			rewind(arqIndice);
			fwrite(&newRaiz,sizeof(long int),1,arqIndice);
			fclose(arqIndice);
		}		
	}
}

/*****************************************************************************************************************************/
 
/*Função que inseri uma chave no índice do arquivo de dados, que está armazenado em uma estrutura tipo árvore B no arquivo
 * de índice.
 *@param - RRNcorrente - endereço da página corrente dentro do arquivo de índice.
 *@param - chave - código da chave que será inserida no índice.
 *@param - chavePromovida - vetor que armazenará a chave, caso haja promoção.
 *@param - filhoPrimovido - ponteiro que armazenará o filho da chavem caso essa seja promovida.
 *return - 1 caso haja promoção.
 *return - 0 caso não haja promoção. */
int inserirIndice(long int RRNcorrente,struct Chave chave, struct Chave * chavePromovida ,long int * filhoPromovido){	
	if(RRNcorrente == -1){
		int i;
		for(i=0; i < 7; i++)
			chavePromovida->codigo[i] = chave.codigo[i];
		chavePromovida->RRN = chave.RRN;
		*filhoPromovido = -1;
		return 1;
	}
	else{
		struct PaginaPrimaria *page = carregarPagina(RRNcorrente);
		page->RRN = RRNcorrente;
		int posBusca;
		int a;
		if(a = buscaBinaria(page->chaves,chave.codigo,&posBusca)){
			printf("Erro\n");
			return 0;
		}
		else{
			if(page->NE == MAX)
				buscaBinariaSplit(page->chaves,chave.codigo,&posBusca);
			struct Chave chaveP;
			long int filhoP;
			int i;
			int valorRetorno = inserirIndice(page->filhos[posBusca],chave,&chaveP,&filhoP);
			if(!valorRetorno){
				return valorRetorno;
			}
			else if (page->NE < MAX){
				inserirChave(chaveP,filhoP,RRNcorrente);
				printf("SUCESSO");
				return 0;
			}
			else{
				struct PaginaSecundaria *newPageS = (struct PaginaSecundaria*) malloc(sizeof(struct PaginaSecundaria));
				inicializaPagina(newPageS);
				struct PaginaPrimaria * newPage = secundariaToPrimaria(newPageS);
				FILE *arqIndice = fopen("index.dat","r+b");
				newPage->RRN = tamArquivo(arqIndice);
				split( chaveP, filhoP, page, chavePromovida, filhoPromovido, newPage);
				salvarPagina(page);
				salvarPagina(newPage);
				fclose(arqIndice);
				return 1;		
			}
		}
	}
}

/*****************************************************************************************************************************/

/*Função que recebe uma chave e uma RRN da página ao qual será inserida a chave.
 *@param - chave - String de tamanho 7 correspondente a chave que será inserida.
 *@param - filho - Valor do RRN correspondente ao filho da chave inserida.
 *@param - RRN - endereço da página que será inserida a chave. */ 

void inserirChave(struct Chave chave,long int filho, long int RRN){
	int i,j;
	struct PaginaPrimaria *page = carregarPagina(RRN);
	int pos = 0;
	if(!buscaBinaria(page->chaves,chave.codigo,&pos)){
		if(pos < page->NE){
			int j,i = MAX-1;
			while( i > pos){
				for(j = 0; j < 7; j++)
					page->chaves[i].codigo[j] = page->chaves[i-1].codigo[j];
				page->chaves[i].RRN = page->chaves[i-1].RRN;
				i--;
			}
			i = MAX;
			while( i > pos+1){
				page->filhos[i] = page->filhos[i-1];
				i--;
			}
		}
	}
	for(i = 0; i < 7; i++){
		page->chaves[pos].codigo[i] = chave.codigo[i];
	}
	page->chaves[pos].RRN = chave.RRN;
	page->filhos[pos+1] = filho;
	page->NE++;
	salvarPagina(page);
}

/*****************************************************************************************************************************/

/*Função que realiza o split.
 * @param - struct Chave chave - chave que será inserida na página com overflow.
 * @param - long int filhoP - filho da chave que será inserida.
 * @param - struct PaginaPrimaria *page - ponteiro para a página que está com overflow.
 * @param - struct Chave * chavePromovida - ponteiro para a chave que armazenará a chave que será promovida pelo split.
 * @param lont int *filhoPromovido - ponteiro para o filho que será promovido.
 * @param - struct PaginaPrimaria *newPage - ponteiro para a página que será criada. */
void split( struct Chave chave, long int filhoP, struct PaginaPrimaria * page, struct Chave * chavePromovida,
 long int * filhoPromovido, struct PaginaPrimaria * newPage){
	struct Chave chaves[MAX+1]; /* vetor que receberá todas as chaves */
	long int filhos[MAX+2];
	int i,j;
	/*redistribuição no vetor de chaves almentado */
	for(i = 0; i < MAX; i++){
		for(j = 0; j< 7; j++){
			chaves[i].codigo[j] = page->chaves[i].codigo[j];
		}
		chaves[i].RRN = page->chaves[i].RRN;
	}
	for(i = 0; i < MAX+1; i++){ /*redistribuindo os filhos em um vetor almentado */
		filhos[i] = page->filhos[i];
	}
	int pos;
	
	buscaBinariaSplit(chaves,chave.codigo,&pos);
	if(pos < MAX){ /* rearanjando o vetor para inserção da chave */
		for(i = MAX; i > pos; i--){
			for(j = 0; j < 7; j++)
				chaves[i].codigo[j] = chaves[i-1].codigo[j];
			chaves[i].RRN = chaves[i-1].RRN;
		}
		for(i = MAX+1; i > pos; i--)
			filhos[i] = filhos[i-1];
	}
	for(i = 0; i < 7 ; i++)
		chaves[pos].codigo[i] = chave.codigo[i];
	chaves[pos].RRN = chave.RRN;
	filhos[pos+1] = filhoP;
	int auxRRN = page->RRN;
	struct PaginaSecundaria *aux = primariaToSecundaria(page);
	inicializaPagina(aux);
	page =  secundariaToPrimaria(aux);
	page->RRN = auxRRN;
	/*redistribuindo as chaves e os filhos entre a página e a nova página */		
	for(i = 0; i < (MAX/2) ; i++){
		for( j = 0; j < 7 ; j++)
			page->chaves[i].codigo[j] = chaves[i].codigo[j];
		page->chaves[i].RRN = chaves[i].RRN;
	}
	for(i = 0; i <= (MAX+1)/2; i++){
		page->filhos[i] = filhos[i];
	}
		
	for(i = MAX+1; i > (MAX+1)/2; i--){
		newPage->filhos[i-1-(MAX/2)] = filhos[i];
	}
	
	for( i = MAX; i > (MAX/2); i--){
		for( j = 0; j < 7 ; j++)
			newPage->chaves[i-1-(MAX/2)].codigo[j] = chaves[i].codigo[j];
		newPage->chaves[i-1-(MAX/2)].RRN = chaves[i].RRN;
	}
	newPage->NE = MAX/2;
	page->NE = MAX/2;
	for(i = 0; i < 7; i++)
		chavePromovida->codigo[i] = chaves[MAX/2].codigo[i];
	chavePromovida->RRN = chaves[MAX/2].RRN;
	*filhoPromovido = newPage->RRN;
	
}

/*****************************************************************************************************************************/

/*Função que realiza uma busca em toda a estrutura a procura de determinada chave. Se a chave não for encontrada na página corrente
 * a função é chamada recursivamente.
 * @param - long int RRN - RRN correspondete a página corrente.
 * @param - char chave[7] - vetor de caracteres correspondente a chave que se deseja buscar.
 * @param - long int * paginaBusca - Ponteiro que armazenará o RRN da página que a chave foi encontrada.
 * @param - int * posBUsca - Ponteiro que armazenará a posição no vetor de chaves da página referente a chave procurada.
 * @param - int *pagPai - Ponteiro que armazenará a posição da página que é pai da página que se encontra a chave buscada. */ 
int buscaIndice(long int RRN, char chave[7], long int * paginaBusca, int * posBusca,long int *pagPai ){
	if(RRN == -1){
		return 0;
	}
	else{
		struct PaginaPrimaria *page = carregarPagina(RRN);
		if(buscaBinaria(page->chaves,chave,posBusca)){
			*paginaBusca = RRN;
			return 1;
		}
		else{
			*pagPai = page->RRN;
			int pos = *posBusca;
			return buscaIndice(page->filhos[pos], chave, paginaBusca, posBusca,pagPai);
		}
	}
}
/*****************************************************************************************************************************/

/*Função que imprimi um registro do arquivo de dados, apartir de seu RRN.
 * @param - long int RRN - RRN correspondente do registro a ser imprimido */
void imprimirRegistro(long int RRN){
	FILE *arqDados = fopen("data.dat","r+b");
	fseek(arqDados,RRN,0);
	char aux;
	do{
		fread(&aux,sizeof(char),1,arqDados);
		if(aux == '@')
			printf(" ");
		else if(aux == '#')
			break;
		else
			printf("%c",aux);
	}while(aux != '#');
	printf("\n");
	fclose(arqDados);
}

/*****************************************************************************************************************************/

/*Função que lê da entrada padrão, o código referente a chave de registro.
 * @return - vetor de char com 7 caracteres referente a chave de algum registro. */
char * lerRegistro(){
	char aux;
	scanf("%c",&aux);
	char *chave =(char*) malloc(7* sizeof(char));
	int i;
	for(i = 0; i < 6; i++)
		scanf("%c",&chave[i]);
	chave[6] = '\0';
	return chave;
}

/*****************************************************************************************************************************/
/*Função correspondente a opção do menu buscar registro, ela chama a função lerRegistro que recebe a chave, entam
 * ela lê o RRN da raiz, entam é feito a busca, se a busca for bem sucedida o registro é imprimido. */
void busca(){
	char *chave = lerRegistro();
	FILE *arqIndice = fopen("index.dat","r+b");
	long int RRNcorrente;
	fread(&RRNcorrente,sizeof(int long),1,arqIndice);
	fclose(arqIndice);
	long int pagBusca,pagPai = -1;
	int pos;
	if(!buscaIndice(RRNcorrente,chave,&pagBusca,&pos,&pagPai))
		printf("Erro");
	else{
		struct PaginaPrimaria *page = carregarPagina(pagBusca);
		imprimirRegistro(page->chaves[pos].RRN);
		free(page);
	}
}


/*****************************************************************************************************************************/

/*Função responsável por imprimir toda a  estrutura da árvore B, a função é chamada recursivamente para todas as páginas filhas.
 * @param - long int RRN - RRN da página a ser imprimida */
void imprimirArvore(long int RRN){
	struct PaginaPrimaria *pag = carregarPagina(RRN);
	printf("%ld|",pag->RRN);
	printf("%d|",pag->NE);
	int i,j;
	for(i = 0; i < MAX; i++){
		printf("%ld|",pag->filhos[i]);
		for(j = 0; j < 7; j++)
			printf("%c",pag->chaves[i].codigo[j]);
		printf(",%ld|",pag->chaves[i].RRN);
	}
	printf("%ld|\n",pag->filhos[MAX]);
	for(i=0; i < MAX+1; i++)
		if(pag->filhos[i] > 0)
			imprimirArvore(pag->filhos[i]);
	free(pag);
}
			
/****************************************************************************************************************************/

/*Função que realiza busca binária no vetor de chaves da página.
 *@param - chaves[MAX][7] - vetor de código de disciplinas com tamanho constante MAX, cada chave contém 6 caracteres mais o /0.
 *@param - chaveBusca - String com a chave que será buscada no vetor.
 *@param - pos - Ponteiro que retornará a posição onde se encontra a chave que se deseja buscar, ou onde ela deveria estar caso
 *a chave não seja encontrada.
 *return 1 - caso a chave buscada seja encontrada.
 *return 0 - caso a chave buscada não seja encontrada. */ 
int buscaBinaria(struct Chave chaves[MAX], char chaveBusca[7], int * pos){
	int a = 0;
	int l,j;
	while(chaves[a].codigo[0] != '#' && a < 7)
		a++;
	int i,ini = 0,fim = a-1, meio,achei = 0;
	int long pos2 = 0; /* variável q armazenará a posição dos filho caso a chave não seja encontrada */
	while(!achei && ini <= fim){
		meio = (ini+fim)/2;
		int i = 0;
		while(chaves[meio].codigo[i] == chaveBusca[i] && i < 7){
			i++;
		}
		if(i == 7)
			achei = 1;
		else if(chaves[meio].codigo[i] > chaveBusca[i]){
			fim = meio-1;
			if(meio > 0 && ini < fim)
				pos2 = meio-1;
		}
		else{
			ini = meio+1;
			pos2 = meio+1;
		}
	}
	if(achei){
		*pos = meio;
		return 1;
	}
	else{
		*pos = pos2;
		return 0;
	}
	
}

/*****************************************************************************************************************************/
/*Função que realiza a busca binária no vetor de chaves com MAX + 1 elementos e retorna com referência a posição em que a chaveBusca
 * deve sem inserida. (Realiza a mesma função da rotina buscaBinaria ja criada, mas com um parametro chaves com um número maior de elementos.
 * @param - struct Chave chaves[MAX+1] - vetor de chaves utilizado no split.
 * @param - char chaveBusca[7] - chave com 7 caracteres do elemento a ser inserido no vetor.
 * @param - int *pos - ponteiro que retornará a posição em que a chaveBusca deve ser inserida no vetor de chaves.*/
void buscaBinariaSplit(struct Chave chaves[MAX+1], char chaveBusca[7], int * pos){
	int a = 0;
	int l,j;
	int i,ini = 0,fim = MAX-1, meio,posA;
	int long pos2 = 0; /* variável q armazenará a posição dos filho caso a chave não seja encontrada */
	do{
		meio = (ini+fim)/2;
		int i = 0;
		while(chaves[meio].codigo[i] == chaveBusca[i] && i < 7){
			i++;
		}
		if(chaves[meio].codigo[i] > chaveBusca[i]){
			fim = meio-1;
			if(meio > 0 && ini < fim)
				posA = meio-1;
			else if(meio == 0)
				posA = meio;
		}
		else{
			posA = meio+1;
			ini = meio+1;
		}
		*pos = posA;
	}while(ini <= fim);
}

/*****************************************************************************************************************************/
/*Função que inicializa uma nova página, preenchendo os espaços das chaves com o caracter #, os ponteiros para filhos com -1 e 
 * o contador de chaves com 0. 
 *@param - pag - ponteiro para a nova página que temporáriamente esta na memória principal, e que será salva na memória secun-
 *dária. */
void inicializaPagina(struct PaginaSecundaria *pag){
	pag->NE = 0;
	int i,j;
	for(i = 0; i < MAX; i++){
		for(j=0; j < 7; j++)
			pag->chaves[i].codigo[j] = '#';
		pag->chaves[i].RRN = -1;
	}
	for(i=0; i < MAX+1; i++)
		pag->filhos[i] = -1;
}

/*****************************************************************************************************************************/

/*Função responsável pela remoção de elementos na árvore B do árquivo index e no arquivo de dados. A partir da chave o elemento é buscado 
 * na árvore de índice, através do RRN o registro é localizado no arquivo data e é indicado com o caracter '#', representando um registro
 * inútilizado, entam a chave é excluida também da árvore B, realizando os devidos procedimentos para manter as propriedades da estrutura.
 * @param - char chave[7] - código com seis caracteres referentes a chave do registro a ser removido, mais o caracter '\0'.*/
void removerRegistro(char chave[7]){
	FILE *arqIndice = fopen("index.dat","r+b");
	long int RRNcorrente;
	fread(&RRNcorrente,sizeof(int long),1,arqIndice);
	long int pagBusca,RRNPai = -1;
	int pos,verificador = 0;
	if(!buscaIndice(RRNcorrente,chave,&pagBusca,&pos,&RRNPai))
		printf("Erro");
	else{
		struct PaginaPrimaria *page = carregarPagina(pagBusca);
		FILE *arqDados = fopen("data.dat","r+b");
		fseek(arqDados,page->chaves[pos].RRN,0);
		char s = '#';
		fwrite(&s,sizeof(char),1,arqDados);
		fclose(arqDados);
		struct PaginaPrimaria *pagIrma, *pagPai;
		int posPagAtual,i,RRNirma,verIrma;
		if(RRNPai != -1){
			pagPai = carregarPagina(RRNPai);		
			for(i=0; i < MAX+1;i++){
				if(pagPai->filhos[i] == page->RRN){
					posPagAtual  = i;
					}
			}
			if(posPagAtual == 0){
				pagIrma = carregarPagina(pagPai->filhos[1]);
				verIrma = 1;
			}
			else if(posPagAtual == MAX){
				pagIrma = carregarPagina(pagPai->filhos[MAX-1]);
				verIrma = -1;
			}
			else {
				pagIrma = carregarPagina(pagPai->filhos[posPagAtual-1]);
				verIrma = -1;
				if(pagIrma->NE <= 2 && pagPai->filhos[posPagAtual+1] != -1){
					pagIrma = carregarPagina(pagPai->filhos[posPagAtual+1]);
					verIrma = 1;
				}
			}
		}
		if(page->NE > (MAX/2) && page->filhos[pos] == -1){ /*caso um, remoção de página folha e com mais elementos que o mínimo */
			printf("a%d %d\n",page->NE,(MAX/2));
			int i,j;
			verificador = 1;
			for( i = pos; i < MAX; i++){
				for(j = 0; j < 7; j++)
					page->chaves[i].codigo[j] = page->chaves[i+1].codigo[j];
				page->chaves[i].RRN = page->chaves[i+1].RRN;
			}
			for(j = 0; j < 7; j++)
				page->chaves[MAX-1].codigo[j] = '#';
			page->chaves[MAX-1].codigo[6] = '\0';
			page->chaves[MAX-1].RRN = -1;
			for(i = pos; i < MAX+1; i++)
				page->filhos[i] = page->filhos[i+1];
			page->filhos[MAX] = -1;
			page->NE--;
			salvarPagina(page);
		}
		else if(page->filhos[pos+1] != -1){/* caso dois, remoção de de página que não é folha */
			long int aux = page->filhos[pos+1];
			struct PaginaPrimaria *pageAux;
			printf("b");
			while(aux != -1){ /*procedimento que percorre as páginas até encontrar a folha, filho maior a direita */
				pageAux = carregarPagina(aux);
				aux = pageAux->filhos[0];
			}
			if(pageAux->NE > (MAX/2)){
				int i;
				verificador = 2;
				for(i = 0; i < 7; i++)
					page->chaves[pos].codigo[i] = pageAux->chaves[0].codigo[i];
				page->chaves[pos].RRN = pageAux->chaves[pos].RRN;
				removerRegistro(pageAux->chaves[0].codigo);
			}
			else{
				aux = page->filhos[pos];
				int pos2;
				while(aux != -1){ /*procedimento que percorre as páginas até encontrar a folha, filho menor a esquerda */
					pageAux = carregarPagina(aux);
					pos2 = 0;
					while(pageAux->chaves[pos2].codigo[0]!= '#' && pos2 < MAX){
						pos2++;
					}
					pos2--;
					aux = pageAux->filhos[pos2];
				}
				if(pageAux->NE > (MAX/2)){
					int i;
					verificador = 2;
					for(i = 0; i < 7; i++)
						page->chaves[pos].codigo[i] = pageAux->chaves[pos2].codigo[i];
					page->chaves[pos].RRN = pageAux->chaves[pos2].RRN;
					removerRegistro(pageAux->chaves[pos2].codigo);
				}
			}
			salvarPagina(page);
			free(page);
			free(pagIrma);
		}
		else if(page->NE <= (MAX/2) && pagIrma->NE > 2){ /* caso três, quando é necessário realizar uma distribuição entre as irmãs */
			verificador = 3;
			redistribuicao(page->RRN,pagPai->RRN,pagIrma->RRN,pos,verIrma);
		}
		if(verificador == 0){ /*Quarto caso, concatenação */
			concatenacao(pagBusca,RRNPai,chave,pos);
		}	
	}
}
/*****************************************************************************************************************************/

void concatenacao(long int pagBusca, long int RRNPai, char chave[7], int pos){
	struct PaginaPrimaria *page = carregarPagina(pagBusca);
	struct PaginaPrimaria *pagPai = carregarPagina(RRNPai);
	int i,j,a = 0,posPag = 0;
	for(i = 0; i < MAX+1; i++)
		if(pagPai->filhos[i] == pagBusca)
			posPag = i;
	struct PaginaPrimaria *pagIrma;
	if(!(pos == MAX)){
		pagIrma = carregarPagina(pagPai->filhos[posPag+1]);
	}
	else{
		pagIrma = carregarPagina(pagPai->filhos[posPag]);
		a = 1;
	}
	int c = 0;
	struct Chave chaves[MAX]; 
	long int filhos[MAX+1];
	if(!a){	
		for(i = 0; i < (MAX/2)-1; i++){ /*pegando as chaves da página */
			if(i!= pos){
				for(j = 0; j< 7; j++){
					chaves[c].codigo[j] = page->chaves[i].codigo[j];
				}
				chaves[c].RRN = page->chaves[i].RRN;
				c++;
			}
		}
		for(i = 0; i < (MAX/2)+1; i++){ /*redistribuindo os filhos em um vetor almentado */
			filhos[i] = page->filhos[i];
		}
		if(posPag > 0){ /*pegando a chave pai */
			for(i = 0; i < 7; i++)
				chaves[c].codigo[j] = pagPai->chaves[posPag-1].codigo[j];
			chaves[c].RRN = pagPai->chaves[posPag-1].RRN;
		}
		else{
			for(i = 0; i < 7; i++)
				chaves[c].codigo[j] = pagPai->chaves[posPag].codigo[j];
			chaves[c].RRN = pagPai->chaves[posPag].RRN;
		}
		c++;
		for(i = 0; i < MAX/2; i++){ /*pegando as chaves da página irmã */
				for(j = 0; j< 7; j++){
					chaves[c].codigo[j] = pagIrma->chaves[i].codigo[j];
				}
				chaves[c].RRN = pagIrma->chaves[i].RRN;
				c++;
			}
		for(i = (MAX/2)+1; i < MAX+1; i++){ /*redistribuindo os filhos em um vetor almentado */
			filhos[i] = pagIrma->filhos[i];
		}		
	}
	for( i =0; i < MAX; i++){
		printf("%ld %s %ld \n",chaves[i].RRN,chaves[i].codigo,filhos[i]);
	}
	printf("%ld\n",filhos[i]);
}

/*****************************************************************************************************************************/
/*Função responsável por realizar a redistribuição das chaves entre as duas páginas irmãs e a chave pai, quando necessário.
 * Todas as chaves das duas páginas irmãs e a chave pai são colacas em um vetor de chaves ordenadamente, se necessário é promovida
 * uma nova chave pai, depois as as chaves são destribuidas igualmente entre as páginas irmãs de maneira que nenhuma fica com número
 * de elementos abaixo da ordem.
 * @param - long int RRNpage - RRN da página em que foi removido o elemento e que está com o NE abaixo da ordem.
 * @param - long int RRNpai - RRN da página pai.
 * @param - long int RRNirma - RRN da página irmã.
 * @param - int pos - posição do elemento que foi removido na página (RRNpage).
 * @param - int verIrma - verificador utilizado para saber se a página irmã é a direita ou a esquerda, sendo -1 para irmãs à esquerda e
 * 1 para irmãs à direita.*/
void redistribuicao(long int RRNpage, long int RRNpai, long int RRNirma, int pos, int verIrma){
	struct PaginaPrimaria *page = carregarPagina(RRNpage);
	struct PaginaPrimaria *pagPai = carregarPagina(RRNpai);
	struct PaginaPrimaria *pagIrma = carregarPagina(RRNirma);
	int total = page->NE + pagIrma->NE;
	long int filhos[total];
	struct Chave chaves[total];
	int i,j,posPai = -1,l = 0,a= 0;
	for(i = 0; i < MAX+1; i++)
		if(pagPai->filhos[i] == page->RRN)
			posPai = i-1;
	if(posPai == -1)
		posPai = 0;
	else if(posPai == MAX)
		posPai--;
	if(verIrma == 1){/*caso seja uma página irmã à direita */
	
		for(i = 0; i < page->NE; i++){ /*Passando os elementos da página para o vetor */
			if(i != pos){
				for(j= 0; j < 7; j++)
					chaves[a].codigo[j] = page->chaves[i].codigo[j];
				chaves[a].RRN = page->chaves[i].RRN;
				a++;
			}
		}
		for( j = 0; j < 7; j++) /*Passando a chave pai para o vetor */
			chaves[a].codigo[j] = pagPai->chaves[posPai].codigo[j];
		chaves[a].RRN = pagPai->chaves[posPai].RRN;
		i++;
		a++;
		l = 0;
		for(; i <total+1; i++){ /*Passando os elementos da página irmã para o vetor */
			for( j = 0; j < 7; j++)
				chaves[a].codigo[j] = pagIrma->chaves[l].codigo[j];
			chaves[a].RRN = pagIrma->chaves[l].RRN;
			a++;
			l++;
		}
		l=0;
		a=0;
		for( i = 0; i < page->NE+1; i++) /* Passando os filhos das duas páginas irmãs para o vetor filhos */
			filhos[i] = page->filhos[i];
		l=0;
		for(; i<=  total; i++){
			filhos[i] = pagIrma->filhos[l];
			l++;
		}
		long int auxRRN;
		struct PaginaSecundaria *pagAux  = (struct PaginaSecundaria *) malloc(sizeof(struct PaginaSecundaria));
		inicializaPagina(pagAux);
		auxRRN = page->RRN;
		page = secundariaToPrimaria(pagAux);
		page->RRN = auxRRN;
		pagAux  = (struct PaginaSecundaria *) malloc(sizeof(struct PaginaSecundaria));
		inicializaPagina(pagAux);
		auxRRN = pagIrma->RRN;
		pagIrma = secundariaToPrimaria(pagAux);
		pagIrma->RRN = auxRRN;
		/*Redistribuido os elementos do vetor chaves entre as páginas pai e as duas irmãs. */
		for( i =0; i < 7; i++) 
			pagPai->chaves[posPai].codigo[i] = chaves[(total/2)].codigo[i];
		pagPai->chaves[posPai].RRN = chaves[(total/2)].RRN;
		
		for(i = 0; i < (total/2) ;i++){
			for(j = 0; j < 7; j++)
				page->chaves[i].codigo[j] = chaves[i].codigo[j];
			page->chaves[i].RRN = chaves[i].RRN;
		}
		for(i = 0; i < (total/2)+1; i++)
			page->filhos[i] = filhos[i];
		int a = 1;
		if(total%2 == 0 ){
			total--;	
			a =2;
		}
		for(i = 0 ; i< (total/2); i++){
			for(j = 0; j < 7; j++)
				pagIrma->chaves[i].codigo[j] = chaves[(total/2)+i+a].codigo[j];
			pagIrma->chaves[i].RRN = chaves[(total/2)+i+1].RRN;
		}
		for(i = 0; i< (total/2)+1;i++)
			pagIrma->filhos[i] = filhos[(total/2)+i];
		if(a==1){
			page->NE = (total/2);
			pagIrma->NE = (total/2);
		}
		else {
			page->NE = (total/2)+1;
			pagIrma->NE = (total/2);
		}
	}
	else if(verIrma == -1){ /* caso seja uma página irmã à esquerda */
		for(i = 0; i < pagIrma->NE; i++){/*Passando os elementos da página para o vetor */
			for(j= 0; j < 7; j++)
				chaves[a].codigo[j] = pagIrma->chaves[i].codigo[j];
			chaves[a].RRN = pagIrma->chaves[i].RRN;
			a++;
		}
		for( j = 0; j < 7; j++)/*Passando a chave pai para o vetor */
			chaves[a].codigo[j] = pagPai->chaves[posPai].codigo[j];
		chaves[a].RRN = pagPai->chaves[posPai].RRN;
		i++;
		a++;
		int b =0;
		l = 0;
		for(; i <total+1; i++){/*Passando os elementos da página irmã para o vetor */
			if(b != pos){
				for( j = 0; j < 7; j++)
					chaves[a].codigo[j] = page->chaves[l].codigo[j];
				chaves[a].RRN = page->chaves[l].RRN;
				a++;
			}
			b++;
			l++;
		}
		l=0;
		a=0;
		for( i = 0; i < pagIrma->NE+1; i++)	/* Passando os filhos das duas páginas irmãs para o vetor filhos */
			filhos[i] = pagIrma->filhos[i];
		for(; i<=  total; i++){
			filhos[i] = page->filhos[l];
			l++;
		}
		long int auxRRN;
		struct PaginaSecundaria *pagAux  = (struct PaginaSecundaria *) malloc(sizeof(struct PaginaSecundaria));
		inicializaPagina(pagAux);
		auxRRN = page->RRN;
		page = secundariaToPrimaria(pagAux);
		page->RRN = auxRRN;
		pagAux  = (struct PaginaSecundaria *) malloc(sizeof(struct PaginaSecundaria));
		inicializaPagina(pagAux);
		auxRRN = pagIrma->RRN;
		pagIrma = secundariaToPrimaria(pagAux);
		pagIrma->RRN = auxRRN;
		/*Redistribuido os elementos do vetor chaves entre as páginas pai e as duas irmãs. */
		for( i =0; i < 7; i++)
			pagPai->chaves[posPai].codigo[i] = chaves[(total/2)].codigo[i];
		pagPai->chaves[posPai].RRN = chaves[(total/2)].RRN;
		int ativado = 0;
		for(i = 0; i < (total/2) ;i++){
			for(j = 0; j < 7; j++)
				pagIrma->chaves[i].codigo[j] = chaves[i].codigo[j];
			pagIrma->chaves[i].RRN = chaves[i].RRN;
		}
		for(i = 0; i < (total/2)+1; i++)
			pagIrma->filhos[i] = filhos[i];
		int a = 1;
		if(total%2 == 0){
			total--;
			a++;
		}
		for(i = 0 ; i< (total/2); i++){
			for(j = 0; j < 7; j++)
				page->chaves[i].codigo[j] = chaves[(total/2)+i+a].codigo[j];
			page->chaves[i].RRN = chaves[(total/2)+i+a].RRN;
		}
		for(i = 0; i< (total/2)+1;i++)
			page->filhos[i] = filhos[(total/2)+i];
		if(a == 2){
			page->NE = (total/2);
			pagIrma->NE = (total/2)+1;
		}
		else{
			page->NE = (total/2);
			pagIrma->NE = (total/2);
		}
	}
	salvarPagina(page);
	salvarPagina(pagIrma);
	salvarPagina(pagPai);
}

/*****************************************************************************************************************************/

/*Função retorna o tamanho do arquivo em bytes
 *@param - arquivo - ponteiro para o arquivo a ser analizado
 *return - O tamanho do arquivo */
long int tamArquivo(FILE * arquivo){			/*retorna o tamanho do arquivo */
	rewind(arquivo);
	long int cont = 0;
	int aux;
	while(fread(&aux,1,1,arquivo) != 0){
		cont++;
	}
	return cont;
}
