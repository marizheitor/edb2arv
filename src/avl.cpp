#include "avl.hpp"
#include <iostream>
#include <algorithm>
#include <climits>
#include <cstdlib>

AVL::~AVL() { destruir(raiz); }

void AVL::destruir(No* no) {
    if (!no) return;
    destruir(no->esq);
    destruir(no->dir);
    delete no;
}

void AVL::atualizarAltura(No* no) {
    no->altura = 1 + std::max(h(no->esq), h(no->dir));
}

AVL::No* AVL::rotacaoDireita(No* y) {
    No* x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    atualizarAltura(y);      
    atualizarAltura(x);
    rotacoes++;
    return x;             
}

AVL::No* AVL::rotacaoEsquerda(No* x) {
    No* y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    atualizarAltura(x);
    atualizarAltura(y);
    rotacoes++;
    return y;
}


AVL::No* AVL::balancear(No* no) {
    atualizarAltura(no);
    int f = fb(no);

    if (f > 1) {                                 
        if (fb(no->esq) < 0)                      
            no->esq = rotacaoEsquerda(no->esq);
        return rotacaoDireita(no);                
    }
    if (f < -1) {                                 
        if (fb(no->dir) > 0)                     
            no->dir = rotacaoDireita(no->dir);
        return rotacaoEsquerda(no);              
    }
    return no;                                
}


bool AVL::inserir(int id) {
    bool inseriu = false;
    raiz = inserir(raiz, id, inseriu);
    if (inseriu) n++;
    return inseriu;
}

AVL::No* AVL::inserir(No* no, int id, bool& inseriu) {
    if (!no) { inseriu = true; return new No(id); }

    if (id < no->chave)      no->esq = inserir(no->esq, id, inseriu);
    else if (id > no->chave) no->dir = inserir(no->dir, id, inseriu);
    else return no;                               

    return balancear(no);                       
}



bool AVL::remover(int id) {
    bool removeu = false;
    raiz = remover(raiz, id, removeu);
    if (removeu) n--;
    return removeu;
}

AVL::No* AVL::remover(No* no, int id, bool& removeu) {
    if (!no) return nullptr;                     

    if (id < no->chave)      no->esq = remover(no->esq, id, removeu);
    else if (id > no->chave) no->dir = remover(no->dir, id, removeu);
    else {
        removeu = true;
        if (!no->esq || !no->dir) {               
            No* filho = no->esq ? no->esq : no->dir;
            delete no;
            return filho;                         
        }
        No* suc = no->dir;
        while (suc->esq) suc = suc->esq;
        no->chave = suc->chave;
        no->dir = remover(no->dir, suc->chave, removeu);
    }
    return balancear(no);
}

bool AVL::buscar(int id) const {
    const No* atual = raiz;
    while (atual) {
        if (id == atual->chave) return true;
        atual = (id < atual->chave) ? atual->esq : atual->dir;
    }
    return false;
}


void AVL::emOrdem() const {
    if (!raiz) { std::cout << "(árvore vazia)\n"; return; }
    emOrdem(raiz);
    std::cout << '\n';
}

void AVL::emOrdem(const No* no) const {
    if (!no) return;
    emOrdem(no->esq);
    std::cout << no->chave << ' ';
    emOrdem(no->dir);
}

void AVL::imprimir() const {
    if (!raiz) { std::cout << "(árvore vazia)\n"; return; }
    std::cout << raiz->chave << " (fb=" << fb(raiz) << ")\n";
    imprimirFilhos(raiz, "");
}

void AVL::imprimirFilhos(const No* no, const std::string& prefixo) const {
    if (!no->esq && !no->dir) return;             // folha: nada a mostrar
    imprimirRamo(no->esq, prefixo, false, "E: ");
    imprimirRamo(no->dir, prefixo, true,  "D: ");
}

void AVL::imprimirRamo(const No* no, const std::string& prefixo,
                       bool ultimo, const char* lado) const {
    std::cout << prefixo << (ultimo ? "└── " : "├── ") << lado;
    if (!no) { std::cout << "-\n"; return; }
    std::cout << no->chave << " (fb=" << fb(no) << ")\n";
    imprimirFilhos(no, prefixo + (ultimo ? "    " : "│   "));
}

bool AVL::verificarPropriedades() const {
    int alt;
    return verificar(raiz, LLONG_MIN, LLONG_MAX, alt);
}

bool AVL::verificar(const No* no, long long min, long long max, int& alt) const {
    if (!no) { alt = 0; return true; }
    if (no->chave <= min || no->chave >= max) return false;

    int he, hd;
    if (!verificar(no->esq, min, no->chave, he)) return false;
    if (!verificar(no->dir, no->chave, max, hd)) return false;

    alt = 1 + std::max(he, hd);
    return no->altura == alt && std::abs(he - hd) <= 1;
}


std::vector<int> AVL::emOrdemVetor() const {
    std::vector<int> v;
    v.reserve(n);
    coletar(raiz, v);
    return v;
}

void AVL::coletar(const No* no, std::vector<int>& v) const {
    if (!no) return;
    coletar(no->esq, v);
    v.push_back(no->chave);
    coletar(no->dir, v);
}

int AVL::filhos(int id) const {
    const No* atual = raiz;
    while (atual) {
        if (id == atual->chave)
            return (atual->esq ? 1 : 0) + (atual->dir ? 1 : 0);
        atual = (id < atual->chave) ? atual->esq : atual->dir;
    }
    return -1;
}


int AVL::custoBusca(int id) const {
    int visitados = 0;
    const No* atual = raiz;
    while (atual) {
        visitados++;
        if (id == atual->chave) break;
        atual = (id < atual->chave) ? atual->esq : atual->dir;
    }
    return visitados;
}
