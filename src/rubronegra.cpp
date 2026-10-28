#include "rubronegra.hpp"
#include <iostream>
#include <algorithm>
#include <climits>

RubroNegra::~RubroNegra() { destruir(raiz); }

void RubroNegra::destruir(No* no) {
    if (!no) return;
    destruir(no->esq);
    destruir(no->dir);
    delete no;
}

int RubroNegra::alturaDe(const No* no) {
    if (!no) return 0;
    return 1 + std::max(alturaDe(no->esq), alturaDe(no->dir));
}

const RubroNegra::No* RubroNegra::minimo(const No* h) {
    while (h->esq) h = h->esq;
    return h;
}

RubroNegra::No* RubroNegra::rotacaoEsquerda(No* h) {
    No* x = h->dir;
    h->dir = x->esq;
    x->esq = h;
    x->cor = h->cor;         
    h->cor = VERMELHO;      
    rotacoes++;
    return x;
}

RubroNegra::No* RubroNegra::rotacaoDireita(No* h) {
    No* x = h->esq;
    h->esq = x->dir;
    x->dir = h;
    x->cor = h->cor;
    h->cor = VERMELHO;
    rotacoes++;
    return x;
}


void RubroNegra::inverterCores(No* h) {
    h->cor      = !h->cor;
    h->esq->cor = !h->esq->cor;
    h->dir->cor = !h->dir->cor;
    ajustes++;
}


RubroNegra::No* RubroNegra::balancear(No* h) {
    if (ehVermelho(h->dir) && !ehVermelho(h->esq))      // vermelho à direita
        h = rotacaoEsquerda(h);
    if (ehVermelho(h->esq) && ehVermelho(h->esq->esq))  // dois vermelhos seguidos
        h = rotacaoDireita(h);
    if (ehVermelho(h->esq) && ehVermelho(h->dir))       // dois filhos vermelhos
        inverterCores(h);
    return h;
}


RubroNegra::No* RubroNegra::moverVermelhoEsquerda(No* h) {
    inverterCores(h);
    if (ehVermelho(h->dir->esq)) {
        h->dir = rotacaoDireita(h->dir);
        h = rotacaoEsquerda(h);
        inverterCores(h);
    }
    return h;
}


RubroNegra::No* RubroNegra::moverVermelhoDireita(No* h) {
    inverterCores(h);
    if (ehVermelho(h->esq->esq)) {
        h = rotacaoDireita(h);
        inverterCores(h);
    }
    return h;
}


bool RubroNegra::inserir(int id) {
    bool inseriu = false;
    raiz = inserir(raiz, id, inseriu);
    raiz->cor = PRETO;                         
    if (inseriu) n++;
    return inseriu;
}

RubroNegra::No* RubroNegra::inserir(No* h, int id, bool& inseriu) {
    if (!h) { inseriu = true; return new No(id, VERMELHO); }  
    if (id < h->chave)      h->esq = inserir(h->esq, id, inseriu);
    else if (id > h->chave) h->dir = inserir(h->dir, id, inseriu);
    else return h;                                
    return balancear(h);
}


bool RubroNegra::remover(int id) {
    if (!buscar(id)) return false;                

    if (!ehVermelho(raiz->esq) && !ehVermelho(raiz->dir))
        raiz->cor = VERMELHO;

    raiz = remover(raiz, id);
    if (raiz) raiz->cor = PRETO;
    n--;
    return true;
}

RubroNegra::No* RubroNegra::removerMinimo(No* h) {
    if (!h->esq) { delete h; return nullptr; }    
    if (!ehVermelho(h->esq) && !ehVermelho(h->esq->esq))
        h = moverVermelhoEsquerda(h);
    h->esq = removerMinimo(h->esq);
    return balancear(h);
}

