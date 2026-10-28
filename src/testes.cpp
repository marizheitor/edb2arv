/*
 * Bateria de testes - AVL x Rubro-Negra (LLRB)
 * IMD0039 - Estruturas de Dados Básicas II
 *
 * Parte 1: testes funcionais. Os mesmos casos rodam nas duas árvores.
 * Parte 2: exemplos de rotação e recoloração (usados nos slides).
 * Parte 3: análise comparativa. Mesmos dados e mesma sequência de operações
 *          nas duas árvores (e numa BST comum, só para comparar a altura).
 *          As propriedades são verificadas durante todo o processo e os
 *          números são salvos em resultados.csv.
 * Parte 4: tempo de execução (média de 5 execuções) e custo médio de busca
 *          com N = 100 mil e 1 milhão. Salvos em tempos.csv.
 *
 * Uso: testes                (roda tudo)
 *      testes --sem-tempo    (pula a Parte 4, que é a mais demorada)
 *      testes --so-tempo     (roda só a Parte 4)
 *
 * O programa retorna 0 se todas as checagens passarem e 1 caso contrário.
 */

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

#include "avl.hpp"
#include "rubronegra.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static void configurarConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    if (GetConsoleMode(saida, &modo))
        SetConsoleMode(saida, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

// ======================================================================
// Infraestrutura
// ======================================================================

static int totalChecagens = 0;
static int totalFalhas = 0;

// Registra uma checagem e imprime [OK] ou [FALHOU]
static void checar(bool condicao, const string& descricao) {
    totalChecagens++;
    if (!condicao) totalFalhas++;
    cout << "      [" << (condicao ? "OK" : "FALHOU") << "] " << descricao << '\n';
}

// tipo 0 = AVL, tipo 1 = Rubro-Negra (LLRB)
static unique_ptr<ArvoreBalanceada> novaArvore(int tipo) {
    if (tipo == 0) return make_unique<AVL>();
    return make_unique<RubroNegra>();
}

// Percurso em ordem: estritamente crescente e com a quantidade certa de elementos
static bool percursoCorreto(const ArvoreBalanceada& a) {
    vector<int> v = a.emOrdemVetor();
    return (int)v.size() == a.tamanho()
        && adjacent_find(v.begin(), v.end(), [](int x, int y) { return x >= y; }) == v.end();
}

// Propriedades de balanceamento + ordem de BST
static bool estadoValido(const ArvoreBalanceada& a) {
    return a.verificarPropriedades() && percursoCorreto(a);
}

// Embaralhamento de Fisher-Yates com semente fixa (reprodutível em qualquer compilador)
static void embaralhar(vector<int>& v, unsigned semente) {
    mt19937 rng(semente);
    for (size_t i = v.size(); i > 1; i--)
        swap(v[i - 1], v[rng() % i]);
}

// Largura de uma string UTF-8 em caracteres (para alinhar tabelas com acentos)
static int larguraTexto(const string& s) {
    int n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) n++;
    return n;
}

static string ajustar(const string& s, int largura, bool direita = true) {
    string espacos(max(0, largura - larguraTexto(s)), ' ');
    return direita ? espacos + s : s + espacos;
}

static void titulo(const string& t) {
    cout << '\n' << string(72, '=') << "\n  " << t << '\n' << string(72, '=') << '\n';
}

// ======================================================================
// Parte 1 - Testes funcionais
// ======================================================================

static void casoVazia(ArvoreBalanceada& a) {
    checar(a.tamanho() == 0 && a.altura() == 0, "árvore nova: tamanho 0 e altura 0");
    checar(!a.buscar(100), "busca em árvore vazia: não encontrado");
    checar(!a.remover(100), "remoção em árvore vazia: recusada");
    checar(a.emOrdemVetor().empty(), "percurso em ordem sem elementos");
    checar(a.verificarPropriedades(), "propriedades válidas na árvore vazia");
}

static void casoUmElemento(ArvoreBalanceada& a) {
    checar(a.inserir(42), "inserir 42 na árvore vazia");
    checar(a.tamanho() == 1 && a.altura() == 1, "tamanho 1 e altura 1");
    checar(a.filhos(42) == 0, "o único nó é uma folha");
    checar(a.buscar(42), "busca de 42: encontrado");
    checar(!a.buscar(7), "busca de 7 (inexistente): não encontrado");
    checar(!a.inserir(42), "inserir 42 de novo: recusado (repetido)");
    checar(!a.remover(7), "remover 7 (inexistente): recusado");
    checar(a.tamanho() == 1 && estadoValido(a), "árvore intacta após as recusas");
    checar(a.remover(42), "remover 42 (único elemento)");
    checar(a.tamanho() == 0 && a.altura() == 0 && estadoValido(a), "árvore voltou a ficar vazia e válida");
}

