#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h> //PT-BR

#define LINE_LENGTH 256
#define MAX_MEMORY_RECORDS 1000 // Tamanho do bloco para a RAM
/*#define QUANTIDADE_TOTAL 1000   // Total de sorteios que queremos gerar*/
#define MAX_TEMP_FILES 200      //Permitir para até 200.000 registros

// Estrutura adaptada para o Merge Sort
typedef struct {
    int dia, mes, ano;
    int p[5]; // Os 5 milhares
} SorteioBruto;

typedef struct {
    FILE *arquivo;
    SorteioBruto registro;
    int finalizado;
    char nome_temp[LINE_LENGTH];
} FonteTemporaria;

int compare_Date(const void *a, const void *b) {
    const SorteioBruto *va = (const SorteioBruto *)a;
    const SorteioBruto *vb = (const SorteioBruto *)b;

    if (va->ano != vb->ano) return va->ano - vb->ano;
    if (va->mes != vb->mes) return va->mes - vb->mes;
    return va->dia - vb->dia;
}

void gerarDataAleatoria(int *dia, int *mes, int *ano) {
    *ano = rand() % 5 + 2020;
    *mes = rand() % 12 + 1;
    int dias_no_mes = 31;
    if (*mes == 2)
        dias_no_mes = ((*ano % 4 == 0 && *ano % 100 != 0) || (*ano % 400 == 0)) ? 29 : 28;
    else if (*mes == 4 || *mes == 6 || *mes == 9 || *mes == 11)
        dias_no_mes = 30;
    *dia = rand() % dias_no_mes + 1;
}

//Etapa de DIVISÃO: Gera em blocos, ordena na RAM e salva em Temps
int gerarEDividir(char temp_files[][LINE_LENGTH], int quantidade_total) {
    int file_count = 0;
    int gerados = 0;
    SorteioBruto *buffer = malloc(MAX_MEMORY_RECORDS * sizeof(SorteioBruto));

    if (!buffer) return 0;

    while (gerados < quantidade_total) {
        int records_in_chunk = 0;

        while (records_in_chunk < MAX_MEMORY_RECORDS && gerados < quantidade_total) {
            gerarDataAleatoria(&buffer[records_in_chunk].dia, &buffer[records_in_chunk].mes, &buffer[records_in_chunk].ano);
            for(int j = 0; j < 5; j++) {
                buffer[records_in_chunk].p[j] = rand() % 10000;
            }
            records_in_chunk++;
            gerados++;
        }

        // Ordena o bloco atual cronologicamente
        qsort(buffer, records_in_chunk, sizeof(SorteioBruto), compare_Date);

        // Salva o bloco ordenado em um arquivo temporário
        sprintf(temp_files[file_count], "temp_%d.txt", file_count);
        FILE *temp = fopen(temp_files[file_count], "w");
        for (int i = 0; i < records_in_chunk; i++) {
            fprintf(temp, "%02d/%02d/%04d %04d %04d %04d %04d %04d\n",
                    buffer[i].dia, buffer[i].mes, buffer[i].ano,
                    buffer[i].p[0], buffer[i].p[1], buffer[i].p[2], buffer[i].p[3], buffer[i].p[4]);
        }
        fclose(temp);
        file_count++;
    }

    free(buffer);
    return file_count;
}

// Auxiliar para puxar informações do arquivo temporário
int loadArquivosBicho(FonteTemporaria *fonte) {
    char linha[LINE_LENGTH];
    if (!fgets(linha, LINE_LENGTH, fonte->arquivo)) {
        fonte->finalizado = 1;
        return 0;
    }
    int lidos = sscanf(linha, "%d/%d/%d %d %d %d %d %d",
                       &fonte->registro.dia, &fonte->registro.mes, &fonte->registro.ano,
                       &fonte->registro.p[0], &fonte->registro.p[1], &fonte->registro.p[2],
                       &fonte->registro.p[3], &fonte->registro.p[4]);

    if (lidos != 8) {
        fonte->finalizado = 1;
        return 0;
    }
    return 1;
}

// Etapa de CONQUISTA: Merge e Atribuição do Concurso
void conquerArquivo(int file_count, char temp_files[][LINE_LENGTH], const char *output_file) {
    FILE *output = fopen(output_file, "w");
    if (!output) return;

    FonteTemporaria *fontes = malloc(file_count * sizeof(FonteTemporaria));

    for (int i = 0; i < file_count; i++) {
        strncpy(fontes[i].nome_temp, temp_files[i], LINE_LENGTH);
        fontes[i].arquivo = fopen(temp_files[i], "r");
        fontes[i].finalizado = 0;
        if (!fontes[i].arquivo || !loadArquivosBicho(&fontes[i])) {
            fontes[i].finalizado = 1;
        }
    }

    int ativos = file_count;
    int concursoSequencial = 1; // O concurso nasce aqui, perfeitamente em ordem!

    while (ativos > 0) {
        int min_index = -1;
        // Procura a data mais antiga entre as cabeças de leitura dos arquivos temporários
        for (int i = 0; i < file_count; i++) {
            if (!fontes[i].finalizado) {
                if (min_index == -1 || compare_Date(&fontes[i].registro, &fontes[min_index].registro) < 0) {
                    min_index = i;
                }
            }
        }

        if (min_index != -1) {
            // Grava a data mais antiga e injeta o concurso no formato: Data Concurso P1 P2 P3 P4 P5
            fprintf(output, "%02d/%02d/%04d %d %04d %04d %04d %04d %04d\n",
                    fontes[min_index].registro.dia, fontes[min_index].registro.mes, fontes[min_index].registro.ano,
                    concursoSequencial,
                    fontes[min_index].registro.p[0], fontes[min_index].registro.p[1], fontes[min_index].registro.p[2],
                    fontes[min_index].registro.p[3], fontes[min_index].registro.p[4]);

            concursoSequencial++; // Incrementa para a próxima iteração

            if (!loadArquivosBicho(&fontes[min_index])) {
                ativos--;
            }
        }
    }

    // Limpeza (Fecha e exclui os arquivos temporários)
    for (int i = 0; i < file_count; i++) {
        if (fontes[i].arquivo) fclose(fontes[i].arquivo);
        remove(fontes[i].nome_temp);
    }
    free(fontes);
    fclose(output);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));
    char temp_files[MAX_TEMP_FILES][LINE_LENGTH];
    const char* arquivoFinal = "bicho_historico.txt";
    int quantidade = 100; //Valor Padrão caso não informado

    if (argc > 1) {
        quantidade = atoi(argv[1]);
        if (quantidade <= 0) {
            printf("Quantidade invalida fornecida. Utilizando valor padrao de 100.\n");
            quantidade = 100;
        }
    }

    printf("Iniciando geração de dados do Jogo do Bicho...\n");

    // Divide (Gera e ordena na RAM)
    int num_files = gerarEDividir(temp_files, quantidade);

    if (num_files > 0) {
        // Conquista (Faz o Merge e atrela o Concurso)
        conquerArquivo(num_files, temp_files, arquivoFinal);
        printf("Série histórica de %d concursos criada com sucesso em '%s'.\n", quantidade, arquivoFinal);
        printf("Você pode agora rodar o extrator ETL para converter este arquivo para CSV.\n");
    } else {
        printf("Erro na alocação da memória para divisão do arquivo.\n");
    }

    return 0;
}
