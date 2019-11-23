#include "block.h"

void clearTela() {
    getchar();
    getchar();
    system(CLEAR);
}

int existeLadae(LADAE **inicio){
    if(inicio == NULL)
        return 0;
    return 1;
}

int existeBloco(BLOCO **chain){
    if(chain == NULL)
        return 0;
    return 1;
}

LADAE** criaInicializaLadae(){
    LADAE **novo;

    novo = (LADAE **) malloc(sizeof(LADAE *));
    *novo = NULL;

    return novo;
}

BLOCO** InicializaBloco() {
    BLOCO **novo;

    novo = (BLOCO **) malloc(sizeof(BLOCO *));
    *novo = NULL;

    return novo;
}

int ehVaziaLadae(LADAE **inicio){
    if (existeLadae(inicio) == 0)
        return -1;
    if(*inicio == NULL)
        return 1;
    return 0;
}

int ehVaziaBloco(BLOCO **chain){
    if (existeBloco(chain) == 0)
        return -1;
    if(*chain == NULL)
        return 1;
    return 0;
}

/* Verifica se o buffer esta cheio(1) ou nao(0) */
int ehCheiaLadae(LADAE **inicio) {
    LADAE *aux;
    aux = *inicio;

    if (existeLadae(inicio) == 0)
        return -1;

    if (ehVaziaLadae(inicio) == 1)
        return 0;

    int qt = 0;
	while (aux != NULL) {
		aux = aux->prox;
		qt++;
	}

    if(qt == MAX_BUFF) {
        return 1;
    }

    return 0;
}

/* Esvazia o buffer */
void zerarLadae(LADAE **inicio) {
    LADAE *aux, *prox;
    aux = *inicio;

    while(aux != NULL) {
        prox = aux->prox;
        free(aux);
        aux = prox;
    }

    *inicio = NULL;
}

int insereBuffer(LADAE **inicio, Data novo) {
    LADAE *novono, *aux;

    if(existeLadae(inicio) == 0)
        return -1;

    /* Se o buffer estiver cheio nao inserimos */
    if(ehCheiaLadae(inicio) == 1)
        return 0;

    novono = (LADAE*)malloc(sizeof(LADAE));

    novono->prox = NULL;
    novono->elem = novo;

    if (ehVaziaLadae(inicio) == 1) {
        *inicio = novono;
        return 1;
    }

    aux = *inicio;

    while (aux->prox != NULL) {
        aux = aux->prox;
    }
    aux->prox = novono;

    return 1;
}

/* Hashing inicial dos registros para as folhas da Merkle */
int hashData(Data *elem, unsigned char *hash) {
    SHA256_CTX ctx;
    if(SHA256_Init(&ctx) == 0)
        return -1;

    SHA256_Update(&ctx, &elem, sizeof(Data));

    if(SHA256_Final(hash, &ctx) == 0)
        return 1;

    return 1;
}

/* Hashing dos hashes dos nos filhos */
int hashPai(Merkle *no[], int i) {

    SHA256_CTX ctx;
    if(SHA256_Init(&ctx) == 0)
        return -1;

    no[i]->esq = no[(2*i)+1];
    no[(2*i)+1]->pai = no[i];

    no[i]->dir = no[(2*i)+2];
    no[(2*i)+2]->pai = no[i];

    SHA256_Update(&ctx, &no[(2*i)+1]->hash, SHA256_DIGEST_LENGTH);
    SHA256_Update(&ctx, &no[(2*i)+2]->hash, SHA256_DIGEST_LENGTH);

    if(SHA256_Final(no[i]->hash, &ctx) == 0)
        return -1;

    return 1;
}