static void casoBusca(ArvoreBalanceada& a) {
    const vector<int> ids = {50, 30, 70, 20, 40, 60, 80, 35, 65};
    for (int id : ids) a.inserir(id);
    checar(all_of(ids.begin(), ids.end(), [&](int id) { return a.buscar(id); }),
           "os 9 IDs inseridos são encontrados");

    const vector<int> ausentes = {0, 10, 25, 45, 55, 75, 100};
    checar(none_of(ausentes.begin(), ausentes.end(), [&](int id) { return a.buscar(id); }),
           "IDs inexistentes (0, 10, 25, 45, 55, 75, 100) não são encontrados");
    checar(!a.inserir(40) && a.tamanho() == 9, "inserir 40 (repetido): recusado, tamanho continua 9");
    checar(estadoValido(a), "percurso em ordem crescente e propriedades válidas");
}

// Confere a configuração do nó, remove e valida a árvore
static void removerComConfiguracao(ArvoreBalanceada& a, int id, int filhosEsperados) {
    string sid = to_string(id);
    checar(a.filhos(id) == filhosEsperados,
           "antes: o nó " + sid + " tem " + to_string(filhosEsperados) + " filho(s)");
    int tamanho = a.tamanho();
    int rot = a.getRotacoes();
    checar(a.remover(id) && !a.buscar(id) && a.tamanho() == tamanho - 1,
           "remover " + sid + ": removido, não é mais encontrado e o tamanho caiu 1");
    checar(estadoValido(a), "propriedades válidas após a remoção ("
           + to_string(a.getRotacoes() - rot) + " rotação(ões))");
}

// Árvore {20, 10, 30, 5}: nas duas estruturas, 5 é folha, 10 tem 1 filho e 20 tem 2 filhos
static void prepararPequena(ArvoreBalanceada& a) {
    for (int id : {20, 10, 30, 5}) a.inserir(id);
}

static void casoRemoverFolha(ArvoreBalanceada& a)      { prepararPequena(a); removerComConfiguracao(a, 5, 0); }
static void casoRemoverUmFilho(ArvoreBalanceada& a)    { prepararPequena(a); removerComConfiguracao(a, 10, 1); }
static void casoRemoverDoisFilhos(ArvoreBalanceada& a) { prepararPequena(a); removerComConfiguracao(a, 20, 2); }

// Procura na árvore atual um ID cujo nó tenha exatamente k filhos
static int acharNoCom(const ArvoreBalanceada& a, int k) {
    for (int id : a.emOrdemVetor())
        if (a.filhos(id) == k) return id;
    return -1;
}

// Remoções numa árvore maior, com rebalanceamento envolvido
static void casoRemocoesMaiores(ArvoreBalanceada& a) {
    vector<int> ids(40);
    iota(ids.begin(), ids.end(), 1);
    embaralhar(ids, 7);
    for (int id : ids) a.inserir(id);
    cout << "      (40 IDs inseridos em ordem embaralhada, altura " << a.altura() << ")\n";

    for (int k = 2; k >= 0; k--) {
        int id = acharNoCom(a, k);
        if (id != -1) removerComConfiguracao(a, id, k);
        else cout << "      (nenhum nó com " << k << " filho(s) nesta árvore)\n";
    }

    int antes = a.tamanho();
    checar(!a.remover(999) && a.tamanho() == antes && estadoValido(a),
           "remover 999 (inexistente): recusado, nada muda");

    bool sempreValida = true;
    for (int id : ids) {
        a.remover(id);
        sempreValida = sempreValida && estadoValido(a);
    }
    checar(sempreValida && a.tamanho() == 0,
           "esvaziar removendo um a um: propriedades válidas em todos os passos");
}

using Caso = void (*)(ArvoreBalanceada&);

// Roda o mesmo caso numa AVL nova e numa LLRB nova
static void rodarCaso(const string& nome, Caso caso) {
    cout << "\n  > " << nome << '\n';
    for (int tipo = 0; tipo < 2; tipo++) {
        unique_ptr<ArvoreBalanceada> a = novaArvore(tipo);
        cout << "    [" << a->nome() << "]\n";
        caso(*a);
    }
}

