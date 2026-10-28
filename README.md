# Amazon Engineering Challenge — Árvores AVL e Rubro-Negra

Trabalho da Unidade II da disciplina **IMD0039 – Estruturas de Dados Básicas II** (IMD/UFRN).

**Autor:** Heitor Gomes de Sena Melo Mariz — Matrícula: 20250037383

---

## Contexto

Durante a Black Friday, o sistema de logística da Amazon precisa **registrar, localizar e remover identificadores de encomendas** com eficiência. Como os IDs costumam ser gerados em sequência, eles chegam **já ordenados**, e esse é o pior caso de uma árvore de busca binária comum: ela degenera numa lista e cada operação passa a custar O(n).

Este projeto é um protótipo em C++ com duas árvores balanceadas, **AVL** e **Rubro-Negra caída para a esquerda (LLRB)**. Elas garantem O(log n) em qualquer ordem de chegada. O projeto inclui uma bateria de testes que compara as duas com os mesmos dados e a mesma sequência de operações e embasa uma recomendação técnica.

---

## Estrutura do repositório

```
include/
  arvoreBalanceada.hpp   interface comum (classe abstrata) usada pelo menu e pelos testes
  avl.hpp                declaração da árvore AVL
  rubronegra.hpp         declaração da árvore Rubro-Negra (LLRB)
src/
  avl.cpp                implementação da AVL
  rubronegra.cpp         implementação da LLRB
  main.cpp               programa principal (menu interativo)
  testes.cpp             bateria de testes e análise comparativa
bin/
  arvores, testes        executáveis para Linux/WSL
  arvores.exe, testes.exe executáveis para Windows
resultados.csv           alturas, rotações e recolorações (Parte 3 dos testes)
tempos.csv               tempos de execução e custo de busca (Parte 4 dos testes)
```

---

## Requisitos

- Compilador **g++ com suporte a C++17** (testado com g++ 11.4 no Ubuntu 22.04 / WSL).
- Para gerar os executáveis de Windows a partir do Linux: **mingw-w64** (`sudo apt install mingw-w64`).
- **Nenhuma biblioteca externa.** Os algoritmos de balanceamento foram implementados do zero, sem `std::map`, `std::set` ou similares.

---

## Como compilar

Na raiz do projeto (Linux ou WSL):

```bash
mkdir -p bin
g++ -std=c++17 -O2 -Wall -Iinclude src/avl.cpp src/rubronegra.cpp src/main.cpp   -o bin/arvores
g++ -std=c++17 -O2 -Wall -Iinclude src/avl.cpp src/rubronegra.cpp src/testes.cpp -o bin/testes
```

Executáveis para Windows (compilação cruzada, sem depender de DLLs):

```bash
x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -Iinclude src/avl.cpp src/rubronegra.cpp src/main.cpp   -o bin/arvores.exe
x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -Iinclude src/avl.cpp src/rubronegra.cpp src/testes.cpp -o bin/testes.exe
```

Os executáveis já compilados estão na pasta `bin/`.

---

## Como executar

### Programa principal (menu)

```bash
./bin/arvores          # Linux/WSL
bin\arvores.exe        # Windows (Prompt de Comando ou PowerShell)
```

Ao iniciar, o usuário escolhe a estrutura. Depois, opera pelo menu:

```
Qual estrutura deseja utilizar?
  1 - Árvore AVL
  2 - Árvore Rubro-Negra (LLRB)

==================================================
  AMAZON LOGISTICS - Registro de Encomendas
  Estrutura: Rubro-Negra (LLRB)  |  Encomendas: 0
==================================================
  1 - Registrar encomenda
  2 - Registrar lote de encomendas
  3 - Buscar encomenda
  4 - Remover encomenda
  5 - Listar encomendas (em ordem crescente)
  6 - Visualizar estrutura da árvore
  7 - Estatísticas e verificação das propriedades
  8 - Trocar estrutura (reinicia os dados)
  0 - Sair
```

