// Trabalho TP2 - Netflix 
// Alunos: Pedro Henrique Corrêa Leme - 2342377
// Pedro Henrique Mendonça - 2350594
// Pedro Lucas Queiroz de Souza - 

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//Struct para armazenar na memoria principal com as categorias do arquivo
struct Netflix {
    string show_id;
    string type;
    string title;
    string director;
    string cast;
    string country;
    string date_added;
    string release_year;
    string rating;
    string duration;
    string listed_in;
    string description;
};

// Struct para armazenar a contagem de títulos por país
// Cada instância representa um país único encontrado no catálogo
struct PaisContagem {
    string nome;   // Nome do país (ex: "United States")
    int total;     // Total de títulos produzidos (filmes + séries)
    int filmes;    // Quantidade de filmes
    int series;    // Quantidade de séries
};

//Tarefa 1 - Leitura e armazenamento dos dados
int contarLinhas(string netflix_titles){
ifstream arquivo(netflix_titles);         //ifstream para leitura do arquivo
        if (!arquivo.is_open()) {
        cerr << "Erro ao abrir arquivo." << endl;
        return 0;
    }
    string linha;
    int total = -1; //Para nao ler as categorias (cabeçalho)

        while (getline(arquivo, linha)) {
        total++;
        }
    arquivo.close();
    return total;
}

//Tarefa 1 - Separação por colunas
void separarCSV(string linha, string campos[], int totalCampos) {
    bool dentroAspas = false;
    string valor = "";
    int coluna = 0;

    for (int i = 0; i < linha.length(); i++) {
        char c = linha[i];

        // Detecta aspas
        if (c == '"') {
            dentroAspas = !dentroAspas;
        }
        // Detecta separador válido
        else if (c == ',' && !dentroAspas) {
            campos[coluna] = valor;
            coluna++;
            valor = "";
        }

        else {
            valor += c;
        }
    }
    // Último campo
    campos[coluna] = valor;
}

//Tarefa 1 - Tratamento de dados em: Filmes ou Series.
void carregarDados(string netflix_titles, Netflix *catalogo, int total, int &filmes, int &series) {

    ifstream arquivo(netflix_titles);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir arquivo." << endl;
        return; 
    }
    string linha;

    int i = 0;
     while (getline(arquivo, linha) && i < total) {
    string campos[12];
    separarCSV(linha, campos, 12);

        catalogo[i].show_id = campos[0];
        catalogo[i].type = campos[1];
        catalogo[i].title = campos[2];
        catalogo[i].director = campos[3];
        catalogo[i].cast = campos[4];
        catalogo[i].country = campos[5];
        catalogo[i].date_added = campos[6];
        catalogo[i].release_year = campos[7];
        catalogo[i].rating = campos[8];
        catalogo[i].duration = campos[9];
        catalogo[i].listed_in = campos[10];
        catalogo[i].description = campos[11];
     
        // Conta filmes e séries
        if (catalogo[i].type == "Movie") {
            filmes++;
        }
        else if (catalogo[i].type == "TV Show") {
            series++;
        }
        i++;
    }

    arquivo.close();
}

// Extrai apenas o primeiro país do campo "country"
// O campo pode conter vários países separados por vírgula
string primeiroPais(string country) {
    // Se o campo estiver vazio, retorna "Unknown"
    if (country.empty()) return "Unknown";

    // Procura a posição da primeira vírgula
    int pos = country.find(',');

    // Se encontrou vírgula, retorna só o trecho antes dela
    if (pos != string::npos)
        return country.substr(0, pos);

    // Se não há vírgula, o campo tem só um país — retorna ele inteiro
    return country;
}

