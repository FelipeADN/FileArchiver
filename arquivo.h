#ifndef ARQUIVO_H
#define ARQUIVO_H
#include "gerais.h"

//copia para o arquivo os dados dos membros
void gravarMembros(FILE *arquivo, infoMembro *infosImp, int argc, int quantidade, int *repeticao);

//desloca todos os dados de um ponto(inicio) ate o final do arquivo uma diferenca em relacao ao inicio
void deslocarMembro(FILE *arquivo, long inicio, int diferenca);

#endif