RubroNegra::No* RubroNegra::remover(No* h, int id) {
    if (id < h->chave) {
        if (!ehVermelho(h->esq) && !ehVermelho(h->esq->esq))
            h = moverVermelhoEsquerda(h);
        h->esq = remover(h->esq, id);
    } else {
        if (ehVermelho(h->esq))
            h = rotacaoDireita(h);                

        if (id == h->chave && !h->dir) {          
            delete h;
            return nullptr;
        }
        if (!ehVermelho(h->dir) && !ehVermelho(h->dir->esq))
            h = moverVermelhoDireita(h);

        if (id == h->chave) {                     
            h->chave = minimo(h->dir)->chave;
            h->dir = removerMinimo(h->dir);
        } else {
            h->dir = remover(h->dir, id);
        }
    }
    return balancear(h);
}


bool RubroNegra::buscar(int id) const {
    const No* atual = raiz;
    while (atual) {
        if (id == atual->chave) return true;
        atual = (id < atual->chave) ? atual->esq : atual->dir;
    }
    return false;
}


void RubroNegra::emOrdem() const {
    if (!raiz) { std::cout << "(árvore vazia)\n"; return; }
    emOrdem(raiz);
    std::cout << '\n';
}

void RubroNegra::emOrdem(const No* no) const {
    if (!no) return;
    emOrdem(no->esq);
    std::cout << no->chave << ' ';
    emOrdem(no->dir);
}


void RubroNegra::imprimirNo(const No* no) const {
    if (ehVermelho(no))
        std::cout << "\033[1;31m" << no->chave << " (V)\033[0m\n";   // vermelho
    else
        std::cout << no->chave << " (P)\n";                          // preto
}

void RubroNegra::imprimir() const {
    if (!raiz) { std::cout << "(árvore vazia)\n"; return; }
    imprimirNo(raiz);
    imprimirFilhos(raiz, "");
}

void RubroNegra::imprimirFilhos(const No* no, const std::string& prefixo) const {
    if (!no->esq && !no->dir) return;
    imprimirRamo(no->esq, prefixo, false, "E: ");
    imprimirRamo(no->dir, prefixo, true,  "D: ");
}

void RubroNegra::imprimirRamo(const No* no, const std::string& prefixo,
                              bool ultimo, const char* lado) const {
    std::cout << prefixo << (ultimo ? "└── " : "├── ") << lado;
    if (!no) { std::cout << "-\n"; return; }
    imprimirNo(no);
    imprimirFilhos(no, prefixo + (ultimo ? "    " : "│   "));
}


bool RubroNegra::verificarPropriedades() const {
    if (ehVermelho(raiz)) return false;           // regra 1
    int alturaNegra;
    return verificar(raiz, LLONG_MIN, LLONG_MAX, alturaNegra);
}


bool RubroNegra::verificar(const No* no, long long min, long long max,
                           int& alturaNegra) const {
    if (!no) { alturaNegra = 1; return true; }    // nulo conta como preto
    if (no->chave <= min || no->chave >= max) return false;
    if (ehVermelho(no->dir)) return false;
    if (ehVermelho(no) && ehVermelho(no->esq)) return false;

    int ne, nd;
    if (!verificar(no->esq, min, no->chave, ne)) return false;
    if (!verificar(no->dir, no->chave, max, nd)) return false;
    if (ne != nd) return false;

    alturaNegra = ne + (ehVermelho(no) ? 0 : 1);
    return true;
}



std::vector<int> RubroNegra::emOrdemVetor() const {
    std::vector<int> v;
    v.reserve(n);
    coletar(raiz, v);
    return v;
}

void RubroNegra::coletar(const No* no, std::vector<int>& v) const {
    if (!no) return;
    coletar(no->esq, v);
    v.push_back(no->chave);
    coletar(no->dir, v);
}

int RubroNegra::filhos(int id) const {
    const No* atual = raiz;
    while (atual) {
        if (id == atual->chave)
            return (atual->esq ? 1 : 0) + (atual->dir ? 1 : 0);
        atual = (id < atual->chave) ? atual->esq : atual->dir;
    }
    return -1;
}

int RubroNegra::custoBusca(int id) const {
    int visitados = 0;
    const No* atual = raiz;
    while (atual) {
        visitados++;
        if (id == atual->chave) break;
        atual = (id < atual->chave) ? atual->esq : atual->dir;
    }
    return visitados;
}
