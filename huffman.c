#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

// Estrutura auxiliar usada para montar a árvore de Huffman
// Funciona como uma lista de prioridade com ordenação por frequência
typedef struct {
    int tam;
    No* dados[256]; // Armazena até 256 nós (todos os caracteres ASCII)
} ListaPrioridade;


// FUNÇÕES PARA MANIPULAR A LISTA


// Cria e retorna um novo nó da árvore de Huffman
No* criar_no(unsigned char c, int f, No* e, No* d) {
    No* novo = (No*) malloc(sizeof(No));
    novo->caractere = c;    // caractere armazenado
    novo->freq = f;         // frequência de ocorrência
    novo->esq = e;          // filho esquerdo
    novo->dir = d;          // filho direito
    return novo;
}

// Inicializa a lista de prioridade (vazia)
void inicializar_lista(ListaPrioridade *lista) {
    lista->tam = 0;
}

// Insere um nó na lista mantendo a ordem crescente de frequência
void inserir_ordenado(ListaPrioridade *lista, No *n) {
    int i = lista->tam - 1;
    while (i >= 0 && lista->dados[i]->freq > n->freq) {
        lista->dados[i + 1] = lista->dados[i]; // desloca os maiores
        i--;
    }
    lista->dados[i + 1] = n;
    lista->tam++;
}

// Remove e retorna o nó com menor frequência (início da lista)
No* remover_minimo(ListaPrioridade *lista) {
    No* n = lista->dados[0];
    for (int i = 1; i < lista->tam; i++) {
        lista->dados[i - 1] = lista->dados[i]; // desloca para frente
    }
    lista->tam--;
    return n;
}


// CONSTRUÇÃO DA ÁRVORE DE HUFFMAN

// Lê o arquivo e preenche o vetor de frequências
void contar_frequencias(FILE *arquivo, int *frequencias) {
    unsigned char c;
    while (fread(&c, sizeof(unsigned char), 1, arquivo)) {
        frequencias[c]++; // incrementa a posição correspondente ao caractere
    }
    rewind(arquivo); // volta o ponteiro para o início
}

// Constrói a árvore de Huffman a partir do vetor de frequências
No* construir_arvore(int *frequencias) {
    ListaPrioridade lista;
    inicializar_lista(&lista);

    // Cria um nó para cada caractere com frequência > 0
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            inserir_ordenado(&lista, criar_no(i, frequencias[i], NULL, NULL));
        }
    }

    // Enquanto houver mais de um nó, junta os dois de menor frequência
    while (lista.tam > 1) {
        No* esq = remover_minimo(&lista);
        No* dir = remover_minimo(&lista);
        No* pai = criar_no('#', esq->freq + dir->freq, esq, dir); // '#' é um marcador interno
        inserir_ordenado(&lista, pai);
    }

    return remover_minimo(&lista); // nó final é a raiz da árvore
}


// GERAÇÃO DOS CÓDIGOS BINÁRIOS

// Percorre a árvore e gera códigos binários para cada caractere
void gerar_codigos(No *raiz, char *codigo, int nivel, char *tabela[256]) {
    if (!raiz) return;

    // Se é folha, salva o código na tabela
    if (!raiz->esq && !raiz->dir) {
        codigo[nivel] = '\0';
        tabela[raiz->caractere] = strdup(codigo); // salva uma cópia
        return;
    }

    // Esquerda = 0
    codigo[nivel] = '0';
    gerar_codigos(raiz->esq, codigo, nivel + 1, tabela);

    // Direita = 1
    codigo[nivel] = '1';
    gerar_codigos(raiz->dir, codigo, nivel + 1, tabela);
}

// Imprime a tabela de códigos gerados
void imprimir(char *tabela[256]) {
    printf("\nTabela de códigos Huffman:\n");
    for (int i = 0; i < 256; i++) {
        if (tabela[i])
            printf("%c: %s\n", i, tabela[i]);
    }
}


// Libera recursivamente todos os nós da árvore
void liberar(No *raiz) {
    if (!raiz) return;
    liberar(raiz->esq);
    liberar(raiz->dir);
    free(raiz);
}

// COMPACTAÇÃO


// Codifica os caracteres usando a tabela e grava os bits no arquivo
void compactar_arquivo(FILE *entrada, FILE *saida, char *tabela[256]) {
    rewind(entrada); // volta ao início do arquivo
    unsigned char buffer = 0;       // armazena bits temporariamente
    int bits_preenchidos = 0;       // contador de bits no buffer
    unsigned char c;
    int total_bits = 0;

    // Reserva os 4 primeiros bytes para gravar depois o total de bits úteis
    fseek(saida, 4, SEEK_SET);

    // Para cada caractere do arquivo
    while (fread(&c, sizeof(unsigned char), 1, entrada)) {
        char *codigo = tabela[c]; // código binário do caractere

        for (int i = 0; codigo[i] != '\0'; i++) {
            buffer = buffer << 1 | (codigo[i] - '0'); // insere o bit no buffer
            bits_preenchidos++;
            total_bits++;

            // Quando o buffer estiver cheio (8 bits), grava um byte
            if (bits_preenchidos == 8) {
                fwrite(&buffer, sizeof(unsigned char), 1, saida);
                buffer = 0;
                bits_preenchidos = 0;
            }
        }
    }

    // Grava os bits restantes (com padding à direita, se necessário)
    if (bits_preenchidos > 0) {
        buffer = buffer << (8 - bits_preenchidos);
        fwrite(&buffer, sizeof(unsigned char), 1, saida);
    }

    // Volta ao início e grava o total de bits úteis
    fseek(saida, 0, SEEK_SET);
    fwrite(&total_bits, sizeof(int), 1, saida);
}


// DESCOMPACTAÇÃO


// Lê o arquivo binário e recupera o texto original usando a árvore
void descompactar_arquivo(FILE *entrada, FILE *saida, No *raiz) {
    int total_bits;
    fread(&total_bits, sizeof(int), 1, entrada); // lê a quantidade de bits válidos

    No *atual = raiz;
    unsigned char c;
    int bits_lidos = 0;

    // Leitura bit a bit do arquivo
    while (bits_lidos < total_bits && fread(&c, sizeof(unsigned char), 1, entrada)) {
        for (int i = 7; i >= 0 && bits_lidos < total_bits; i--) {
            int bit = (c >> i) & 1; // extrai bit a bit
            bits_lidos++;

            // Navega na árvore de acordo com o bit
            atual = bit ? atual->dir : atual->esq;

            // Se chegou em uma folha, escreve o caractere
            if (!atual->esq && !atual->dir) {
                fputc(atual->caractere, saida);
                atual = raiz; // volta para o topo da árvore
            }
        }
    }
}
