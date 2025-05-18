#ifndef LISTACONTEUDOS_H
#define LISTACONTEUDOS_H
#include "gerais.h"

//le do arquivo as informacoes dos membros para infosImp[]
int lerListaConteudo(FILE *arquivo, infoMembro *infosImp, long offsetLista); 

//popula nos infosImp[] informacoes dos membros novos
int membrosNovos(infoMembro *infosImp, struct stat *info, int argc, char *argv[], int quantidade, int *repeticao, int ordemAnterior, int offsetAnterior, int tamanhoAnterior, int membrosTam);

//escreve no arquivo informacoes dos membros
void escreveListaConteudo(FILE *arquivo, infoMembro *infosImp, long local, int argc, int quantidade, int *repeticao);

//guarda offset da lista de conteudos no inicio do arquivo
void guardaLocal(FILE *arquivo, long local);

//move membros da lista de conteudos de posicao
void moverNaLista(FILE *arquivo, infoMembro *infosImp, long offsetLista, int idMembro, int idTarget, int quantidade);

//escreve na lista de conteudo apenas os membros que nao foram removidos
void removerDaLista(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, int quantidade, int *repeticao);

//insere membros na lista de conteudos do arquivo (so atualiza se for mais recente)
void atualizaListaConteudo(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, char *argv[], int quantidade, int *repeticao);

//retorna local da lista de conteudo, lendo offset no inicio do arquivo
long localListaConteudo(FILE *arquivo);

//imprime dados dos membros
void imprimirLista(infoMembro *infosImp, int quantidade);

//converte UID para nome do usuario
char* converteUID(long UID);

//converte GID para nome do grupo
char* converteGID(long GID);

//converte numero de permissoes para string no formato (rwxrwxrwx)
char* convertePermissoes(unsigned short permNum);

#endif