static void parte1() {
    titulo("PARTE 1 - Testes funcionais (mesmos casos nas duas árvores)");
    rodarCaso("Operações em árvore vazia", casoVazia);
    rodarCaso("Árvore com apenas um elemento", casoUmElemento);
    rodarCaso("Busca de elementos existentes e inexistentes", casoBusca);
    rodarCaso("Remoção de folha (0 filhos)", casoRemoverFolha);
    rodarCaso("Remoção de nó com 1 filho", casoRemoverUmFilho);
    rodarCaso("Remoção de nó com 2 filhos", casoRemoverDoisFilhos);
    rodarCaso("Remoções numa árvore maior + remoção de inexistente", casoRemocoesMaiores);
}

// ======================================================================
// Parte 2 - Exemplos de balanceamento (para os slides)
// ======================================================================

static void parte2() {
    titulo("PARTE 2 - Exemplos de balanceamento (para os slides)");

    cout << "\n  AVL - rotação simples à direita (caso Esquerda-Esquerda)\n";
    {
        AVL a;
        a.inserir(30); a.inserir(20);
        cout << "  Antes de inserir 10:\n";
        a.imprimir();
        a.inserir(10);
        cout << "  Depois de inserir 10 (" << a.getRotacoes() << " rotação):\n";
        a.imprimir();
    }

    cout << "\n  AVL - rotação dupla (caso Esquerda-Direita)\n";
    {
        AVL a;
        a.inserir(30); a.inserir(10);
        cout << "  Antes de inserir 20:\n";
        a.imprimir();
        a.inserir(20);
        cout << "  Depois de inserir 20 (" << a.getRotacoes() << " rotações):\n";
        a.imprimir();
    }

    cout << "\n  LLRB - rotação à esquerda e inversão de cores\n";
    {
        RubroNegra r;
        r.inserir(10);
        cout << "  Apenas 10:\n";
        r.imprimir();
        r.inserir(20);
        cout << "  Depois de inserir 20 (vermelho à direita -> rotação à esquerda):\n";
        r.imprimir();
        r.inserir(30);
        cout << "  Depois de inserir 30 (dois filhos vermelhos -> inversão de cores):\n";
        r.imprimir();
        cout << "  Total: " << r.getRotacoes() << " rotação, "
             << r.getAjustes() << " recoloração\n";
    }
}

// ======================================================================
// Parte 3 - Análise comparativa
// ======================================================================

// BST comum (sem balanceamento), usada só para mostrar a degeneração.
// Inserção e altura são iterativas para não estourar a pilha quando ela vira "lista".
class BSTSimples {
    struct No {
        int chave;
        No* esq = nullptr;
        No* dir = nullptr;
        explicit No(int k) : chave(k) {}
    };
    No* raiz = nullptr;

public:
    BSTSimples() = default;
    BSTSimples(const BSTSimples&) = delete;
    BSTSimples& operator=(const BSTSimples&) = delete;

    ~BSTSimples() {
        vector<No*> pilha;
        if (raiz) pilha.push_back(raiz);
        while (!pilha.empty()) {
            No* no = pilha.back();
            pilha.pop_back();
            if (no->esq) pilha.push_back(no->esq);
            if (no->dir) pilha.push_back(no->dir);
            delete no;
        }
    }

    void inserir(int k) {
        No** p = &raiz;
        while (*p) {
            if (k < (*p)->chave)      p = &(*p)->esq;
            else if (k > (*p)->chave) p = &(*p)->dir;
            else return;
        }
        *p = new No(k);
    }

    int altura() const {
        int h = 0;
        vector<No*> nivel;
        if (raiz) nivel.push_back(raiz);
        while (!nivel.empty()) {
            h++;
            vector<No*> proximo;
            for (No* no : nivel) {
                if (no->esq) proximo.push_back(no->esq);
                if (no->dir) proximo.push_back(no->dir);
            }
            nivel.swap(proximo);
        }
        return h;
    }
};

static const int ID_BASE = 100000;   // IDs de encomenda: 100001, 100002, ...

static vector<int> gerarIds(const string& cenario, int n) {
    vector<int> ids(n);
    iota(ids.begin(), ids.end(), ID_BASE + 1);
    if (cenario == "decrescente")    reverse(ids.begin(), ids.end());
    else if (cenario == "aleatória") embaralhar(ids, 2026);
    return ids;
}

struct Medida {
    int altura = 0;
    int rotacoes = 0;
    int recoloracoes = 0;
};

