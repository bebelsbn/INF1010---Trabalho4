#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main() {
    FILE *entrada = fopen("irene.txt", "rb");
    if (!entrada) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    int frequencias[256] = {0};
    contar_frequencias(entrada, frequencias);

    printf("Tabela de frequências:\n");
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0)
            printf("%c (%d): %d\n", i, i, frequencias[i]);
    }

    No *raiz = construir_arvore(frequencias);
    if (!raiz) {
        printf("Erro ao construir a árvore de Huffman.\n");
        fclose(entrada);
        return 1;
    }

    char *tabela[256] = {0};
    char codigo[100];
    gerar_codigos(raiz, codigo, 0, tabela);

    imprimir(tabela);

    // Parte da compactação
    FILE *compactado = fopen("saida_compactada.bin", "wb");
    if (!compactado) {
        printf("Erro ao criar o arquivo compactado.\n");
        fclose(entrada);
        liberar(raiz);
        return 1;
    }

    compactar_arquivo(entrada, compactado, tabela);
    fclose(compactado);
    printf("\nCompactação concluída \n");

    // Parte da descompactação
    FILE *arquivo_compactado = fopen("saida_compactada.bin", "rb");
    FILE *descompactado = fopen("saida_descompactada.txt", "w");
    if (!arquivo_compactado || !descompactado) {
        printf("Erro ao abrir os arquivos para descompactar.\n");
        fclose(entrada);
        liberar(raiz);
        return 1;
    }

    descompactar_arquivo(arquivo_compactado, descompactado, raiz);
    printf("Descompactação concluída \n");

    // Fechando tudo
    fclose(entrada);
    fclose(arquivo_compactado);
    fclose(descompactado);
    liberar(raiz);

    for (int i = 0; i < 256; i++) {
        free(tabela[i]);
    }

    return 0;
}
