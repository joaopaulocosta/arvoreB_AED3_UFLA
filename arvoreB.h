#include <stdlib.h>
#include <stdio.h>
#define MAX 4
#define OR 2

struct Chave{
	char codigo[7];
	long int RRN;
};

/*Estrutura para armazenamento da página em memória secundária */
struct PaginaSecundaria{
	short NE;
	struct Chave chaves[MAX];
	long int filhos[MAX + 1];
};
/*Estrutura para armazenamento da página em memória principal */
struct PaginaPrimaria{
	long int RRN;
	short int NE;
	struct Chave chaves[MAX];
	long int filhos[MAX + 1];
};

struct PaginaPrimaria *carregarPagina(long int RRN);
int salvarPagina(struct PaginaPrimaria * pageP);
void carregarArquivos();
struct PaginaSecundaria * primariaToSecundaria(struct PaginaPrimaria *pageP);
struct PaginaPrimaria * secundariaToPrimaria(struct PaginaSecundaria *pageS);
int verificaEntrada(char entrada[1024]);
void inserirDados();
int inserirIndice(long int RRNcorrente,struct Chave chave, struct Chave * chavePromovida, long int * filhoPromovido);
long int tamArquivo(FILE * arquivo);
void inserirChave(struct Chave chave,long int filho, long int RRN);
int buscaBinaria(struct Chave chaves[MAX], char chaveBusca[7],int * pos);
void inicializaPagina(struct PaginaSecundaria *pag);
void split( struct Chave chave, long int filhoP, struct PaginaPrimaria* page, struct Chave * chavePromovida, long int * filhoPromovido, struct PaginaPrimaria * newPage);
int buscaIndice(long int RRN, char chave[7], long int * paginaBusca, int * posBusca,long int *pagPai );
void imprimirArvore(long int RRN);
void buscaBinariaSplit(struct Chave chaves[MAX+1], char chaveBusca[7], int * pos);
void imprimirRegistro(long int RRN);
char * lerRegistro();
void removerRegistro();
void busca();
void redistribuicao(long int RRNpage, long int RRNpai, long int RRNirma, int pos, int verIrma);
void concatenacao(long int pagBusca, long int RRNPai, char chave[7], int pos);