struct Linha {
    string cenario;
    int n = 0;
    int alturaBST = 0;
    Medida ins[2];   // fase de inserção: [0] = AVL, [1] = LLRB
    Medida rem[2];   // fase de remoção
};

// Mesmos dados e mesma sequência de operações para as duas árvores:
//  1) insere as N encomendas na ordem do cenário
//  2) busca as N existentes e 2N inexistentes
//  3) remove as N/2 primeiras que chegaram (despacho na ordem de chegada)
static Linha rodarCenario(const string& cenario, int n) {
    Linha L;
    L.cenario = cenario;
    L.n = n;

    const vector<int> ids = gerarIds(cenario, n);
    const vector<int> removidos(ids.begin(), ids.begin() + n / 2);
    const int passo = (n <= 1000) ? 1 : 100;       // frequência da verificação completa
    const string quando = (passo == 1) ? "após cada operação" : "a cada 100 operações";

    BSTSimples bst;
    for (int id : ids) bst.inserir(id);
    L.alturaBST = bst.altura();

    cout << "\n  > Cenário " << cenario << ", N = " << n
         << "  (BST comum ficaria com altura " << L.alturaBST << ")\n";

    for (int tipo = 0; tipo < 2; tipo++) {
        unique_ptr<ArvoreBalanceada> a = novaArvore(tipo);
        const string nome = a->nome();
        cout << "    [" << nome << "]\n";

        // 1) Inserções
        bool valida = true;
        for (int i = 0; i < n; i++) {
            a->inserir(ids[i]);
            if ((i + 1) % passo == 0 || i == n - 1) valida = valida && estadoValido(*a);
        }
        checar(valida && a->tamanho() == n,
               to_string(n) + " inserções; propriedades válidas " + quando);
        L.ins[tipo] = {a->altura(), a->getRotacoes(), a->getAjustes()};

        // 2) Buscas
        bool achouTodos = all_of(ids.begin(), ids.end(), [&](int id) { return a->buscar(id); });
        bool nenhumFalso = true;
        for (int i = 1; i <= n; i++)
            nenhumFalso = nenhumFalso && !a->buscar(ID_BASE + n + i) && !a->buscar(ID_BASE + 1 - i);
        checar(achouTodos && nenhumFalso,
               to_string(n) + " buscas de existentes acertaram e "
               + to_string(2 * n) + " de inexistentes retornaram 'não encontrado'");

        // 3) Remoções
        a->zerarContadores();
        valida = true;
        for (size_t i = 0; i < removidos.size(); i++) {
            a->remover(removidos[i]);
            if ((i + 1) % passo == 0 || i + 1 == removidos.size()) valida = valida && estadoValido(*a);
        }
        bool sumiram = none_of(removidos.begin(), removidos.end(), [&](int id) { return a->buscar(id); });
        bool ficaram = all_of(ids.begin() + n / 2, ids.end(), [&](int id) { return a->buscar(id); });
        checar(valida && sumiram && ficaram && a->tamanho() == n - n / 2,
               to_string(n / 2) + " remoções; propriedades válidas " + quando
               + "; removidos sumiram e o resto continua");
        L.rem[tipo] = {a->altura(), a->getRotacoes(), a->getAjustes()};
    }
    return L;
}

