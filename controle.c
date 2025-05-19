#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "controle.h"
#include "arquivo.h"
#include "gerais.h"
#include "listaConteudos.h"

int removerMembro(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, char *argv[], int quantidade, int *repeticao){

    //busca quem remover
    for (int i = 3; i < argc; i++){ //membros passados para remocao
        int achou = 0;
        for (int y = 0; y < quantidade-(argc-3); y++){ //membros antigos
            if (strcmp(infosImp[y].nome, argv[i]) == 0){ //compara nomes antigos com os passados
                achou = 1;
                repeticao[i-3] = infosImp[y].ordem; //guarda os encontrados
            }
        }
        if (achou == 0){
            fprintf(stderr, "Nao encontrou membro %s no arquivo", argv[i]);
        }
    }

    //remover membros
    for(int i = 0; i < argc-3; i++){//membros que serao removidos
        if (repeticao[i] != 0){ //quem deve ser removidos
            int diferenca = infosImp[repeticao[i]-1].tamanho;
            if (repeticao[i] < quantidade-(argc-3)){ //tem prox membro
                for(int y = repeticao[i]; y < quantidade-(argc-3); y++){ //percorre o resto dos membros
                    if (y == repeticao[i]){ //desloca uma vez
                        deslocarMembro(arquivo, infosImp[y].localOffset, -diferenca);
                    }
                    infosImp[y].localOffset -= diferenca;
                }
            }
            else{ //eh o ultimo membro
                deslocarMembro(arquivo, offsetLista, -diferenca);
            }
            offsetLista -= diferenca;
        }
    }

    //escreve no inicio do arquivo, o local da lista
    guardaLocal(arquivo, offsetLista);

    return offsetLista;
}

void moveMembro(FILE *arquivo, infoMembro *infosImp, int idMembro, int idTarget, int quantidade){
    char buffer[1024];
    
    if (idMembro == idTarget+1){//membro ja esta no local certo
        return;
    }

    //abrir espaco depois do target para o membro
    long targetOffset = infosImp[idTarget-1].localOffset;
    int targetTamanho = infosImp[idTarget-1].tamanho;
    int membroTamanho = infosImp[idMembro-1].tamanho;
    deslocarMembro(arquivo, targetOffset+targetTamanho, membroTamanho);

    //copia o membro para espaco depois do target
    long membroOffset = infosImp[idMembro-1].localOffset;
    int completude = membroTamanho;
    int lido = 0;
    //se copiar para tras, o membro muda o offset para offset+membroTamanho
    int movimento = 0;
    if (idMembro > idTarget){
        movimento = membroTamanho;
    }
    while (completude > 0){
        if (completude > 1024){
            fseek(arquivo, membroOffset+movimento+lido, SEEK_SET);
            fread(&buffer, 1, 1024, arquivo);
            fseek(arquivo, targetOffset+targetTamanho+lido, SEEK_SET);
            fwrite(&buffer, 1 ,1024, arquivo);
            lido += 1024;
            completude -= 1024;
        }
        else{
            fseek(arquivo, membroOffset+movimento+lido, SEEK_SET);
            fread(&buffer, 1, completude, arquivo);
            fseek(arquivo, targetOffset+targetTamanho+lido, SEEK_SET);
            fwrite(&buffer, 1, completude, arquivo);
            completude -= completude;
        }
    }

    //remover membro do seu local original
    deslocarMembro(arquivo, membroOffset+movimento+membroTamanho, -membroTamanho);

    
    //target antes do membro, offset dos membros entre eles aumenta pelo tamanho do membro
    if (idMembro > idTarget){
        for (int i = idTarget+1; i < idMembro; i++){
            infosImp[i-1].localOffset += membroTamanho;
            infosImp[i-1].ordem++;
        }
    }
    else{//target depois do membro, offset dos membros entre eles diminui pelo tamamnho do membro
        for (int i = idMembro+1; i <= idTarget; i++){
            infosImp[i-1].localOffset -= membroTamanho;
            infosImp[i-1].ordem--;
        }
    }

    //novo offset do membro, passa a ser depois do target
    infosImp[idMembro-1].localOffset = infosImp[idTarget-1].localOffset+infosImp[idTarget-1].tamanho;
    infosImp[idMembro-1].ordem = infosImp[idTarget-1].ordem+1;
}

