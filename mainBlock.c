#include "block.h"
#define SUCESSO 1
#define FALHA 0

int main(){

    BLOCO **chain;
    chain = NULL;

    LADAE **lista;
    lista = NULL;
    lista = criaInicializaLadae(lista);
    //LADAE *node;

	Data aux;
    int op=-1;

    do{
        system(CLEAR);

        op = menuInterativo(op);

        if(op == 1){
			system(CLEAR);

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

        else if(op == 2){
            system(CLEAR);

			insereRegistro(chain, lista, aux);
        }

        else if(op == 3){
			system(CLEAR);

            buscaElementoBloco(chain, &aux);
        }

		else if(op == 4){
            system(CLEAR);

            imprimeRegGen(chain);
        }

		else if(op == 5){
            system(CLEAR);

            imprimeChain(chain);
        }
		else if(op == 6){
            system(CLEAR);

            imprimeBuff(lista);
        }

    }while(op != 0);

    if(existeBloco(chain) == 1)
        liberaBloco(chain);
    if(existeLadae(lista) == 1)
        liberaLadae(lista);

    printf("Encerrando as operacoes...\n");

    return 0;
}
