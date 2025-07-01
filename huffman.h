#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  

typedef struct no {
    unsigned char caractere;
    int freq;
    struct no *esq, *dir;
} No;

// Protótipos de função
void contar_frequencias(FILE *arquivo, int *frequencias);
No* construir_arvore(int *frequencias);
void gerar_codigos(No *raiz, char *codigo, int nivel, char *tabela[256]);
void compactar_arquivo(FILE *entrada, FILE *saida, char *tabela[256]);
void descompactar_arquivo(FILE *entrada, FILE *saida, No *raiz);
void imprimir(char *tabela[256]);
void liberar(No *raiz);


#endif
