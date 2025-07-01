#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

typedef struct {
    int tam;
    No* dados[256];
} ListaPrioridade;

// Cria um novo nó
No* criar_no(unsigned char c, int f, No* e, No* d) {
    No* novo = (No*) malloc(sizeof(No));
    novo->caractere = c;
    novo->freq = f;
    novo->esq = e;
    novo->dir = d;
    return novo;
}

// Inicializa a lista
void inicializar_lista(ListaPrioridade *lista) {
    lista->tam = 0;
}

// Insere nó na ordem certa
void inserir_ordenado(ListaPrioridade *lista, No *n) {
    int i = lista->tam - 1;
    while (i >= 0 && lista->dados[i]->freq > n->freq) {
        lista->dados[i + 1] = lista->dados[i];
        i--;
    }
    lista->dados[i + 1] = n;
    lista->tam++;
}

// Pega o menor nó e remove da lista
No* remover_minimo(ListaPrioridade *lista) {
    No* n = lista->dados[0];
    for (int i = 1; i < lista->tam; i++) {
        lista->dados[i - 1] = lista->dados[i];
    }
    lista->tam--;
    return n;
}

// Conta quantas vezes cada caractere aparece
void contar_frequencias(FILE *arquivo, int *frequencias) {
    unsigned char c;
    while (fread(&c, sizeof(unsigned char), 1, arquivo)) {
        frequencias[c]++;
    }
    rewind(arquivo);
}

// Monta a árvore de Huffman
No* construir_arvore(int *frequencias) {
    ListaPrioridade lista;
    inicializar_lista(&lista);

    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            inserir_ordenado(&lista, criar_no(i, frequencias[i], NULL, NULL));
        }
    }

    while (lista.tam > 1) {
        No* esq = remover_minimo(&lista);
        No* dir = remover_minimo(&lista);
        No* pai = criar_no('#', esq->freq + dir->freq, esq, dir);
        inserir_ordenado(&lista, pai);
    }

    return remover_minimo(&lista);
}

// Gera os códigos da árvore (recursivo)
void gerar_codigos(No *raiz, char *codigo, int nivel, char *tabela[256]) {
    if (!raiz) return;

    if (!raiz->esq && !raiz->dir) {
        codigo[nivel] = '\0';
        tabela[raiz->caractere] = strdup(codigo);
        return;
    }

    codigo[nivel] = '0';
    gerar_codigos(raiz->esq, codigo, nivel + 1, tabela);

    codigo[nivel] = '1';
    gerar_codigos(raiz->dir, codigo, nivel + 1, tabela);
}


void imprimir(char *tabela[256]) {
    printf("\nTabela de códigos Huffman:\n");
    for (int i = 0; i < 256; i++) {
        if (tabela[i])
            printf("%c: %s\n", i, tabela[i]);
    }
}


void liberar(No *raiz) {
    if (!raiz) return;
    liberar(raiz->esq);
    liberar(raiz->dir);
    free(raiz);
}

// Compacta usando os códigos da tabela
void compactar_arquivo(FILE *entrada, FILE *saida, char *tabela[256]) {
    rewind(entrada);
    unsigned char buffer = 0;
    int bits_preenchidos = 0;
    unsigned char c;
    int total_bits = 0;

    // Reservar espaço no começo pra guardar total de bits
    fseek(saida, 4, SEEK_SET);

    while (fread(&c, sizeof(unsigned char), 1, entrada)) {
        char *codigo = tabela[c];
        for (int i = 0; codigo[i] != '\0'; i++) {
            buffer = buffer << 1 | (codigo[i] - '0');
            bits_preenchidos++;
            total_bits++;

            if (bits_preenchidos == 8) {
                fwrite(&buffer, sizeof(unsigned char), 1, saida);
                buffer = 0;
                bits_preenchidos = 0;
            }
        }
    }

    if (bits_preenchidos > 0) {
        buffer = buffer << (8 - bits_preenchidos);
        fwrite(&buffer, sizeof(unsigned char), 1, saida);
    }

    
    fseek(saida, 0, SEEK_SET);
    fwrite(&total_bits, sizeof(int), 1, saida);
}


void descompactar_arquivo(FILE *entrada, FILE *saida, No *raiz) {
    int total_bits;
    fread(&total_bits, sizeof(int), 1, entrada);

    No *atual = raiz;
    unsigned char c;
    int bits_lidos = 0;

    while (bits_lidos < total_bits && fread(&c, sizeof(unsigned char), 1, entrada)) {
        for (int i = 7; i >= 0 && bits_lidos < total_bits; i--) {
            int bit = (c >> i) & 1;
            bits_lidos++;

            atual = bit ? atual->dir : atual->esq;

            if (!atual->esq && !atual->dir) {
                fputc(atual->caractere, saida);
                atual = raiz;
            }
        }
    }
}
