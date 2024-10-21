#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUTOS 100
#define ARQUIVO "produtos.txt"
#define MARGEM 0.20

int contadorVendas = 0; // Contador global de vendas
int contadorDias = 1;   // Contador global de dias

// Estrutura para armazenar as informações de um produto
struct Produto {
    int id;               // ID do produto (posição no estoque)
    char nome[50];        // Nome do produto
    float preco;          // Preço do produto
    float quantidade;     // Quantidade em Kg no estoque
};

// Função para limpar a tela
void limparTela() {
    // Limpa a tela dependendo do sistema operacional
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear"); // Linux e macOS
    #endif
}

// Função para salvar os produtos no arquivo
void salvarProdutos(struct Produto produtos[], int contador) {
    FILE *file = fopen(ARQUIVO, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    for (int i = 0; i < contador; i++) {
        fprintf(file, "ID:%d | Nome:%s | Preco por Kg: R$%.2f | Quantidade: %.2f Kg\n",
                produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade);
    }

    fclose(file);
}

// Função para carregar os produtos do arquivo
int carregarProdutos(struct Produto produtos[]) {
    FILE *file = fopen(ARQUIVO, "r");
    if (file == NULL) {
        printf("Arquivo não encontrado. Criando um novo...\n");
        return 0;
    }

    int contador = 0;
    while (fscanf(file, "ID:%d | Nome:%s | Preco por Kg: R$%f | Quantidade: %f Kg\n",
                  &produtos[contador].id, produtos[contador].nome,
                  &produtos[contador].preco, &produtos[contador].quantidade) == 4) {
        contador++;
    }

    fclose(file);
    return contador;
}

// Função para cadastrar um novo produto ou apenas atualizar a quantidade
void cadastrarProduto(struct Produto produtos[], int *contador, int modoAtualizacao, int idProdutoAtualizar) {
    if (modoAtualizacao == 0 && *contador >= MAX_PRODUTOS) {
        printf("Limite de produtos cadastrados atingido.\n");
        return;
    }

    struct Produto novoProduto;
    if (modoAtualizacao == 1) {
        novoProduto = produtos[idProdutoAtualizar];
        printf("Atualizando quantidade do produto %s (ID %d)\n", novoProduto.nome, novoProduto.id);
    } else {
        novoProduto.id = *contador + 1; // Define a posição no estoque com base no contador atual
        printf("Nome do produto: ");
        getchar(); // Para consumir o caractere de nova linha deixado pelo scanf anterior
        fgets(novoProduto.nome, sizeof(novoProduto.nome), stdin);
        novoProduto.nome[strcspn(novoProduto.nome, "\n")] = 0; // Remove a nova linha

        printf("Preco do produto por Kg: ");
        if (scanf("%f", &novoProduto.preco) != 1) {
            printf("Entrada invalida para preco!\n");
            return;
        }
    }

    printf("Quantidade no estoque (Kg): ");
    if (scanf("%f", &novoProduto.quantidade) != 1) { // Alterado para float
        printf("Entrada inválida para quantidade!\n");
        return;
    }

    if (modoAtualizacao == 1) {
        produtos[idProdutoAtualizar].quantidade += novoProduto.quantidade;
    } else {
        produtos[*contador] = novoProduto;
        (*contador)++;
    }

    printf("Produto atualizado com sucesso!\n\n");

    // Salva os produtos no arquivo
    salvarProdutos(produtos, *contador);
}

// Função para listar todos os produtos cadastrados
void listarProdutos(struct Produto produtos[], int contador) {
    if (contador == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    printf("Lista de produtos cadastrados:\n");
    for (int i = 0; i < contador; i++) {
        printf("ID: %d | Nome: %s | Preco por Kg: R$ %.2f | Quantidade: %.2f Kg\n",
               produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade);
    }
}

// Função para procurar um produto pelo ID
struct Produto* procurarProduto(struct Produto produtos[], int numProdutos, int id) {
    for (int i = 0; i < numProdutos; i++) {
        if (produtos[i].id == id) {
            return &produtos[i];
        }
    }
    return NULL;
}

// Função para registrar a venda
void registrarVenda(const char *nomeProduto, float precoFinal, float quantidadeVendida, int dia) {
    char arquivoVendas[50];
    sprintf(arquivoVendas, "vendas_dia_%d.txt", dia); // Cria nome do arquivo para o dia específico

    FILE *file = fopen(arquivoVendas, "a");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de vendas.\n");
        return;
    }

    fprintf(file, "Produto: %s| Preco com margem: R$%.2f | Quantidade vendida: %.2f Kg\n", nomeProduto, precoFinal, quantidadeVendida);
    fclose(file);
}

// Função para realizar a venda com opções de pagamento
void realizarVenda(struct Produto produtos[], int numProdutos, int *contador) {
    int idProduto;
    float quantidadeVenda;
    float total = 0.0;

    printf("Bem vindo ao sistema de PDV!\n");
    printf("Digite o ID do produto (0 para finalizar): ");

    while (1) {
        scanf("%d", &idProduto);

        // Se o ID for 0, finalizar o processo de venda
        if (idProduto == 0) {
            break;
        }

        // Procurar o produto pelo ID
        struct Produto *produto = procurarProduto(produtos, numProdutos, idProduto);

        if (produto != NULL) {
            printf("Quantidade a vender (Kg): ");
            if (scanf("%f", &quantidadeVenda) != 1) { // Garantir que a leitura é válida
                printf("Entrada invalida para quantidade!\n");
                continue; // Volta ao início do loop para tentar novamente
            }

            if (quantidadeVenda > 0 && quantidadeVenda <= produto->quantidade) {
                float precoFinal = produto->preco * (1 + MARGEM); // Aplicar a margem de 20%
                float valorVenda = precoFinal * quantidadeVenda; // Calcular valor total da venda
                printf("Preco do produto %s: R$%.2f\n", produto->nome, precoFinal); // Mostrar preço por Kg
                printf("Valor total da venda: R$%.2f\n", valorVenda); // Mostrar valor total
                total += valorVenda; // Adicionar o valor total ao total geral
                produto->quantidade -= quantidadeVenda; // Diminuir a quantidade em estoque
                contadorVendas++; // Incrementa a contagem de vendas
                registrarVenda(produto->nome, precoFinal, quantidadeVenda, contadorDias); // Registrar a venda no dia correto

                // Verificar se o estoque zerou e perguntar se deseja atualizar
                if (produto->quantidade == 0) {
                    char resposta;
                    printf("O estoque do produto %s acabou. Deseja repor? (s/n): ", produto->nome);
                    scanf(" %c", &resposta);
                    if (resposta == 's' || resposta == 'S') {
                        cadastrarProduto(produtos, contador, 1, produto->id - 1);
                    }
                }
            } else if (quantidadeVenda <= 0) {
                printf("A quantidade deve ser maior que zero.\n\n");
            } else {
                printf("Estoque insuficiente para o produto %s. Disponivel: %.2f Kg\n\n", produto->nome, produto->quantidade);
            }
        } else {
            printf("Produto com ID %d nao encontrado.\n\n", idProduto);
        }

        printf("Digite o ID do próximo produto (0 para finalizar): ");
    }

    printf("Total a pagar: R$%.2f\n\n", total);

    // Verificação se o total é igual a zero
    while (total == 0) {
        printf("O valor total não pode ser cobrado pois é igual a zero.\n");
        break; // Finaliza o loop caso o total seja zero
    }

    // Adicionando opções de pagamento
    int metodoPagamento;
    printf("Escolha o metodo de pagamento:\n\n");
    printf("1. Dinheiro\n");
    printf("2. Cartao\n");
    printf("3. Pix\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &metodoPagamento);

    switch (metodoPagamento) {
        case 1:
            printf("Pagamento em dinheiro realizado.\n\n");
            break;
        case 2: {
            int tipoCartao;
            printf("Escolha o tipo de cartao:\n");
            printf("1. Credito\n");
            printf("2. Debito\n");
            printf("3. Voucher\n");
            printf("Escolha uma opcao: ");
            scanf("%d", &tipoCartao);
            switch (tipoCartao) {
                case 1:
                    printf("Pagamento realizado com cartao de credito.\n");
                    break;
                case 2:
                    printf("Pagamento realizado com cartao de debito.\n");
                    break;
                case 3:
                    printf("Pagamento realizado com cartao de voucher.\n");
                    break;
                default:
                    printf("Opcao invalida. Tente novamente.\n");
                    break;
            }
            break;
        }
        case 3:
            printf("Pagamento via Pix realizado.\n\n");
            break;
        default:
            printf("Metodo de pagamento invalido. Tente novamente.\n\n");
            break;
    }

    salvarProdutos(produtos, *contador); // Salva o estado atualizado dos produtos
}

// Função para finalizar o dia e criar um novo arquivo de vendas para o dia seguinte
void finalizarDia() {
    printf("Dia finalizado. Relatório salvo.\n");
    contadorDias++; // Incrementa o contador de dias
    contadorVendas = 0; // Reinicia o contador de vendas do novo dia
}

// Função principal
int main() {
    struct Produto produtos[MAX_PRODUTOS];
    int numProdutos = carregarProdutos(produtos); // Carrega os produtos do arquivo
    int opcao;

    do {
        system("clear || cls");

        printf("\n1. Cadastrar novo produto\n");
        printf("2. Listar produtos\n");
        printf("3. Realizar venda\n");
        printf("4. Finalizar dia\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                cadastrarProduto(produtos, &numProdutos, 0, 0);
                break;
            case 2:
                listarProdutos(produtos, numProdutos);
                break;
            case 3:
                realizarVenda(produtos, numProdutos, &numProdutos);
                break;
            case 4:
                finalizarDia();
                break;
            case 5:
                printf("Saindo do progr;ama...\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
        if (opcao != 6) {
            printf("Pressione Enter para continuar...");
            getchar(); // Consome o caractere de nova linha pendente
            getchar(); // Espera o usuário pressionar Enter
        }
    } while (opcao != 5);

    return 0;
}