void idsMembroTarget(infoMembro *infosImp, char *membro, char *target, int *idMembro, int *idTarget, int quantidade){
    for(int i = 0; i < quantidade-2; i++){ //para cada membro antigo
        if (strcmp(infosImp[i].nome, membro) == 0){ //define id do membro
            *idMembro = infosImp[i].ordem;
        }
        if (strcmp(infosImp[i].nome, target) == 0){ //define id do target
            *idTarget = infosImp[i].ordem;
        }
    }
}

void insereListaConteudo(FILE *arquivo, infoMembro *infosImp, long offsetLista, int argc, char *argv[], int quantidade, int *repeticao){

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
                repeticao[i-3] = infosImp[y].ordem; //guarda os repetidos

                //popula as structs com todos os dados dos membros
                stat(argv[i], &info[i-3]);

                //popula as structs com os dados importantes
                infosImp[y].UID = info[i-3].st_uid;
                infosImp[y].GID = info[i-3].st_gid;
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

    //popula a parte nova
    membrosTam = membrosNovos(infosImp, info, argc, argv, quantidade, repeticao, ordemAnterior, offsetAnterior, tamanhoAnterior, membrosTam);

    //local da nova lista de conteudos
    long local = membrosTam+sizeof(long);
    //escreve o array da lista dos membros
    escreveListaConteudo(arquivo, infosImp, local, argc, quantidade, repeticao);
    
    //escreve no inicio do arquivo, o local da lista
    guardaLocal(arquivo, local);
}

void extrairMembros(FILE *arquivo, infoMembro *infosImp, int argc, char *argv[], int quantidade, int *repeticao){
    char buffer[1024];

    int remover[quantidade-(argc-3)];
    memset(remover, 0, (quantidade-(argc-3))*sizeof(int)); //inicia vetor zerado

    if (argc == 3){//nenhum membro foi indicado, entao extrai todos
        for (int i = 0; i < quantidade-(argc-3); i++){
            remover[i] = 1; //marca todos para extracao
        }
    }
    else{//procura quem deve ser extraido e marca
        int achou = 0;
        for(int i = 3; i < argc; i++){ //membros passados
            for(int y = 0; y < quantidade-(argc-3); y++){ //membros antigos
                if (strcmp(argv[i], infosImp[y].nome) == 0){
                    achou = 1;
                    remover[y] = 1; //vai ser extraido
                }
            }
            if (achou == 0){
                fprintf(stderr, "item %s nao encontrado\n", argv[i]);
            }
        }
    }

    for (int i = 0; i < quantidade-(argc-3); i++){ //membros antigos
        if (remover[i] != 0){ //so extrai os marcados
            FILE *membro = abrirMembroW(infosImp[i].nome);
            int completude = infosImp[i].tamanho;
            int lido = 0;
            while (completude > 0){
                if (completude > 1024){
                    fseek(arquivo, infosImp[i].localOffset+lido, SEEK_SET);
                    fread(&buffer, 1, 1024, arquivo);
                    fseek(membro, lido, SEEK_SET);
                    fwrite(&buffer, 1, 1024, membro);
                    lido += 1024;
                    completude -=1024;
                }
                else{
                    fseek(arquivo, infosImp[i].localOffset+lido, SEEK_SET);
                    fread(&buffer, 1, completude, arquivo);
                    fseek(membro, lido, SEEK_SET);
                    fwrite(&buffer, 1, completude, membro);
                    completude -= completude;
                }
            }
        }
    }
}