/* Criacao da merkle completa, utilizando o buffer tipo LADAE como parametro */
Merkle** criaMerkle(LADAE **inicio) {
    Merkle **arvore;
    /* Formatacao vetorizada para arvore para facilitar a manipulacao */
    Merkle *no[(MAX_BUFF*2)-1];
    LADAE *aux;
    int i;

    arvore = (Merkle **) malloc(sizeof(Merkle *));
    aux = *inicio;

    for(i = 0; i < (MAX_BUFF*2)-1; i++) {
        no[i] = (Merkle*) malloc(sizeof(Merkle));
    }

    i = MAX_BUFF - 1; /* no folha mais a esquerda */
    /* while enquanto nao chegarmos ao final do buffer */
    while(aux != NULL) {

        no[i]->registro = aux->elem;
        no[i]->dir = NULL;
        no[i]->esq = NULL;

        /* Hashing dos nos folhas com caso de falha */
        if(hashData(&no[i]->registro, no[i]->hash) == -1)
            printf("\nERRO: Falha no hashing dos dados\n");

        aux = aux->prox;
        i++;
    }

    /* hashing do restante dos nos */
    for(i = MAX_BUFF - 2; i >= 0; i--) {
        hashPai(no, i);
    }

    /* pointeiro de ponteiro para o inico da arvore Merkle */
    *arvore = no[0];
    /* Nulificacao do ponteiro de pai do no raiz */
    no[0]->pai = NULL;

    return arvore;
}

/* Criacao de blocos da nossa blockchain, buffer como parametro para utiliza-lo
na criacao da arvore Merkle com afuncao criaMerkle() */
BLOCO* criaBloco(LADAE **inicio) {
    BLOCO *novo;

    novo = (BLOCO*) malloc(sizeof(BLOCO));

    novo->prox = NULL;
    novo->ant = NULL;
    novo->timestamp = (int)time(NULL);
    /* criacao da merkle */
    novo->raiz = criaMerkle(inicio);

    /* igualamos a hash da raiz da Merkle com a hash do bloco */
    memcpy(novo->hash, (*novo->raiz)->hash, SHA256_DIGEST_LENGTH);

    /* esvaziamos o buffer ja utilizado */
    zerarLadae(inicio);

    return novo;
}

void caminharLiberarMerkle(Merkle *nodo) {
    /* condicao de parada: nos folha */
    if(nodo->dir == NULL && nodo->esq == NULL) {
        free(nodo);
        nodo = NULL;
        return ;
    }

    caminharLiberarMerkle(nodo->esq);
    caminharLiberarMerkle(nodo->dir);
    free(nodo);
    nodo = NULL;
}

void liberaMerkle(Merkle **arvore) {
    Merkle *aux;
    aux = *arvore;

    caminharLiberarMerkle(aux);
    free(arvore);
    arvore = NULL;
}

/* Impressao do buffer (impressao de ladae normal) */
void imprimeBuff(LADAE **inicio) {
    if(ehVaziaLadae(inicio) == 1) {
        printf("Buffer vazio.\n\nperte ENTER para voltar ao MENU\n");
        clearTela();

        return ;
    }

    Data ele;
    LADAE *aux;
    aux = *inicio;

    printf("Insira %d elementos no buffer para criar um bloco.\n", MAX_BUFF);
    printf("\tElementos no buffer:\n");
    while(aux != NULL) {
        ele = aux->elem;

        printf("\tAluno: %s - %s\n", ele.matricula, ele.nome);
        printf("\t\tIRA: %.4f\n\n", ele.info);

        aux = aux->prox;
    }
    clearTela();
}

void imprimeMerkle(Merkle *nodo) {
    /* condicao de parada: nos folha */
    if(nodo->esq == NULL && nodo->dir == NULL){
        printf("\tAluno: %s - %s", nodo->registro.matricula, nodo->registro.nome);
        printf("\t\tIRA: %.4f\n\n", nodo->registro.info);

        return ;
    }
    imprimeMerkle(nodo->esq);
    imprimeMerkle(nodo->dir);
}

/* Impressao apenas do bloco Genesis */
void imprimeRegGen(BLOCO **chain){
    if(existeBloco(chain) == 0) {
        printf("Bloco nao criado!\n\nAperte ENTER para voltar ao MENU\n");
        clearTela();

        return ;
    }
    if(ehVaziaBloco(chain) == 1) {
        printf("Nenhum elemento enviado ao bloco!\nAperte ENTER para voltar ao MENU\n");
        clearTela();

        return ;
    }

    Merkle *aux;
    BLOCO *auxb;

    auxb = *chain;
    aux = *(auxb->raiz);

    printf("\n\t\tBloco Genesis\n\n");

    imprimeMerkle(aux);

    printf("Hash: ");
    int i;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", auxb->hash[i]);
    }
    printf("\n\n\n");
    printf("\nAperte ENTER para voltar ao MENU\n");
    clearTela();
}