Exemplo de uso na LLRB: registrar o lote `10 20 30 40 50 25` (opção 2) e visualizar (opção 6):

```
6 registrada(s), 0 repetida(s) ignorada(s).
   Rebalanceamento: 3 rotação(ões), 2 recoloração(ões) | Altura: 4 | Propriedades: OK

Legenda: (V) = vermelho, (P) = preto | E = esquerda, D = direita
40 (P)
├── E: 20 (V)
│   ├── E: 10 (P)
│   └── D: 30 (P)
│       ├── E: 25 (V)
│       └── D: -
└── D: 50 (P)
```

No terminal, os nós vermelhos também aparecem em vermelho. Na AVL, a visualização mostra o fator de balanceamento (`fb`) de cada nó.

Também é possível alimentar o menu por linha de comando, útil para demonstrações:

```bash
printf '2\n2\n10 20 30 40 50 25\n6\n0\n' | ./bin/arvores
```

### Bateria de testes

```bash
./bin/testes              # roda tudo (Partes 1 a 4, cerca de 45 s)
./bin/testes --sem-tempo  # pula a Parte 4 (menos de 1 s)
./bin/testes --so-tempo   # roda só a Parte 4
```

O programa termina com código `0` se todas as checagens passarem e `1` se alguma falhar.

---

## Funcionalidades

| Operação | AVL | LLRB |
|---|---|---|
| Inserção com rebalanceamento | rotações simples e duplas | rotações + inversão de cores |
| Remoção com rebalanceamento | rotações simples e duplas | `moverVermelho` + rotações + inversão de cores |
| Busca | iterativa, O(log n) | iterativa, O(log n) |
| Percurso em ordem | crescente | crescente |
| Visualização da estrutura | mostra o `fb` de cada nó | mostra a cor de cada nó (V/P, em cor no terminal) |
| Verificação das propriedades | ordem de BST, alturas, \|fb\| ≤ 1 | ordem de BST, raiz preta, sem vermelho à direita, sem vermelhos seguidos, altura negra igual |
| Contadores | rotações | rotações e recolorações |

---

## Decisões de implementação

- **Interface comum (`ArvoreBalanceada`).** As duas árvores herdam da mesma classe abstrata. O menu e os testes trabalham pela interface, o que garante que as duas estruturas recebem exatamente as mesmas operações na comparação.
- **ID repetido:** a inserção é ignorada, `inserir()` retorna `false` e o menu avisa o usuário. A árvore não muda.
- **Busca ou remoção de ID inexistente:** retorna `false`, o usuário é avisado e nada muda.
- **Árvore vazia:** busca e remoção informam que não há encomendas, e o percurso mostra "(árvore vazia)".
- **Entrada inválida** (texto, ID ≤ 0): é rejeitada com mensagem, e o menu continua. IDs de encomenda são inteiros positivos.
- **Remoção de nó com 2 filhos:** nas duas árvores, o nó recebe a chave do **sucessor em ordem** (menor da subárvore direita), e o sucessor é removido.
- **Altura** é contada em níveis: árvore vazia = 0, um nó = 1.
- **Contagem de rotações:** cada rotação simples conta 1, então uma rotação dupla da AVL conta 2. Na LLRB, cada inversão de cores conta como 1 recoloração.
- **Por que LLRB:** é a variante de Rubro-Negra apresentada na disciplina. Ela é uma Rubro-Negra válida e equivale a uma árvore 2-3: um link vermelho representa um nó "colado" ao pai. Ela simplifica muito o código em relação à versão clássica (Cormen).
- **Na LLRB, o ponteiro nulo é considerado preto**, e a cor guardada no nó é a cor do link que vem do pai.
- **Remoção na LLRB:** antes de remover, o programa verifica se o ID existe, porque o algoritmo de Sedgewick pressupõe que a chave está na árvore. A inserção desce a árvore uma única vez, como na AVL, para não distorcer a comparação de tempo.

---

## Como as árvores funcionam

### AVL

