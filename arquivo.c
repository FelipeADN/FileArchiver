#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "arquivo.h"

void gravarMembros(FILE *arquivo, infoMembro *infosImp, int argc, int quantidade, int *repeticao){
    char *buffer[1024];

    //membros novos repetidos(atualizacoes)
    for (int i = 0; i < argc-3; i++){
        if (repeticao[i] > 0){ //se for -1(quando -a tem mesma data) nao atualiza ou 0 nao atualiza
            long tempOffset = infosImp[repeticao[i]-1].localOffset;
            fseek(arquivo, tempOffset, SEEK_SET); //vai ate offset do novo membro no arq
            int completude = infosImp[repeticao[i]-1].tamanho;
            long lido = 0;
            FILE *membro = abrirMembroR(infosImp[repeticao[i]-1].nome);
            while (completude > 0){ //enquanto ainda precisa escrever
                if (completude > 1024){ //escrever ate tamanho do buffer
                    fseek(membro, lido, SEEK_SET); //offset dentro do membro
                    fread(buffer, 1, 1024, membro);
                    fwrite(buffer, 1, 1024, arquivo);
                    completude -= 1024;
                    lido += 1024;
                    tempOffset += 1024;
                }
                else { //menor que buffer (ultima escrita)
                    fseek(membro, lido, SEEK_SET);
                    fread(buffer, 1, completude, membro);
                    fwrite(buffer, 1, completude, arquivo);
                    completude -= completude;
                }
                fseek(arquivo, tempOffset, SEEK_SET); //offset dentro do arquivo
            }
            fclose(membro);
        }
    }

    //inserir membros novos
    for(int i = quantidade-(argc-3); i < quantidade; i++){
        if (repeticao[i-(quantidade-(argc-3))] == 0){
            long tempOffset = infosImp[i].localOffset;
            fseek(arquivo, tempOffset, SEEK_SET); //vai ate offset do novo membro no arq
            int completude = infosImp[i].tamanho;
            long lido = 0;
            FILE *membro = abrirMembroR(infosImp[i].nome);
            while (completude > 0){ //enquanto ainda precisa escrever
                if (completude > 1024){ //escrever ate tamanho do buffer
                    fseek(membro, lido, SEEK_SET); //offset dentro do membro
                    fread(buffer, 1, 1024, membro);
                    fwrite(buffer, 1, 1024, arquivo);
                    completude -= 1024;
                    lido += 1024;
                    tempOffset += 1024;
                }
                else { //menor que buffer (ultima escrita)
                    fseek(membro, lido, SEEK_SET);
                    fread(buffer, 1, completude, membro);
                    fwrite(buffer, 1, completude, arquivo);
                    completude -= completude;
                }
                fseek(arquivo, tempOffset, SEEK_SET); //offset dentro do arquivo
            }
            fclose(membro);
        }
    }
}

//desloca todos os dados de um ponto(inicio) ate o final do arquivo uma diferenca em relacao ao inicio
void deslocarMembro(FILE *arquivo, long inicio, int diferenca){
    char *buffer[1024];
    int tamanho = tamanhoArq(arquivo);
    int completude = tamanho - inicio;
    if (diferenca > 0){ //membro aumentou (escrever a partir do final)
        long lido = 0;
        while (completude > 0){ //ainda precisa escrever
            if (completude > 1024){ //escrever limite do buffer
                fseek(arquivo, tamanho-(lido+1024), SEEK_SET);
                fread(buffer, 1, 1024, arquivo);
                fseek(arquivo, tamanho-(lido+1024)+diferenca, SEEK_SET);
                fwrite(buffer, 1, 1024, arquivo);
                lido += 1024;
                completude -= 1024;
            }
            else{ //menos que o limite (ultima escrita)
                fseek(arquivo, tamanho-lido-completude, SEEK_SET);
                fread(buffer, 1, completude, arquivo);
                fseek(arquivo, tamanho-lido-completude+diferenca, SEEK_SET);
                fwrite(buffer, 1, completude, arquivo);
                completude -= completude;
            }
        }
        
    }
    else{ //membro diminuiu (escrever a partir do offset)
        long lido = 0;
        while (completude > 0){ // ainda precisa escrever
            if (completude > 1024){ // escrever limite do buffer
                fseek(arquivo, inicio+lido, SEEK_SET);
                fread(buffer, 1, 1024, arquivo);
                fseek(arquivo, inicio+lido+diferenca, SEEK_SET);
                fwrite(buffer, 1, 1024, arquivo);
                lido += 1024;
                completude -= 1024;
            }
            else{ //menos que o limite (ultima escrita)
                fseek(arquivo, inicio+lido, SEEK_SET);
                fread(buffer, 1, completude, arquivo);
                fseek(arquivo, inicio+lido+diferenca, SEEK_SET);
                fwrite(buffer, 1, completude, arquivo);
                completude -= completude;
            }
        }
        ftruncate(fileno(arquivo), tamanho+diferenca);
    }
}