static void imprimirTabelas(const vector<Linha>& linhas) {
    titulo("RESULTADOS - fase de inserção (N encomendas)");
    cout << ajustar("Cenário", 12, false) << ajustar("N", 7) << ajustar("Alt.BST", 9)
         << ajustar("Alt.AVL", 9) << ajustar("Alt.LLRB", 10) << ajustar("Rot.AVL", 9)
         << ajustar("Rot.LLRB", 10) << ajustar("Recol.LLRB", 12) << '\n'
         << string(78, '-') << '\n';
    for (const Linha& L : linhas)
        cout << ajustar(L.cenario, 12, false) << ajustar(to_string(L.n), 7)
             << ajustar(to_string(L.alturaBST), 9)
             << ajustar(to_string(L.ins[0].altura), 9) << ajustar(to_string(L.ins[1].altura), 10)
             << ajustar(to_string(L.ins[0].rotacoes), 9) << ajustar(to_string(L.ins[1].rotacoes), 10)
             << ajustar(to_string(L.ins[1].recoloracoes), 12) << '\n';

    titulo("RESULTADOS - fase de remoção (N/2 primeiras encomendas despachadas)");
    cout << ajustar("Cenário", 12, false) << ajustar("N", 7)
         << ajustar("Alt.AVL", 9) << ajustar("Alt.LLRB", 10) << ajustar("Rot.AVL", 9)
         << ajustar("Rot.LLRB", 10) << ajustar("Recol.LLRB", 12) << '\n'
         << string(69, '-') << '\n';
    for (const Linha& L : linhas)
        cout << ajustar(L.cenario, 12, false) << ajustar(to_string(L.n), 7)
             << ajustar(to_string(L.rem[0].altura), 9) << ajustar(to_string(L.rem[1].altura), 10)
             << ajustar(to_string(L.rem[0].rotacoes), 9) << ajustar(to_string(L.rem[1].rotacoes), 10)
             << ajustar(to_string(L.rem[1].recoloracoes), 12) << '\n';

    titulo("Referência teórica de altura (em níveis)");
    cout << ajustar("N", 7) << ajustar("Mínima", 9) << ajustar("Máx. AVL", 11)
         << ajustar("Máx. RB", 10) << '\n' << string(37, '-') << '\n';
    vector<int> ns;
    for (const Linha& L : linhas)
        if (find(ns.begin(), ns.end(), L.n) == ns.end()) ns.push_back(L.n);
    for (int n : ns) {
        int minima = (int)ceil(log2(n + 1.0));
        int maxAvl = (int)floor(1.4405 * log2(n + 2.0) - 0.3277);   // h < 1,44·log2(n+2) - 0,33
        int maxRb  = (int)floor(2.0 * log2(n + 1.0));                // h <= 2·log2(n+1)
        cout << ajustar(to_string(n), 7) << ajustar(to_string(minima), 9)
             << ajustar(to_string(maxAvl), 11) << ajustar(to_string(maxRb), 10) << '\n';
    }
}

static void salvarCsv(const vector<Linha>& linhas, const string& arquivo) {
    ofstream out(arquivo);
    if (!out) {
        cout << "\n  Aviso: não foi possível criar " << arquivo << '\n';
        return;
    }
    out << "\xEF\xBB\xBF";   // BOM: faz o Excel reconhecer UTF-8
    out << "arvore,cenario,n,fase,altura,rotacoes,recoloracoes\n";
    const string nomes[2] = {"AVL", "LLRB"};
    for (const Linha& L : linhas) {
        out << "BST," << L.cenario << ',' << L.n << ",insercao," << L.alturaBST << ",0,0\n";
        for (int t = 0; t < 2; t++) {
            out << nomes[t] << ',' << L.cenario << ',' << L.n << ",insercao,"
                << L.ins[t].altura << ',' << L.ins[t].rotacoes << ',' << L.ins[t].recoloracoes << '\n';
            out << nomes[t] << ',' << L.cenario << ',' << L.n << ",remocao,"
                << L.rem[t].altura << ',' << L.rem[t].rotacoes << ',' << L.rem[t].recoloracoes << '\n';
        }
    }
    cout << "\n  Resultados salvos em " << arquivo << '\n';
}

static vector<Linha> parte3() {
    titulo("PARTE 3 - Análise comparativa (mesmos dados e mesma sequência)");
    cout << "  IDs de encomenda a partir de " << ID_BASE + 1
         << "; remoção = despacho das N/2 primeiras que chegaram\n";

    vector<Linha> linhas;
    for (const string cenario : {"crescente", "decrescente", "aleatória"})
        for (int n : {100, 1000, 10000})
            linhas.push_back(rodarCenario(cenario, n));
    return linhas;
}

// ======================================================================
// Parte 4 - Tempo de execução e custo de busca
// ======================================================================

using Relogio = chrono::steady_clock;

static double msDesde(Relogio::time_point inicio) {
    return chrono::duration<double, milli>(Relogio::now() - inicio).count();
}

static string formatar(double valor, int casas) {
    ostringstream ss;
    ss << fixed << setprecision(casas) << valor;
    return ss.str();
}

static const int REPETICOES = 5;

struct Tempos {
    double insercao = 0, busca = 0, remocao = 0;   // médias em ms
    double comparacoesMedias = 0;                  // nós visitados por busca bem-sucedida
    int altura = 0;
};