Cada nó guarda sua altura. O **fator de balanceamento** `fb = altura(esq) − altura(dir)` precisa ficar entre −1 e 1. Depois de cada inserção ou remoção, os nós do caminho são rebalanceados na volta da recursão:

| Caso | Condição | Correção |
|---|---|---|
| Esquerda-Esquerda | fb > 1 e fb(esq) ≥ 0 | rotação à direita |
| Esquerda-Direita | fb > 1 e fb(esq) < 0 | rotação à esquerda no filho + à direita no nó |
| Direita-Direita | fb < −1 e fb(dir) ≤ 0 | rotação à esquerda |
| Direita-Esquerda | fb < −1 e fb(dir) > 0 | rotação à direita no filho + à esquerda no nó |

### Rubro-Negra caída para a esquerda (LLRB)

Regras: (1) a raiz é preta; (2) links vermelhos só existem à esquerda; (3) nunca há dois vermelhos seguidos; (4) todo caminho até um nulo tem a mesma quantidade de nós pretos. Novos nós entram vermelhos. Na volta da recursão, três correções restauram as regras:

1. vermelho à direita e preto à esquerda → **rotação à esquerda**;
2. dois vermelhos seguidos à esquerda → **rotação à direita**;
3. dois filhos vermelhos → **inversão de cores**, que sobe o vermelho para o pai.

Na remoção, `moverVermelhoEsquerda` e `moverVermelhoDireita` "emprestam" um vermelho do pai durante a descida, para que nunca se remova um nó preto isolado.

---

## Testes

A bateria (`src/testes.cpp`) roda **os mesmos casos e os mesmos dados nas duas árvores**:

- **Parte 1, testes funcionais:** árvore vazia; árvore com um elemento; busca de existentes e inexistentes; inserção de repetido; remoção de folha, de nó com 1 filho e de nó com 2 filhos (a configuração é confirmada antes de remover); remoção de inexistente; esvaziar a árvore validando cada passo.
- **Parte 2, exemplos de balanceamento:** rotação simples e dupla na AVL; rotação e inversão de cores na LLRB, com a árvore antes e depois.
- **Parte 3, análise comparativa:** inserção em ordem **crescente, decrescente e aleatória** (semente fixa), com N = 100, 1.000 e 10.000; buscas de existentes e inexistentes; remoção das N/2 primeiras encomendas que chegaram (despacho na ordem de chegada). As propriedades são verificadas após cada operação (a cada 100 operações para N = 10.000). Uma BST comum entra só como referência de altura. Saída: `resultados.csv`.
- **Parte 4, tempo e custo de busca:** N = 100 mil e 1 milhão, média de 5 execuções, mais a quantidade média de nós visitados por busca. Saída: `tempos.csv`.

Resultado: **133 checagens, 0 falhas.**

---

## Resultados

### Altura e rotações, N = 10.000

| Cenário | Altura BST | Altura AVL | Altura LLRB | Rotações AVL (inserção) | Rotações LLRB (inserção) | Rotações AVL (remoção) | Rotações LLRB (remoção) |
|---|---|---|---|---|---|---|---|
| Crescente | 10.000 | 14 | 14 | 9.986 | 9.987 | 2.497 | 5.004 |
| Decrescente | 10.000 | 14 | 18 | 9.986 | 9.982 | 2.497 | 55.625 |
| Aleatória | 28 | 16 | 19 | 7.078 | 11.903 | 2.135 | 29.379 |

A LLRB também fez cerca de 7.500 a 10.000 recolorações na inserção e 48.000 a 52.000 na remoção. Os resultados completos estão em `resultados.csv`.

### Tempo e custo de busca, N = 1.000.000 (média de 5 execuções, em ms)

