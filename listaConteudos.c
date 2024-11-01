#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "listaConteudos.h"
#include "arquivo.h"

//imprime dados dos membros
void imprimirLista(infoMembro *infosImp, int quantidade){
    for (int i = 0; i < quantidade; i++){
        printf("\n");
        char modificacao[25];
        snprintf(modificacao, 25, "%s", ctime(&infosImp[i].modificacao));
        modificacao[24] = '\0';

        printf("%o  %ld  %7d  %s  %20s", infosImp[i].permissao, infosImp[i].UID, infosImp[i].tamanho, modificacao, infosImp[i].nome);
    }
}

long localListaConteudo(FILE *arquivo){
    long local;

    //descobre tamanho do arquivo
    long tamanho = tamanhoArq(arquivo);

    if (tamanho == 0){ //se arquivo ta vazio, lista de conteudo fica no inicio
        return sizeof(long);
    }
    
    //le um long int no inicio do arquivo que representa local da lista
    fseek(arquivo, 0, SEEK_SET);
    fread(&local, 1, sizeof(long), arquivo);

    return local;
}

void atualizaListaConteudo(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, char *argv[], int quantidade, int *repeticao){
    
    int membrosTam = 0;

    //array das structs com todos os dados dos novos
    struct stat info[argc-3];

    //prepara infosImp, e popula parte antiga
    int ordemAnterior = 0;
    int offsetAnterior = sizeof(long);
    int tamanhoAnterior = 0;
    membrosTam = lerListaConteudo(arquivo, infosImp, offsetLista); //le infosImp do arquivo, parte antiga
    //valores do ultimo membro
    if (membrosTam > 0){
        ordemAnterior = infosImp[quantidade-(argc-3)-1].ordem;
        offsetAnterior = infosImp[quantidade-(argc-3)-1].localOffset;
        tamanhoAnterior = infosImp[quantidade-(argc-3)-1].tamanho;
    }

    //busca repeticoes 
    for (int i = 3; i < argc; i++){ //membros novos
        for (int y = 0; y < quantidade-(argc-3); y++){ //membros antigos
            if (strcmp(infosImp[y].nome, argv[i]) == 0){ //compara nomes antigos com novos

                repeticao[i-3] = -1; //-1 nao precisa atualizar

                //pega informacoes sobre argv[i], para conseguir seu tempo de modificacao
                struct stat temp;
                stat(argv[i], &temp);

                if (difftime(temp.st_mtime, infosImp[y].modificacao) > 0){ //testa se o arquivo foi modificado ou nao
                    //popula as structs com todos os dados dos membros
                    stat(argv[i], &info[i-3]);

                    //popula as structs com os dados importantes
                    infosImp[y].UID = info[i-3].st_uid;
                    infosImp[y].permissao = (info[i-3].st_mode & 0x1FF);
                    infosImp[y].modificacao = info[i-3].st_mtime;

                    //se tamanho mudou
                    if (infosImp[y].tamanho != info[i-3].st_size){
                        membrosTam -= infosImp[y].tamanho; //remover tamanho antigo
                        int diferenca = info[i-3].st_size - infosImp[y].tamanho;
                        infosImp[y].tamanho = info[i-3].st_size; //atualiza tamanho
                        membrosTam += infosImp[y].tamanho; //adiciona novo tamanho total
        
                        for (int j = y+1; j < quantidade-(argc-3); j++){ //percorre resto dos membros antigos arrumando offset e deslocando
                            if (j == y+1){ //primeira execucao, desloca o resto do arquivo uma vez
                                deslocarMembro(arquivo, infosImp[y+1].localOffset, diferenca);
                            }
                            infosImp[j].localOffset = infosImp[j].localOffset + diferenca;//arruma o resto dos offsets
                        }
                        
                        offsetAnterior = infosImp[quantidade-(argc-3)-1].localOffset; //arruma offset para os novos
                        if (y == quantidade-(argc-3)-1){//tamanho do ultimo antigo mudou
                            tamanhoAnterior = infosImp[quantidade-(argc-3)-1].tamanho;//arruma para definir os novos
                        }
                    }
                }
            }
        }
    }

    //popula a parte nova
    membrosTam = membrosNovos(infosImp, info, argc, argv, quantidade, repeticao, ordemAnterior, offsetAnterior, tamanhoAnterior, membrosTam);

    //local da nova lista de conteudos
    long local = membrosTam+sizeof(long);
    //escreve o array da lista dos membros
    escreveListaConteudo(arquivo, infosImp, local, argc, quantidade, repeticao);
    
    //escreve no inicio do arquivo, o local da lista
    guardaLocal(arquivo, local);
}


