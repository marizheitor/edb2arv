#ifndef RUBRO_NEGRA_HPP
#define RUBRO_NEGRA_HPP

#include "arvoreBalanceada.hpp"

class RubroNegra : public ArvoreBalanceada {
public:
    RubroNegra() = default;
    ~RubroNegra() override;

    bool inserir(int id) override;
    bool remover(int id) override;
    bool buscar(int id) const override;
    void emOrdem() const override;
    void imprimir() const override;
    int  altura() const override { return alturaDe(raiz); }
    int  tamanho() const override { return n; }
    bool verificarPropriedades() const override;
    std::string nome() const override { return "Rubro-Negra (LLRB)"; }
    std::vector<int> emOrdemVetor() const override;
    int filhos(int id) const override;
    int custoBusca(int id) const override;

private:
    static constexpr bool VERMELHO = true;
    static constexpr bool PRETO    = false;

    struct No {
        int  chave;
        bool cor;           
        No*  esq;
        No*  dir;
        No(int k, bool c) : chave(k), cor(c), esq(nullptr), dir(nullptr) {}
    };

    No* raiz = nullptr;
    int n = 0;

    static bool ehVermelho(const No* no) { return no && no->cor == VERMELHO; }
    static int  alturaDe(const No* no);
    static const No* minimo(const No* h);

    No*  rotacaoEsquerda(No* h);
    No*  rotacaoDireita(No* h);
    void inverterCores(No* h);
    No*  moverVermelhoEsquerda(No* h);
    No*  moverVermelhoDireita(No* h);
    No*  balancear(No* h);

    No* inserir(No* h, int id, bool& inseriu);
    No* remover(No* h, int id);
    No* removerMinimo(No* h);

    void emOrdem(const No* no) const;
    void imprimirNo(const No* no) const;
    void imprimirFilhos(const No* no, const std::string& prefixo) const;
    void imprimirRamo(const No* no, const std::string& prefixo,
                      bool ultimo, const char* lado) const;
    bool verificar(const No* no, long long min, long long max, int& alturaNegra) const;
    void destruir(No* no);
    void coletar(const No* no, std::vector<int>& v) const;
};

#endif