/* Impressao da blockchain inteira (nao imprime buffer) */
void imprimeChain(BLOCO **chain){
    if(existeBloco(chain) == 0) {
        printf("Bloco nao criado!\n\nAperte ENTER para voltar ao MENU\n");
        clearTela();

        return ;
    }

    int bloco = 1, i = 0;
    Merkle *aux;
    BLOCO *auxb;

    auxb = *chain;


    if(ehVaziaBloco(chain) == 0){

        /* while para percorrer a blockchain */
        while(auxb != NULL) {
            aux = *(auxb->raiz);

            /* Impressao diferenciada para bloco genesis (sem hash pai) */
            if(bloco == 1){
                printf("\n\t\tBloco Genesis\n\n");

                imprimeMerkle(aux);
                for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                    printf("%02x", auxb->hash[i]);
                }
                printf("\n");
            }
            else{
                printf("\n\t\tBloco %d\n\n", bloco);

                imprimeMerkle(aux);

                printf("Hash do bloco: ");
                for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                    printf("%02x", auxb->hash[i]);
                }

                printf("\nHash do pai: ");
                for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                    printf("%02x", auxb->ant->hash[i]);
                }
                printf("\n");

            }
            /* contagem dos blocos e passo da blockchain */
            bloco++;
            auxb = auxb->prox;
        }
        printf("\n\n\n");
        printf("\nAperte ENTER para voltar ao MENU\n");
        clearTela();
    }
    else {
        printf("Nenhum elemento enviado ao bloco!\nAperte ENTER para voltar ao MENU\n");
        clearTela();
    }
}

/* busca elementos nas folhas das arvores Merkle de um bloco */
int buscaElementoMerkle(Merkle* nodo, Data *ele) {
    /* condicao de parada: nos folha */
    if(nodo->esq == NULL && nodo->dir == NULL) {
        if(strcmp(ele->matricula, (nodo->registro).matricula) == 0) {
            memcpy(ele, &nodo->registro, sizeof(Data));
            return 1;
        }
        else
            return 0;
    }

    int achou = 0;

    achou += buscaElementoMerkle(nodo->esq, ele);
    achou += buscaElementoMerkle(nodo->dir, ele);

    return achou;
}

/* percorre a blockchain para procurar um elemento */
int buscaElementoBloco(BLOCO **chain, Data *ele){
    if(existeBloco(chain) == 0) {
        printf("Bloco nao criado!\n\nAperte ENTER para voltar ao MENU\n");
        clearTela();
        return -1;
    }
    if(ehVaziaBloco(chain) == 1){
        printf("Nenhum elemento enviado ao bloco!\nAperte ENTER para voltar ao MENU\n");
        clearTela();
        return -1;
    }

    int achou;
	Merkle *aux;
    BLOCO *auxb;

    auxb = *chain;

    printf("Digite a matricula de um aluno para a busca: ");
    scanf("%s", ele->matricula);

    achou = 0;
    while(auxb != NULL && achou != 1) {
        aux = *(auxb->raiz);

        if(buscaElementoMerkle(aux, ele) == 1) {
            printf("Elemento existente na BlockChain!\n");
            printf("\tAluno: %s - %s\n", ele->matricula, ele->nome);
            printf("\t\tIRA: %.4f\n\n", ele->info);

            achou = 1;
        }

        auxb = auxb->prox;
    }

    if (achou == 0)
        printf("Elemento nao encontrado na cadeia de blocos!\n");

    printf("\nAperte ENTER para voltar ao MENU\n");
    clearTela();

    return 0;
}

int buscaElementoBuff(LADAE **inicio, BLOCO **chain, Data ele){
    LADAE *auxL;
    BLOCO *auxb;
	Merkle *auxM;

    auxL = *inicio;

    if (ehVaziaLadae(inicio) == 1) {
        return 0;
    }
    else { /* percorre no buffer */
        while (auxL != NULL) {
            if (strcmp(ele.matricula,(auxL->elem).matricula) == 0) {
                printf("Matricula ja inserida, por favor insira uma diferente!\n");
                return 1;
            }
            auxL = auxL->prox;
        }
        return 0;
    }

    if(ehVaziaBloco(chain) == 1)
        return 0;
    else { /* percorre a blockchain */
        auxb = *chain;
        while(auxb != NULL) {
            auxM = *(auxb->raiz);

            if(buscaElementoMerkle(auxM, &ele) == 1){
                printf("Matricula ja inserida, por favor insira uma diferente!\n");
                return 1;
            }
            auxb = auxb->prox;
        }
    }

    return 0;
}

