#ifndef GERAIS_H
#define GERAIS_H

//struct com informacoes sobre os membros para lista de conteudos
struct informacoes{
    char nome[255];
    long UID;
    int permissao;
    int tamanho;
    time_t modificacao;
    int ordem;
    long localOffset;
}typedef infoMembro;

//printa ajuda
void ajuda();

//retorna tamanho do arquivo
long tamanhoArq(FILE *arquivo);

//abre arquivo
FILE *abrirArquivo(char *nome);

//abre membro leitura
FILE *abrirMembroR(char *nome);

//abre membro escrita
FILE *abrirMembroW(char *nome);

//retorna quantidade de membros, contando membros do arquivo, e futuros membros passados pelo argv
int quantidadeMembros(FILE *arquivo, int argc, long offsetLista);

#endif