// Tarefa 2 — Ranking de Países Produtores
// Percorre o catálogo já carregado em memória,
// acumula contagens por país e exibe o Top 10
void rankingPaises(Netflix *catalogo, int total) {

    // Prepara o vetor dinâmico de países
    // Começa com capacidade para 10 países.
    // Se precisar de mais, o vetor será realocado (dobrado).
    int capacidade = 10;  // Capacidade atual do vetor
    int qtdPaises  = 0;   // Quantos países únicos já foram encontrados

    // Aloca dinamicamente o vetor de países (requisito do enunciado)
    PaisContagem *paises = new PaisContagem[capacidade];

    // Percorrer todos os registros do catálogo
    for (int i = 0; i < total; i++) {

        // Extrai o primeiro país do campo country do registro atual
        string pais = primeiroPais(catalogo[i].country);

        // Garante que campos vazios virem "Unknown"
        if (pais.empty()) pais = "Unknown";

        // Verificar se o país já está no vetor
        // Busca linear no vetor de países já cadastrados
        int idx = -1; // Índice do país no vetor (-1 = não encontrado)

        for (int j = 0; j < qtdPaises; j++) {
            if (paises[j].nome == pais) {
                idx = j; // País encontrado na posição j
                break;
            }
        }

        //Se o país NÃO existe ainda no vetor,
        // precisamos inseri-lo
        if (idx == -1) {

            // Verifica se o vetor está cheio
            // Se estiver, realoca com o dobro da capacidade
            if (qtdPaises == capacidade) {
                capacidade *= 2; // Dobra a capacidade

                // Cria um novo vetor maior
                PaisContagem *novo = new PaisContagem[capacidade];

                // Copia todos os países do vetor antigo para o novo
                for (int k = 0; k < qtdPaises; k++)
                    novo[k] = paises[k];

                // Libera o vetor antigo da memória
                delete[] paises;

                // Aponta para o novo vetor
                paises = novo;
            }

            // Insere o novo país no final do vetor
            // Inicializa todos os contadores em zero
            paises[qtdPaises].nome   = pais;
            paises[qtdPaises].total  = 0;
            paises[qtdPaises].filmes = 0;
            paises[qtdPaises].series = 0;

            // O índice do novo país é a última posição
            idx = qtdPaises;

            // Incrementa a quantidade de países únicos
            qtdPaises++;
        }

        // Incrementar os contadores do país
        // (seja ele recém-inserido ou já existente)
        paises[idx].total++; // Sempre incrementa o total

        if (catalogo[i].type == "Movie")        // Se for filme
            paises[idx].filmes++;
        else if (catalogo[i].type == "TV Show") // Se for série
            paises[idx].series++;
    }

    // Ordenar o vetor por total (decrescente)
    // A cada iteração, encontra o maior valor restante
    // e o coloca na posição correta
    for (int i = 0; i < qtdPaises - 1; i++) {

        // Assume que a posição i tem o maior valor
        int maxIdx = i;

        // Percorre os elementos seguintes procurando um maior
        for (int j = i + 1; j < qtdPaises; j++) {
            if (paises[j].total > paises[maxIdx].total)
                maxIdx = j; // Atualiza o índice do maior encontrado
        }

        // Se o maior não está na posição i, troca os dois
        if (maxIdx != i) {
            PaisContagem tmp  = paises[i];
            paises[i]         = paises[maxIdx];
            paises[maxIdx]    = tmp;
        }
    }

    // Exibir o Top 10
    // Se houver menos de 10 países, exibe todos
    cout << "\n=== Top 10 Paises Produtores ===" << endl;

    // Define o limite: 10 ou o total de países (o que for menor)
    int limite = (qtdPaises < 10) ? qtdPaises : 10;

    for (int i = 0; i < limite; i++) {
        // Imprime a posição no ranking
        cout << " " << i + 1 << ". ";

        // Imprime o nome do país
        string nome = paises[i].nome;
        cout << nome;

        // Adiciona espaços para alinhar as colunas (estilo tabela)
        // Todos os nomes ocupam pelo menos 16 caracteres
        for (int s = nome.length(); s < 16; s++) cout << ' ';

        // Imprime os contadores formatados
        cout << ": " << paises[i].total  << " titulos" << " (Filmes: " << paises[i].filmes << " | Series: " << paises[i].series << ")" << endl;
    }

    // Liberar a memória alocada dinamicamente
    // Sempre liberar o que foi alocado com new[]
    delete[] paises;
}

int main() {
    int totalRegistros = contarLinhas("netflix_titles.csv");
    // Alocação dinâmica
    Netflix *catalogo = new Netflix[totalRegistros];
    int filmes = 0;
    int series = 0;

    // Carrega os dados
    carregarDados("netflix_titles.csv", catalogo, totalRegistros, filmes, series);
cout << "Registros carregados: " << totalRegistros << endl;
cout << "Filmes: " << filmes << " | Series: " << series << endl;

rankingPaises(catalogo, totalRegistros); 

    return 0;
}