//le do arquivo as informacoes dos membros para infosImp[]
int lerListaConteudo(FILE *arquivo, infoMembro *infosImp, long offsetLista){
    int tamanho = tamanhoArq(arquivo);
    int membrosTam = 0;
    if (tamanho > 0){ //existe membros antigos
        fseek(arquivo, offsetLista, SEEK_SET); //vai ate o local da lista de conteudos
        for (int i = 0; fread(&infosImp[i], 1, sizeof(infoMembro), arquivo) == sizeof(infoMembro) ; i++){//popula com os valores anteriores
            membrosTam += infosImp[i].tamanho;
        }
        return membrosTam;
    }
    else{
        // fprintf(stderr, "arquivo nao possui nenhum membro");
        return 0;
    }
}

//popula nos infosImp[] informacoes dos membros novos
int membrosNovos(infoMembro *infosImp, struct stat *info, int argc, char *argv[], int quantidade, int *repeticao, int ordemAnterior, int offsetAnterior, int tamanhoAnterior, int membrosTam){
    for(int i = quantidade-(argc-3), y = 0; i < quantidade; i++, y++){ //para cada novo membro
        if (repeticao[y] == 0){//se for 0, nao eh repeticao
            ordemAnterior++;
            
            //popula as structs com todos os dados dos membros
            stat(argv[y+3], &info[y]);

            //popula as structs com os dados importantes
            memset(infosImp[i].nome, 0, 255);
            strcpy(infosImp[i].nome, argv[y+3]);
            infosImp[i].UID = info[y].st_uid;
            infosImp[i].permissao = (info[y].st_mode & 0x1FF);
            infosImp[i].tamanho = info[y].st_size;
            infosImp[i].modificacao = info[y].st_mtime;
            infosImp[i].ordem = ordemAnterior;
            infosImp[i].localOffset = offsetAnterior + tamanhoAnterior;
            offsetAnterior = infosImp[i].localOffset; //novo offset
            tamanhoAnterior = infosImp[i].tamanho; //novo tamanho

            membrosTam += infosImp[i].tamanho;
        }
    }
    return membrosTam;
}

//escreve no arquivo informacoes dos membros
void escreveListaConteudo(FILE *arquivo, infoMembro *infosImp, long local, int argc, int quantidade, int *repeticao){
    //local da lista
    fseek(arquivo, local, SEEK_SET); 

    //escreve no arquivo informacoes dos membros
    for (int i = 0; i < quantidade; i++){
        if (i < quantidade-(argc-3)){//escreve antigos
            fwrite(&infosImp[i], 1, sizeof(infoMembro), arquivo);
        }
        else if(repeticao[i-(quantidade-(argc-3))] == 0){ //escreve novos que nao repetiram
            fwrite(&infosImp[i], 1, sizeof(infoMembro), arquivo);
        }
    }
}

//guarda offset da lista de conteudos no inicio do arquivo
void guardaLocal(FILE *arquivo, long local){
    fseek(arquivo, 0L, SEEK_SET);
    fwrite(&local, 1, sizeof(long), arquivo);
}

void moverNaLista(FILE *arquivo, infoMembro *infosImp, long offsetLista, int idMembro, int idTarget, int quantidade){
    //vai ate local da lista
    fseek(arquivo, offsetLista, SEEK_SET);

    //escreve info dos membros na ordem que ja foi ajustada
    for(int y = 1; y < quantidade-1; y++){
        for (int i = 0; i < quantidade-2; i++){
            if (infosImp[i].ordem == y){ //escreve quando "ordem" corresponde
                fwrite(&infosImp[i], 1, sizeof(infoMembro), arquivo);
            }
        }
    }
}

void removerDaLista(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, int quantidade, int *repeticao){
    //vai ate local da lista
    fseek(arquivo, offsetLista, SEEK_SET);

    //rescreve a lista de membros sem os removidos
    int novaordem = 1;
    for (int i = 0; i < quantidade-(argc-3); i++){ //passa pela lista dos membros do arquivo
        int achou = 0;
        for (int y = 0; y < argc-3; y++){//procura membro em repeticoes
            if (infosImp[i].ordem == repeticao[y]){ //se achou nao escreve
                achou = 1;
            }
        }
        if (achou == 0){ //se nao achou, escreve
            infosImp[i].ordem = novaordem;
            fwrite(&infosImp[i], 1, sizeof(infoMembro), arquivo);
            novaordem++;
        }
    }

    int quantRemovidos = 0;
    for (int y = 0; y < argc-3; y++){//procura membro em repeticoes
        if (repeticao[y] != 0){ //conta quantos foram realmente removidos
            quantRemovidos++;
        }
    }

    //remove o lixo que ficou no final
    ftruncate(fileno(arquivo), tamanhoArq(arquivo)-(quantRemovidos*sizeof(infoMembro)));

    //escreve no inicio do arquivo, o local da lista
    guardaLocal(arquivo, offsetLista);
}