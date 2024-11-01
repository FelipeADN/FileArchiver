#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "gerais.h"

void ajuda(){
    printf("\n-i:          insere novo membro no archive"
    "\n-a:          insere apenas se membro tiver sido modificado"
    "\n-m (target): move membro para depois do membro (target)"
    "\n-x:          extrai os membros indicados"
    "\n-r:          remove os membros indicados"
    "\n-c:          lista conetudos"
    "\n-h:          ajuda\n");
}

long tamanhoArq(FILE *arquivo){
    fseek(arquivo, 0L, SEEK_END);
    return ftell(arquivo);
}

FILE *abrirArquivo(char *nome){
    FILE *arquivo = fopen(nome, "r+b"); //se existir
    if (!arquivo){
        arquivo = fopen(nome, "w+b"); //se nao cria
        if (!arquivo){
            fprintf(stderr, "falha na criacao do arquivo"); //se falhar
            exit(1);
        }
    }
    return arquivo;
}

FILE *abrirMembroR(char *nome){
    FILE *arquivo = fopen(nome, "r+b");
    if (!arquivo){
        fprintf(stderr, "arquivo nao existe");
        exit(1);
    }
    return arquivo;
}

FILE *abrirMembroW(char *nome){
    FILE *arquivo = fopen(nome, "wb");
    if (!arquivo){
        fprintf(stderr, "falha na criacao do arquivo");
        exit(1);
    }
    return arquivo;
}

int quantidadeMembros(FILE *arquivo, int argc, long offsetLista){
    int tamanho = tamanhoArq(arquivo);
    int quantidade = argc-3; //caso nao exista membros antigos
    if (tamanho > 0) //existem membros
        quantidade = ((tamanho - offsetLista)/sizeof(infoMembro))+(argc-3); //existe membros antigos, define quantos e soma com os novos
    return quantidade;
}

