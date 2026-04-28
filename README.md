# Analise Estatistica - Jogo do Bicho

Projeto desenvolvido em C focado na geração, ordenação e extração de dados simulados do Jogo do Bicho para fins de estudo em Estatística Descritiva.

## Sobre o Projeto
Este repositório contém os algoritmos necessários para construir uma série histórica cronológica de resultados do Jogo do Bicho. O objetivo é aplicar técnicas de Engenharia de Software, processar os dados em linguagem C e exportá-los para análises externas em planilhas como o LibreOffice Calc.

O código resolve problemas de uso excessivo de RAM aplicando conceitos de ordenação externa (Merge Sort) e alocação em blocos de memória.

## Arquitetura e Funcionalidades
* **Gerador de Série Histórica (`criar_jogo_bicho.c`):** Simula sorteios aleatórios de datas e milhares, ordenando os dados cronologicamente via Merge Sort com ficheiros temporários para manter a sequência de concursos. Aceita a quantidade de sorteios como argumento dinâmico.
* **Extrator e Decodificador ETL (`bicho_exportado_csv.c`):** Lê o histórico em formato texto sequencial e aplica funções matemáticas para decodificar Milhar, Dezena e o Grupo (Bicho).
* **Exportação CSV:** Converte os dados estruturados em `.csv` (separado por `;`) padronizado para ferramentas de análise quantitativa.

## Metodologia Estatística
Os dados gerados por este repositório permitem a aplicação de Estatística Descritiva, como:
* Distribuição de Frequência Absoluta e Relativa (Variáveis Qualitativas - Grupos/Bichos).
* Medidas de Tendência Central e Dispersão (Variáveis Quantitativas - Milhar/Dezena).
* Elaboração de Histogramas, Box Plots e Gráficos de Barras.

## Como Executar

**1. Compilar os códigos:**
Abra o terminal na pasta do projeto e utilize o compilador `gcc`:
```bash
gcc criar_jogo_bicho.c -o criar_jogo_bicho
gcc bicho_exportado_csv.c -o bicho_exportado_csv
