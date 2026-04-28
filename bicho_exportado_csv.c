#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define LINE_LENGTH 256

// Estrutura para refletir a extração de dados
typedef struct {
    int milhar;
    int dezena;
    int bicho;
} PremioDecodificado;

typedef struct {
    int concurso;
    int dia, mes, ano;
    PremioDecodificado premios[5]; // Array fixo para os 5 prêmios [cite: 46]
} SorteioBicho;

// Função matemática O(1) para obter o grupo (bicho) a partir da dezena [cite: 57, 58, 61]
int obterGrupo(int dezena) {
    if (dezena == 0) return 25; // A dezena 00 (Vaca) é o grupo 25
    return ((dezena - 1) / 4) + 1;
}

// Processa o valor bruto do milhar e preenche a struct decodificada [cite: 50]
void decodificarPremio(int milharBruto, PremioDecodificado* premio) {
    premio->milhar = milharBruto;
    premio->dezena = milharBruto % 100; // Extração via módulo
    premio->bicho = obterGrupo(premio->dezena);
}

// Função principal de conversão e exportação (Stream Processing)
void exportarParaCSV(const char* arquivoEntrada, const char* arquivoSaida) {
    FILE* entrada = fopen(arquivoEntrada, "r");
    if (!entrada) {
        perror("Erro ao abrir arquivo de entrada");
        return;
    }

    FILE* saida = fopen(arquivoSaida, "w");
    if (!saida) {
        perror("Erro ao criar arquivo CSV");
        fclose(entrada);
        return;
    }

    // Escreve o cabeçalho do CSV (Separado por ';' para o padrão PT-BR do Excel)
    fprintf(saida, "Data;Concurso;");
    for (int i = 1; i <= 5; i++) {
        fprintf(saida, "P%d_Milhar;P%d_Dezena;P%d_Bicho", i, i, i);
        if (i < 5) fprintf(saida, ";");
    }
    fprintf(saida, "\n");

    char linha[LINE_LENGTH];
    int contador = 0;

    // Leitura sequencial (maximiza o uso de Cache L1/L2) [cite: 17]
    while (fgets(linha, sizeof(linha), entrada)) {
        SorteioBicho sorteio;
        int milharesBrutos[5];

        // ASSUMPTION: O arquivo TXT de entrada segue o formato: Data Concurso P1 P2 P3 P4 P5
        // Exemplo: 15/05/2024 1234 8492 1024 3333 0001 9999
        int lidos = sscanf(linha, "%d/%d/%d %d %d %d %d %d %d",
                           &sorteio.dia, &sorteio.mes, &sorteio.ano,
                           &sorteio.concurso,
                           &milharesBrutos[0], &milharesBrutos[1], &milharesBrutos[2],
                           &milharesBrutos[3], &milharesBrutos[4]);

        if (lidos == 9) {
            // Achatamento e decodificação na mesma passagem [cite: 49, 50]
            for (int i = 0; i < 5; i++) {
                decodificarPremio(milharesBrutos[i], &sorteio.premios[i]);
            }

            // Gravação no CSV
            fprintf(saida, "%02d/%02d/%04d;%d;", sorteio.dia, sorteio.mes, sorteio.ano, sorteio.concurso);
            for (int i = 0; i < 5; i++) {
                fprintf(saida, "%04d;%02d;%02d",
                        sorteio.premios[i].milhar,
                        sorteio.premios[i].dezena,
                        sorteio.premios[i].bicho);
                if (i < 5) fprintf(saida, ";");
            }
            fprintf(saida, "\n");
            contador++;
        } else {
            fprintf(stderr, "Aviso: Linha mal formatada ignorada: %s", linha);
        }
    }

    fclose(entrada);
    fclose(saida);
    printf("Exportação concluída! %d sorteios processados e salvos em '%s'.\n", contador, arquivoSaida);
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    // Para testar, certifique-se de criar um 'bicho_historico.txt' no mesmo diretório
    // contendo linhas no formato: DD/MM/AAAA Concurso P1 P2 P3 P4 P5
    const char* arquivoEntrada = "bicho_historico.txt";
    const char* arquivoSaida = "bicho_exportado.csv";

    printf("Iniciando processamento de dados do Jogo do Bicho...\n");
    exportarParaCSV(arquivoEntrada, arquivoSaida);

    return 0;
}
