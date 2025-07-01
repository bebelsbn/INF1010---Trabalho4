# Trabalho 4 – Compressão de Arquivos com Algoritmo de Huffman  
**Disciplina:** INF1010 – Estruturas de Dados  

## Objetivo

Este trabalho tem como objetivo implementar um compactador e um descompactador de arquivos texto utilizando o **algoritmo de Huffman**, aplicando conceitos estudados em aula como **árvores binárias**, **listas de prioridade**, manipulação de **bits**, e **alocação dinâmica de memória**.

## Estrutura do Projeto

Trabalho04/
├── main.c                    # Função principal: chama compactação e descompactação
├── huffman.c                 # Implementação do algoritmo de Huffman
├── huffman.h                 # Estrutura de dados e protótipos das funções
├── irene.txt                 # Arquivo de entrada original (texto de Manuel Bandeira)
├── saida\_compactada.bin      # Arquivo compactado gerado
├── saida\_descompactada.txt   # Arquivo descompactado
├── Trabalho 4 .pdf           # Enunciado do trabalho



## Conceitos Utilizados

- **Árvore de Huffman:** estrutura binária em que cada caractere é representado por um caminho único de bits.
- **Lista de Prioridade:** usada para construir a árvore a partir das frequências dos caracteres.
- **Manipulação de Bits:** compressão e descompressão são feitas diretamente com bits via operadores `<<`, `>>` e `&`.
- **Alocação Dinâmica:** os nós da árvore são alocados dinamicamente com `malloc` e liberados ao final do programa.


## Compilação e Execução

Para compilar e executar o programa:

```bash
    gcc main.c huffman.c -o huffman
    ./huffman
````

Após a execução:

* O programa gera o arquivo `saida_compactada.bin` a partir de `irene.txt`.
* Depois, descompacta o binário e gera `saida_descompactada.txt`, que deve ser igual ao original.

## Observações

* O arquivo original “**irene.txt**” foi padronizado conforme exigido: **sem acentuação**, apenas letras **minúsculas**, contendo apenas caracteres ASCII e **espaços/\n** como símbolos especiais.
* A compressão é **sem perda** e utiliza a codificação de menor comprimento para os caracteres mais frequentes, como esperado pelo algoritmo de Huffman.
* A árvore é reconstruída somente com base na frequência dos caracteres do texto original, sem armazenar a estrutura da árvore no arquivo binário.


## Resultados

* Compressão funcional e eficaz.
* Arquivo descompactado idêntico ao original.
* Utilização correta de estruturas e técnicas ensinadas em aula.