| Cenário | Árvore | Inserção | Busca | Remoção | Nós visitados por busca | Altura |
|---|---|---|---|---|---|---|
| Crescente | AVL | 358,9 | 578,9 | 126,9 | 18,95 | 20 |
| Crescente | LLRB | 411,3 | 518,7 | 502,7 | 18,95 | 20 |
| Decrescente | AVL | 296,6 | 526,1 | 101,9 | 18,95 | 20 |
| Decrescente | LLRB | 484,5 | 562,5 | 533,2 | 19,27 | 26 |
| Aleatória | AVL | 829,3 | 420,1 | 502,2 | 19,29 | 24 |
| Aleatória | LLRB | 845,0 | 433,1 | 746,3 | 19,62 | 29 |

Os tempos foram medidos no WSL do autor e variam conforme a máquina. Altura, rotações e nós visitados são determinísticos. Os resultados completos estão em `tempos.csv`.

---

## Análise e recomendação técnica

**Recomendação: árvore AVL.**

1. **As duas resolvem o problema da Black Friday.** Com IDs chegando ordenados, a BST comum chegou a altura 10.000, enquanto AVL e LLRB ficaram em 14. Qualquer uma das duas elimina o pior caso O(n).
2. **A AVL mantém a árvore mais baixa.** Ela ficou com altura menor ou igual à da LLRB em todos os cenários (por exemplo, 16 contra 19 no aleatório com N = 10.000). Isso era esperado pela teoria: a AVL garante altura de no máximo cerca de 1,44·log₂ n, e a Rubro-Negra, de no máximo 2·log₂ n.
3. **A AVL rotacionou menos.** Na inserção, as duas empatam com dados ordenados, mas no aleatório a AVL fez cerca de 40% menos rotações. Na remoção, a diferença é grande: de 2 a 22 vezes menos rotações, sem contar as cerca de 50 mil recolorações da LLRB. Isso acontece porque a LLRB força todos os links vermelhos para a esquerda e precisa "emprestar" vermelhos durante a descida da remoção.
4. **Os tempos confirmam as métricas.** A AVL foi mais rápida para inserir em todos os cenários e de 1,5 a 5 vezes mais rápida para remover. A busca ficou praticamente empatada: a diferença é de no máximo 0,33 nó visitado por busca.
5. **O padrão de uso do cenário favorece a AVL.** Na Black Friday, os IDs chegam em sequência (cenário crescente) e as encomendas mais antigas são despachadas primeiro. Nessa combinação, a remoção na AVL levou 127 ms, contra 503 ms na LLRB, com 1 milhão de encomendas.

### Ressalvas e limitações

- A comparação foi feita com a **LLRB**, não com a Rubro-Negra **clássica** (Cormen). A clássica limita o rebalanceamento a no máximo 2 rotações por inserção e 3 por remoção, e é a usada em `std::map` (C++), `TreeMap` (Java) e no kernel Linux. Em cargas com muitas escritas, ela tende a ser competitiva com a AVL ou melhor que ela. A conclusão deste trabalho vale para as variantes implementadas.
- A AVL guarda a altura em cada nó (um `int`), enquanto a Rubro-Negra guarda só a cor. Na prática, por causa do alinhamento de memória, os dois nós ocupam o mesmo espaço nesta implementação (24 bytes em 64 bits).
- Tempos dependem da máquina e da carga do sistema. Por isso, a análise se apoia principalmente em altura, rotações e nós visitados, que são reproduzíveis.

### Quando usar cada uma

- **AVL:** quando a busca precisa ser a mais rápida e previsível possível (árvore mais baixa), e quando há remoções frequentes em comparação com uma LLRB. Exemplos: índices consultados com frequência, o protótipo deste desafio.
- **LLRB:** quando a simplicidade e a clareza do código são prioridade, como em ensino, protótipos e código fácil de verificar. Ela também é boa para cargas dominadas por inserções, em que as duas empatam.
- **Rubro-Negra clássica:** bibliotecas de uso geral e sistemas com muitas escritas, onde o número máximo de rotações por operação importa.
---

## Uso I.A.

**Utilizei inteligência artificial (Claude, da Anthropic) como apoio para compreender falhas no código, estruturar os testes, gerar as imagens .png dos slides e organizar o README.**