void liberaLadae(LADAE **inicio){
    LADAE *aux,*liberar;
    aux = *inicio;

    while(aux!=NULL)
    {
        liberar = aux;
        aux=aux->prox;
        free(liberar);
    }
    free(inicio);
    inicio = NULL;
}

void liberaBloco(BLOCO **chain) {
    BLOCO *auxb, *liberar;
    auxb = *chain;

    while(auxb != NULL) {
        liberar = auxb;
        auxb = auxb->prox;
        liberaMerkle(liberar->raiz);
        free(liberar);
    }

    free(chain);
    chain = NULL;
}

void opcaoUm(BLOCO **chain){
    if(existeBloco(chain) == 0){
        chain = InicializaBloco();

        printf("Bloco Genesis inicializado com sucesso.\n");
    }
    else {
        printf("ERRO: Bloco Genesis ja existente.\n");
    }

    printf("\nAperte ENTER para continuar\n");
    getchar();
    getchar();
}

void insereRegistro(BLOCO **chain, LADAE **inicio, Data aux){
    if(existeBloco(chain) == 0) {
        printf("Bloco nao criado!\n\nAperte ENTER para voltar ao MENU\n");
        clearTela();

        return ;
    }

    int achou = 0;

    do {
        do{
            printf("Digite a matricula do Aluno: ");
            scanf("%s", aux.matricula);

            achou = buscaElementoBuff(inicio, chain, aux);
        }while(achou == 1);
        achou = 0;

        printf("Digite o nome do Aluno: ");
        scanf("%s", aux.nome);
        printf("Digite o IRA do Aluno: ");
        scanf("%f", &aux.info);

        if (aux.info > 5 || aux.info < 0)
        printf("IRA invalido, digite novamente o nome do aluno e um IRA valido!\n");

    } while (aux.info > 5 || aux.info < 0);

    insereBuffer(inicio, aux);

    if(ehCheiaLadae(inicio) == 1){
        BLOCO *atual, *ant;
        atual = *chain;
        ant = NULL;

        if(ehVaziaBloco(chain) == 1) {
            atual = criaBloco(inicio);
            *chain = atual;
            return ;
        }

        while(atual != NULL) {
            ant = atual;
            atual = atual->prox;
        }

        atual = criaBloco(inicio);

        atual->ant = ant;
        ant->prox = atual;


        printf("Registro inserido com sucesso. Novo bloco criado.\n");
        printf("\tAluno(a): %s - %s\n\t\tIRA: %.4f\n\n", aux.matricula, aux.nome, aux.info);
        printf("Aperte ENTER para voltar ao MENU\n\n");
    }
    else{
        printf("Registro inserido com sucesso.\n");
        printf("\tAluno(a): %s - %s\n\t\tIRA: %.4f\n\n", aux.matricula, aux.nome, aux.info);
        printf("Aperte ENTER para voltar ao MENU\n\n");
    }
    clearTela();

    printf("\n\n");
}

int menuInterativo(int op){

    printf("\n\t\tMenu interativo\n\n");
    printf("(1) Criar e Inicializar Bloco Genesis\n");
    printf("(2) Inserir Registro de um aluno\n");
    printf("(3) Buscar um aluno na Blockchain\n");
    printf("(4) Imprimir Bloco Genesis\n");
    printf("(5) Imprimir Todos Blocos\n");
    printf("(6) Imprimir Buffer\n");
    printf("(0) SAIR\n\n\n");
    printf("Instrucoes:\n");
    printf("O seguinte BlockChain armazena os dados de um aluno : sua matricula\n");
    printf("(sem barra e sem pontos), seu nome(com ate 20 caracteres sem espaco\n");
    printf("e sem acento) e seu IRA, Indice de Rendimento Academico, ( menor ou\n");
    printf("igual a 5 ). Quando o buffer conter 16 registros, ele sera inserido\n");
    printf("no bloco e o hash sera criado de modo a ser unico, seguro e imutavel.\n\n");
    printf("Para comecar, por favor, aperte opcao (1) para criar o Bloco Genesis!\n\n");

    printf("Digite sua opcao: ");
    scanf("%d",&op);

    return op;
}
