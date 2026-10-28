#ifndef ARVORE_BALANCEADA_HPP
#define ARVORE_BALANCEADA_HPP

#include <string>
#include <vector>

class ArvoreBalanceada {
public:
    virtual ~ArvoreBalanceada() = default;

    virtual bool inserir(int id) = 0;        // false se o ID já existe
    virtual bool remover(int id) = 0;        // false se o ID não existe
    virtual bool buscar(int id) const = 0;
    virtual void emOrdem() const = 0;
    virtual void imprimir() const = 0;
    virtual int  altura() const = 0;         
    virtual int  tamanho() const = 0;
    virtual bool verificarPropriedades() const = 0;
    virtual std::string nome() const = 0;

    virtual std::vector<int> emOrdemVetor() const = 0;  
    virtual int filhos(int id) const = 0;               
    virtual int custoBusca(int id) const = 0;           

    int  getRotacoes() const { return rotacoes; }
    int  getAjustes()  const { return ajustes; }   
    void zerarContadores() { rotacoes = 0; ajustes = 0; }

protected:
    int rotacoes = 0;
    int ajustes  = 0;
};

#endif