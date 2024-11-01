#ifndef CONTROLE_H
#define CONTROLE_H
#include "gerais.h"

//remove dados do membro do arquivo
int removerMembro(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, char *argv[], int quantidade, int *repeticao);

//move dados do membro para depois do target
void moveMembro(FILE *arquivo, infoMembro *infosImp, int idMembro, int idTarget, int quantidade);

//defina ids do membro e do target
void idsMembroTarget(infoMembro *infosImp, char *membro, char *target, int *idMembro, int *idTarget, int quantidade);

//insere membros na lista de conteudos do arquivo
void insereListaConteudo(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, char *argv[], int quantidade, int *repeticao);

//extrai membros do arquivo
void extrairMembros(FILE *arquivo, infoMembro *infosImp, int argc, char *argv[], int quantidade, int *repeticao);

#endif