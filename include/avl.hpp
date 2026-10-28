#ifndef AVL_HPP
#define AVL_HPP

#include "arvoreBalanceada.hpp"

class AVL : public ArvoreBalanceada {
public:
    AVL() = default;
    ~AVL() override;

    bool inserir(int id) override;
    bool remover(int id) override;
    bool buscar(int id) const override;
    void emOrdem() const override;
    void imprimir() const override;
    int  altura() const override { return h(raiz); }
    int  tamanho() const override { return n; }
    bool verificarPropriedades() const override;
    std::string nome() const override { return "AVL"; }
    std::vector<int> emOrdemVetor() const override;
    int filhos(int id) const override;
    int custoBusca(int id) const override;

private:
    struct No {
        int chave;
        int altura;          
        No* esq;
        No* dir;
        explicit No(int k) : chave(k), altura(1), esq(nullptr), dir(nullptr) {}
    };

    No* raiz = nullptr;
    int n = 0;

    static int h(const No* no)  { return no ? no->altura : 0; }
    static int fb(const No* no) { return no ? h(no->esq) - h(no->dir) : 0; }
    static void atualizarAltura(No* no);

    No* rotacaoDireita(No* y);
    No* rotacaoEsquerda(No* x);
    No* balancear(No* no);

    No* inserir(No* no, int id, bool& inseriu);
    No* remover(No* no, int id, bool& removeu);

    void emOrdem(const No* no) const;
    void imprimirFilhos(const No* no, const std::string& prefixo) const;
    void imprimirRamo(const No* no, const std::string& prefixo,
                      bool ultimo, const char* lado) const;
    bool verificar(const No* no, long long min, long long max, int& alt) const;
    void destruir(No* no);
    void coletar(const No* no, std::vector<int>& v) const;
};

#endif