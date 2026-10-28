#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

#include "avl.hpp"
#include "rubronegra.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using namespace std;

// Habilita UTF-8 e cores ANSI no console do Windows (no Linux não faz nada)
static void configurarConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    if (GetConsoleMode(saida, &modo))
        SetConsoleMode(saida, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

static string lerLinha(const string& mensagem) {
    cout << mensagem;
    string linha;
    if (!getline(cin, linha)) {
        cout << "\nEntrada encerrada. Até logo!\n";
        exit(0);
    }
    return linha;
}


static bool paraInteiro(const string& texto, int& valor) {
    stringstream ss(texto);
    char sobra;
    return (ss >> valor) && !(ss >> sobra);
}


static bool lerId(int& id) {
    if (!paraInteiro(lerLinha("ID da encomenda: "), id) || id <= 0) {
        cout << "ID inválido: digite um número inteiro positivo.\n";
        return false;
    }
    return true;
}



static bool ehRubroNegra(const ArvoreBalanceada& arv) {
    return dynamic_cast<const RubroNegra*>(&arv) != nullptr;
}


static void mostrarCusto(const ArvoreBalanceada& arv, int rotAntes, int ajAntes) {
    cout << "   Rebalanceamento: " << arv.getRotacoes() - rotAntes << " rotação(ões)";
    if (ehRubroNegra(arv))
        cout << ", " << arv.getAjustes() - ajAntes << " recoloração(ões)";
    cout << " | Altura: " << arv.altura()
         << " | Propriedades: " << (arv.verificarPropriedades() ? "OK" : "VIOLADAS!") << '\n';
}



static void opRegistrar(ArvoreBalanceada& arv) {
    int id;
    if (!lerId(id)) return;
    int rot = arv.getRotacoes(), aj = arv.getAjustes();
    if (arv.inserir(id)) {
        cout << "Encomenda " << id << " registrada.\n";
        mostrarCusto(arv, rot, aj);
    } else {
        cout << "Encomenda " << id << " já estava registrada: ID repetido ignorado.\n";
    }
}


static void opRegistrarLote(ArvoreBalanceada& arv) {
    stringstream ss(lerLinha("IDs separados por espaço: "));
    string token;
    int registrados = 0, repetidos = 0, invalidos = 0;
    int rot = arv.getRotacoes(), aj = arv.getAjustes();

    while (ss >> token) {
        int id;
        if (!paraInteiro(token, id) || id <= 0) { invalidos++; continue; }
        if (arv.inserir(id)) registrados++;
        else                 repetidos++;
    }
    cout << registrados << " registrada(s), " << repetidos << " repetida(s) ignorada(s)";
    if (invalidos) cout << ", " << invalidos << " valor(es) inválido(s) descartado(s)";
    cout << ".\n";
    if (registrados) mostrarCusto(arv, rot, aj);
}

static void opBuscar(const ArvoreBalanceada& arv) {
    if (arv.tamanho() == 0) {
        cout << "Nenhuma encomenda registrada (árvore vazia).\n";
        return;
    }
    int id;
    if (!lerId(id)) return;
    if (arv.buscar(id)) cout << "Encomenda " << id << " ENCONTRADA no sistema.\n";
    else                cout << "Encomenda " << id << " NÃO encontrada.\n";
}

static void opRemover(ArvoreBalanceada& arv) {
    if (arv.tamanho() == 0) {
        cout << "Nenhuma encomenda para remover (árvore vazia).\n";
        return;
    }
    int id;
    if (!lerId(id)) return;
    int rot = arv.getRotacoes(), aj = arv.getAjustes();
    if (arv.remover(id)) {
        cout << "Encomenda " << id << " removida (despachada).\n";
        mostrarCusto(arv, rot, aj);
    } else {
        cout << "Encomenda " << id << " não existe: nada foi removido.\n";
    }
}

static void opListar(const ArvoreBalanceada& arv) {
    cout << "Encomendas em ordem crescente (" << arv.tamanho() << "): ";
    arv.emOrdem();
}

static void opVisualizar(const ArvoreBalanceada& arv) {
    if (ehRubroNegra(arv)) cout << "Legenda: (V) = vermelho, (P) = preto | E = esquerda, D = direita\n";
    else                   cout << "Legenda: fb = altura(esq) - altura(dir) | E = esquerda, D = direita\n";
    arv.imprimir();
}

static void opEstatisticas(const ArvoreBalanceada& arv) {
    cout << "Estrutura .............. " << arv.nome() << '\n'
         << "Encomendas registradas . " << arv.tamanho() << '\n'
         << "Altura ................. " << arv.altura() << '\n'
         << "Rotações acumuladas .... " << arv.getRotacoes() << '\n';
    if (ehRubroNegra(arv))
        cout << "Recolorações acumuladas  " << arv.getAjustes() << '\n';
    cout << "Propriedades ........... "
         << (arv.verificarPropriedades() ? "OK" : "VIOLADAS!") << '\n';
}


static unique_ptr<ArvoreBalanceada> escolherArvore() {
    while (true) {
        cout << "\nQual estrutura deseja utilizar?\n"
             << "  1 - Árvore AVL\n"
             << "  2 - Árvore Rubro-Negra (LLRB)\n";
        int op;
        if (paraInteiro(lerLinha("Opção: "), op)) {
            if (op == 1) return make_unique<AVL>();
            if (op == 2) return make_unique<RubroNegra>();
        }
        cout << "Opção inválida.\n";
    }
}

static void mostrarMenu(const ArvoreBalanceada& arv) {
    cout << "\n==================================================\n"
         << "  AMAZON LOGISTICS - Registro de Encomendas\n"
         << "  Estrutura: " << arv.nome() << "  |  Encomendas: " << arv.tamanho() << '\n'
         << "==================================================\n"
         << "  1 - Registrar encomenda\n"
         << "  2 - Registrar lote de encomendas\n"
         << "  3 - Buscar encomenda\n"
         << "  4 - Remover encomenda\n"
         << "  5 - Listar encomendas (em ordem crescente)\n"
         << "  6 - Visualizar estrutura da árvore\n"
         << "  7 - Estatísticas e verificação das propriedades\n"
         << "  8 - Trocar estrutura (reinicia os dados)\n"
         << "  0 - Sair\n";
}

int main() {
    configurarConsole();
    cout << "Black Friday - Protótipo de registro de encomendas\n";
    unique_ptr<ArvoreBalanceada> arv = escolherArvore();

    while (true) {
        mostrarMenu(*arv);
        int op;
        if (!paraInteiro(lerLinha("Opção: "), op)) op = -1;
        cout << '\n';

        switch (op) {
            case 1: opRegistrar(*arv);     break;
            case 2: opRegistrarLote(*arv); break;
            case 3: opBuscar(*arv);        break;
            case 4: opRemover(*arv);       break;
            case 5: opListar(*arv);        break;
            case 6: opVisualizar(*arv);    break;
            case 7: opEstatisticas(*arv);  break;
            case 8:
                arv = escolherArvore();
                cout << "Nova estrutura: " << arv->nome() << " (vazia).\n";
                break;
            case 0:
                cout << "Encerrando. Até logo!\n";
                return 0;
            default:
                cout << "Opção inválida.\n";
        }
    }
}
