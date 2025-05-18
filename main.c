#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "arquivo.h"
#include "controle.h"
#include "listaConteudos.h"


int main(int argc, char *argv[]){
    char opt, *target;
    FILE* arquivo;

    if (strcmp(argv[1], "-m") == 0){
        arquivo = abrirArquivo(argv[3]);
    }
    else{
        arquivo = abrirArquivo(argv[2]);
    }
    long offsetLista = localListaConteudo(arquivo); //pega offset da lista de conteudo do comeco do arquivo
    int quantidade = quantidadeMembros(arquivo, argc, offsetLista);
    infoMembro infosImp[quantidade]; //array das structs com os dados importantes
    int repeticao[argc-3]; //array com id dos membros repetidos/desatualizados
    memset(repeticao, 0, (argc-3)*sizeof(int)); //inicia vetor zerado

    while ((opt = getopt(argc, argv, "iam:xrch")) != -1){ //detecta opcoes com (-)
        if (argv[2] != NULL && argv[2][0] == '-'){ //nao pode utilizar duas opcoes com (-)
            fprintf(stderr, "nao eh valida mais que uma opcao com (-)");
            exit(1);
        }
        switch (opt){ //um caso para cada opcao
        case 'i':
            //popula array de struct dos membros, e insere os novos, e atualiza repetidos
            insereListaConteudo(arquivo, infosImp, offsetLista, argc, argv, quantidade, repeticao);

            //grava novos membros no arquivo
            gravarMembros(arquivo, infosImp, argc, quantidade, repeticao);
            break;
        case 'a':
            //popula array da struct dos membros, insere os novos, e so atualiza se for mais recente
            atualizaListaConteudo(arquivo, infosImp, offsetLista, argc, argv, quantidade, repeticao);

            //grava novos membros no arquivo
            gravarMembros(arquivo, infosImp, argc, quantidade, repeticao);
            break;
        case 'm':
            //so recebe 1 membro
            if (argc != 5){
                fprintf(stderr, "opcao -m so pode receber 1 membro\n");
                fprintf(stderr, "usage -m (target) <archive> [membro]");
                exit(1);
            }
            else{
                target = optarg;
                //popula array de struct dos membros antigos
                lerListaConteudo(arquivo, infosImp, offsetLista);

                //define id do membro e do target
                int idMembro = 0, idTarget = 0;
                idsMembroTarget(infosImp, argv[4], target, &idMembro, &idTarget, quantidade);

                //movimenta o membro para depois do target
                moveMembro(arquivo, infosImp, idMembro, idTarget, quantidade);

                //movimenta membros da lista de conteudos
                moverNaLista(arquivo, infosImp, offsetLista, idMembro, idTarget, quantidade);
            }
            break;
        case 'x':
            //popula array de struct dos membros antigos
            lerListaConteudo(arquivo, infosImp, offsetLista);

            //extrai os membros do arquivo
            extrairMembros(arquivo, infosImp, argc, argv, quantidade, repeticao);
            break;
        case 'r':
            //popula array de struct dos membros antigos
            lerListaConteudo(arquivo, infosImp, offsetLista);
            //remove membros em si, retorna novo offset
            offsetLista = removerMembro(arquivo, infosImp, offsetLista, argc, argv, quantidade, repeticao);
            //remove membros da lista de conteudos
            removerDaLista(arquivo, infosImp, offsetLista, argc, quantidade, repeticao);
            break;
        case 'c':
            if (quantidade == 0){
                fprintf(stderr, "arquivo esta vazio");
                break;
            }
            //popula infosImp
            lerListaConteudo(arquivo, infosImp, offsetLista);
            //imprime infosImp
            imprimirLista(infosImp, quantidade);
            break;
        case 'h':
            ajuda();
            break;
        case '?': //caso nao tenha encontrado a opcao
            printf("usage <opcao> <archive> [membro1 membro2 ...]");
            exit(1);
        }
    }
    fclose(arquivo);
}

//restringir uso incorreto dos comandos (argc etc)
//lembrar dos fclose
//guardar caminho ao inves de nome