// Mede uma estrutura com os mesmos dados: média de REPETICOES execuções.
// Cada execução começa com uma árvore nova.
static Tempos medir(int tipo, const vector<int>& ids, const vector<int>& consultas, bool& buscasOk) {
    Tempos t;
    size_t encontrados = 0;    // usado também para o compilador não descartar as buscas

    for (int r = 0; r < REPETICOES; r++) {
        unique_ptr<ArvoreBalanceada> a = novaArvore(tipo);

        auto inicio = Relogio::now();
        for (int id : ids) a->inserir(id);
        t.insercao += msDesde(inicio);

        inicio = Relogio::now();
        for (int id : consultas) encontrados += a->buscar(id);
        t.busca += msDesde(inicio);

        if (r == 0) {   // o custo de busca não muda entre repetições: mede uma vez
            long long soma = 0;
            for (int id : ids) soma += a->custoBusca(id);
            t.comparacoesMedias = (double)soma / ids.size();
            t.altura = a->altura();
        }

        inicio = Relogio::now();
        for (size_t i = 0; i < ids.size() / 2; i++) a->remover(ids[i]);
        t.remocao += msDesde(inicio);
    }

    t.insercao /= REPETICOES;
    t.busca    /= REPETICOES;
    t.remocao  /= REPETICOES;
    buscasOk = buscasOk && encontrados == consultas.size() * REPETICOES;
    return t;
}

static void parte4() {
    titulo("PARTE 4 - Tempo de execução e custo de busca (média de 5 execuções)");
    cout << "  Tempos em milissegundos (dependem da máquina; compile com -O2).\n"
         << "  Inserção: N encomendas | Busca: N consultas a IDs existentes, embaralhadas\n"
         << "  Remoção: N/2 primeiras que chegaram | Comp./busca: nós visitados em média\n\n";

    cout << ajustar("Cenário", 12, false) << ajustar("N", 9) << "  " << ajustar("Árvore", 7, false)
         << ajustar("Inserção", 10) << ajustar("Busca", 9) << ajustar("Remoção", 10)
         << ajustar("Comp./busca", 13) << ajustar("Altura", 8) << '\n'
         << string(80, '-') << '\n';

    ofstream csv("tempos.csv");
    if (csv) csv << "\xEF\xBB\xBF" << "cenario,n,arvore,insercao_ms,busca_ms,remocao_ms,comparacoes_medias,altura\n";

    const string nomes[2] = {"AVL", "LLRB"};
    bool buscasOk = true;

    for (const string cenario : {"crescente", "decrescente", "aleatória"}) {
        for (int n : {100000, 1000000}) {
            const vector<int> ids = gerarIds(cenario, n);
            vector<int> consultas = ids;
            embaralhar(consultas, 99);           // mesma ordem de consultas para as duas

            for (int tipo = 0; tipo < 2; tipo++) {
                Tempos t = medir(tipo, ids, consultas, buscasOk);
                cout << ajustar(cenario, 12, false) << ajustar(to_string(n), 9) << "  "
                     << ajustar(nomes[tipo], 7, false)
                     << ajustar(formatar(t.insercao, 1), 10) << ajustar(formatar(t.busca, 1), 9)
                     << ajustar(formatar(t.remocao, 1), 10)
                     << ajustar(formatar(t.comparacoesMedias, 2), 13)
                     << ajustar(to_string(t.altura), 8) << '\n' << flush;
                if (csv)
                    csv << cenario << ',' << n << ',' << nomes[tipo] << ','
                        << formatar(t.insercao, 3) << ',' << formatar(t.busca, 3) << ','
                        << formatar(t.remocao, 3) << ',' << formatar(t.comparacoesMedias, 3) << ','
                        << t.altura << '\n';
            }
        }
    }
    cout << '\n';
    checar(buscasOk, "todas as buscas da medição de tempo encontraram os IDs");
    if (csv) cout << "\n  Resultados salvos em tempos.csv\n";
}

// ======================================================================

int main(int argc, char* argv[]) {
    configurarConsole();

    bool comTempo = true, soTempo = false;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--sem-tempo")     comTempo = false;
        else if (arg == "--so-tempo") soTempo = true;
        else {
            cout << "Uso: " << argv[0] << " [--sem-tempo | --so-tempo]\n";
            return 2;
        }
    }

    cout << "BATERIA DE TESTES - AVL x RUBRO-NEGRA (LLRB)\n";

    if (!soTempo) {
        parte1();
        parte2();
        vector<Linha> linhas = parte3();
        imprimirTabelas(linhas);
        salvarCsv(linhas, "resultados.csv");
    }
    if (comTempo) parte4();

    titulo("RESUMO");
    cout << "  " << totalChecagens << " checagens, " << totalFalhas << " falha(s)\n";
    return totalFalhas ? 1 : 0;
}
