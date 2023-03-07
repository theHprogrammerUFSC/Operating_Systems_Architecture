// Author: Helder Henrique da Silva
// Year: 2022/2
//
// Description: Implementação de uma lista encadeada em disco.
//

#ifndef fs_h
#define fs_h
#include <string>
#include <cstring>

using namespace std;

// Definindo a estrutura do Nodo
typedef struct
{
    int estaNaLista;
    char nome[20];
    int proxNodo;
} Nodo;

int achaNodoVazio(FILE *arquivo)
{
    Nodo auxiliar;
    while (fread(&auxiliar, sizeof(Nodo), 1, arquivo))
    {
        if (auxiliar.estaNaLista == 0)
        {
            return ftell(arquivo) - sizeof(Nodo);
        }
        fseek(arquivo, auxiliar.proxNodo, SEEK_SET);
    }
    return 0;
}

bool pegaNodo(FILE *arquivo, Nodo *auxiliar)
{
    if (!fread(auxiliar, sizeof(Nodo), 1, arquivo))
    {
        return false;
    }
    if (auxiliar->proxNodo <= 0)
    {
        return false;
    }
    return true;
}

int procuraDepoisDesteNome(FILE *arquivo, const char nome[])
{
    Nodo auxiliar;
    while (pegaNodo(arquivo, &auxiliar))
    {
        if (strncmp(auxiliar.nome, nome, sizeof(auxiliar.nome)) == 0)
        {
            return ftell(arquivo) - sizeof(Nodo);
        }
        fseek(arquivo, auxiliar.proxNodo, SEEK_SET);
    }
    return 0;
}

/**
 * @param arquivoDaLista nome do arquivo em disco que contem a lista encadeada
 * @param novoNome nome a ser adicionado apos depoisDesteNome
 * @param depoisDesteNome um nome presente na lista
 */
void adiciona(std::string arquivoDaLista, std::string novoNome, std::string depoisDesteNome)
{
    /* Abre o arquivo em modo de leitura e escrita */
    FILE *arquivo = fopen(arquivoDaLista.c_str(), "r+");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir arquivo!\n");
        system("pause");
        exit(1);
    }

    // Posicionando o ponteiro no inicio do arquivo, lê os 4 primeiros bytes e posiciona o ponteiro no primeiro nodo usado.
    fseek(arquivo, 0, SEEK_SET);
    int pInicial = 0;
    int pNodoVazio = 0;
    fread(&pInicial, sizeof(int), 1, arquivo);
    if (pInicial == 0)
    {
        printf("Lista vazia!\n");
        system("pause");
        exit(1);
    }
    else if (pInicial == -1)
    {
        printf("Erro\n");
        system("pause");
        exit(1);
    }
    else
    {
        /* Pega o primeiro nodo vazio */
        pNodoVazio = achaNodoVazio(arquivo);
        if (pNodoVazio == 0)
        {
            printf("Erro ao achar nodo vazio!\n");
            system("pause");
            exit(1);
        }
        fseek(arquivo, pInicial, SEEK_SET);
    }

    /* Procura nodo que contem depoisDesteNome */
    int pNodoDepoisDesteNome = procuraDepoisDesteNome(arquivo, depoisDesteNome.c_str());
    if (pNodoDepoisDesteNome == 0)
    {
        printf("Erro em pegar o proximo nodo depoisDesteNome!\n");
        system("pause");
        exit(1);
    }

    /* Aloca o novo nodo */
    Nodo novoNodo;
    novoNodo.estaNaLista = 1;
    strncpy(novoNodo.nome, novoNome.c_str(), sizeof(novoNodo.nome));

    Nodo proximoNodo;
    fseek(arquivo, pNodoDepoisDesteNome, SEEK_SET);
    fread(&proximoNodo, sizeof(Nodo), 1, arquivo);
    novoNodo.proxNodo = proximoNodo.proxNodo;

    /* Escreve o novo nodo no arquivo */
    fseek(arquivo, pNodoVazio, SEEK_SET);
    fwrite(&novoNodo, sizeof(Nodo), 1, arquivo);

    /* Atualiza o nodo anterior ao novo nodo */
    Nodo nodoAnterior;
    fseek(arquivo, pNodoDepoisDesteNome, SEEK_SET);
    fread(&nodoAnterior, sizeof(Nodo), 1, arquivo);
    nodoAnterior.proxNodo = pNodoVazio;
    fseek(arquivo, pNodoDepoisDesteNome, SEEK_SET);
    fwrite(&nodoAnterior, sizeof(Nodo), 1, arquivo);

    /* Fecha o arquivo */
    fclose(arquivo);
}

#endif /* fs_h */