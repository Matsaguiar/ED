#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<openssl/sha.h>

#define MAX_BUFF 2
#define SUCESSO 1
#define FALHA 0

#ifdef _WIN32
	# define CLEAR "cls"
#else
	# define CLEAR "clear"
#endif

struct dados{
	char matricula[15];
	char nome[21];
	float info; /* IRA */
};
typedef struct dados Data;

struct node{
    Data elem;
    struct node *prox;
};
typedef struct node LADAE;

struct merkle {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    struct merkle *pai;
    struct merkle *dir;
    struct merkle *esq;
	Data registro;
};
typedef struct merkle Merkle;

struct block{
	struct block *ant;
	struct block *prox;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	time_t timestamp;
	Merkle **raiz;
};
typedef struct block BLOCO;

int menuInterativo(int op);

int existeLadae(LADAE **inicio);
LADAE** criaInicializaLadae();
int ehVaziaLadae(LADAE **inicio);
int ehCheiaLadae(LADAE **inicio);
void zerarLadae(LADAE **inicio);

int existeBloco(BLOCO **chain);
BLOCO** InicializaBloco();
BLOCO* criaBloco();
int ehVaziaBloco(BLOCO **chain);
void opcaoUm(BLOCO **chain);

void insereRegistro(BLOCO **chain, LADAE **inicio, Data aux);
int insereBuffer(LADAE **inicio, Data novo);
int hashData(Data *elem, unsigned char *hash);
int hashPai(Merkle *no[], int i);

Merkle** criaMerkle(LADAE **inicio);

//void imprimeLADAEGen(BLOCO **chain);
void imprimeRegGen(BLOCO **chain);
void imprimeChain(BLOCO **chain);
void imprimeMerkle(Merkle *nodo);
void imprimeBuff(LADAE **inicio);

int buscaElementoMerkle(Merkle* nodo, Data *ele);
int buscaElementoBloco(BLOCO **chain, Data *ele);
int buscaElementoBuff(LADAE **inicio, BLOCO **chain, Data ele);

//int buscaElemento(BLOCO **chain, Data novo, Data vetor[], int n);
//int buscaElementoBLOCO(BLOCO **chain, Data novo);

void liberaLadae(LADAE** inicio);
void liberaBloco(BLOCO ** inicio);
void liberaMerkle(Merkle **arvore);
void caminharLiberarMerkle(Merkle *nodo);
