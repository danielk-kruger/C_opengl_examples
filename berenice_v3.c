#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Funções principais
void cadastrar_estoque(); // loop
void realizar_venda(); // loop
int update_cart(int cod);
void cobrar(int forma_pgto, int total);


// Auxiliares principais
int pesquisar_item(int target);
void ordernar_carrinho(); // loop
void reset_carrinho(); // 0


// Calculos
float calc_total();
void faturar_vista(float total);
void faturar_prazo(float total);
float calc_desconto_vista(float total);
float acrescentar_prazo(float total, int parcelas);


// Validação de Inputs 
int buscar_produtos(char status[10]); // loop 
int buscar_quantidade(char item[], int qty_atual); // loop
int buscar_forma_de_pagamento(); // loop
int buscar_parcelas(); // loop
int iniciar_venda(char status[10]); // loop


// funções de visualização
void visualizar_estoque();
void visualizar_produtos();
void visualizar_compra(float total);
void visualizar_carrinho();
void visualizar_formas_pgto();
void visualizar_relatorio();


// funções utilitárias
int is_valid_input(int cod);
int valid_quantity(int qty, int item_estoque);
int is_natural_number(int num);
void limpar_tela();


// Dados do carrinho
int cart_size = -1; // vazio
int cart[5];
int quantidades[5] = { 0 };
float sub_totals[5] = { 0 };


// Dados fixos
char produtos[5][20] = { "Pão de Forma", "Pão de Centeio", "Broa de Milho", "Sonho", "Tubaína" };
float precos[5] = { 7.50, 8.69, 5.0, 4.5, 3.25 };
int estoque[5] = {5, 6, 2, 3, 8};


// relatorios
int total_quantidades[5] = {0}; 
float total_vendas[5] = {0};


int main() {
    // default values
    int opc;
    
    while (1) {
        printf("\n\nMenu\n\n");
        printf("1. Cadastrar Estoque\n");
        printf("2. Visualizar Estoque\n");
        printf("3. Realizar Venda\n");
        printf("4. Sair\n");

        printf("\nEscolha uma opção:  ");
        scanf("%d", &opc);
        getchar();

        switch (opc) {
            case 1:
                limpar_tela();
                cadastrar_estoque();
                break;
            case 2:
                limpar_tela();
                printf("\n\n\t\tLista de Estoque\t\t\n\n\n");

                visualizar_estoque();
                break;
            case 3:
                limpar_tela();
                realizar_venda();
                break;
            case 4:
                limpar_tela();
                printf("\nPrograma Encerrado!\n\n");
                visualizar_relatorio();
                return 0;
            default:
                limpar_tela();
                printf("Opcao invalida. Tente novamente.\n");
        }
    }

    return 0;
}

/*
-----------------------------
|    Funções principais     |
-----------------------------
*/

void cadastrar_estoque() {
    char status[10] = "estoque"; // STATUS -> estoque 

    int cod = buscar_produtos(status);
    int quantidade = buscar_quantidade(produtos[cod], estoque[cod]);

    estoque[cod] = quantidade; // alterar no vetor referente ao código do item a respectiva posição com a nova quantidade
    limpar_tela();
    printf("\nEstoque do %s alterado para %d com sucesso!!\n", produtos[cod], quantidade);
}

void realizar_venda() {
    char status[] = "venda"; 
    int cod;

    // Dados do Carrinho
    int total = 0;

    while (1) {
        cod = buscar_produtos(status); // buscar o codigo do produto que usuario escolheu 

        limpar_tela();
        int cart_updated = update_cart(cod);

        if (cart_updated) // Se o carrinho atualizou com successo, imprimir esta mensagem
            printf("\nItem added to cart successfully!\n\n");

        ordernar_carrinho();

        visualizar_carrinho();

        // Ver se o usuario quer repetir a venda, em caso de erro ou não
        if (!iniciar_venda(status)) { 
            if (cart_size < 0) return; // Se o carrinho está vazio e não quer fazer venda, sair para o menu principal

            break;
        }

        limpar_tela();
    }

    total = calc_total();

    // Visualizar o cupom fiscal
    visualizar_compra(total);
    
    strcpy(status, "pgto");
    if (iniciar_venda(status)) {
        // cobrar pagamento
        cobrar(buscar_forma_de_pagamento(), total);
        reset_carrinho();
    }
}

/**
 * @brief 
 * 
 * @param cod 
 * @return int 
 */
int update_cart(int cod) {
    int item;
    int qty = buscar_quantidade(produtos[cod], estoque[cod]);

    if (!valid_quantity(qty, estoque[cod])) {
        return 0; // deu erro, não atualize o carrinho
    }

    // decrementar estoque
    estoque[cod] -= qty;

    // cart[cod] = cod;
    item = pesquisar_item(cod);

    cart[item] = cod;
    total_quantidades[cod] += quantidades[item] += qty;
    total_vendas[cod] += sub_totals[item] += precos[cod] * qty;
    // cart_size++;

    return 1;
}
// 1 3 4
// 3 -> [1]
// 

void cobrar(int forma_pgto, int total) {
    switch (forma_pgto) {
        case 1:
            faturar_vista(total);
            break;
        case 2:
            faturar_prazo(total);
            break;
        case 3:
            printf("\nCancelando Pagamento.\n");
            return;
    }
}

/*
--------------------------------
|    Auxiliares principais     |
--------------------------------
*/

void reset_carrinho() {
    cart_size = -1; // vazio
    
    for (int i = 0; i < 5; i++) {
        cart[i] = 0;
        quantidades[i] = 0;
        sub_totals[i] = 0;
    }
}

/**
 * @brief 
 * 
 * @param cod 
 * @return int 
 */
int pesquisar_item(int cod) {
    int index;

    for (index = 0; index <= cart_size; index++)
        if (cod == cart[index]) // 1 -> pao forma 
            // foi encontrado no carrinho, retorne a localização do item
            return index; // local

    // se não existir no carrinho, adicione-o
    cart_size++; // -1 > 0 -> 1 > 2
    return cart_size;
}

// Insertion sort
void ordernar_carrinho() {
    int i;

    // Começamos no segundo item (índice 1) porque o primeiro item já está ordenado.
    for (i = 1; i <= cart_size+1; i++) {
        // Pegamos os valores-chave da parte não ordenada das listas.
        int cart_key = cart[i]; // pao de forma, sonho, 
        int quantity_key = quantidades[i]; // 4, 2
        float sub_total_key = sub_totals[i]; // 12, 8.50,

        // Agora, comparamos esses valores-chave com a parte ordenada das listas.
        // Queremos encontrar o local certo para os valores-chave na parte ordenada.
        int j = i - 1;

        // Movemos os itens maiores na parte ordenada uma posição à direita
        // até encontrar o local correto para os valores-chave.
        while (j >= 0 && sub_totals[j] < sub_total_key) {
            cart[j + 1] = cart[j]; // Move o ID do item do carrinho.
            quantidades[j + 1] = quantidades[j]; // Move a quantidade.
            sub_totals[j + 1] = sub_totals[j]; // Move o sub-total.
            j--;
        }

        // Finalmente, colocamos os valores-chave em seus locais corretos na parte ordenada.
        cart[j + 1] = cart_key;
        quantidades[j + 1] = quantity_key;
        sub_totals[j + 1] = sub_total_key;
    }
}

/*
------------------------------
|    Calculos principais     |
------------------------------
*/


float calc_total() {
    float acc = 0; // Acumulador de valores

    for (int i = 0; i <= cart_size; i++)
        acc += sub_totals[i];

    return acc;
}

/**
 * @brief 
 * 
 * @param total 
 */
void faturar_vista(float total) {
    limpar_tela();

    printf("Total a pagar: R$ %.2f\n", total);
    float valor_pagar, troco;
    float novo_total = calc_desconto_vista(total);

    do {
        printf("Novo total a pagar: R$ %.2f\n", novo_total);

        printf("\nDigite o valor de pgto: ");
        scanf("%f", &valor_pagar);
        getchar();

        if (valor_pagar < novo_total) {
            printf("Valor Invalida\n");
            continue;
        }

        troco = valor_pagar - novo_total;
        printf("\nSeu troco: R$ %.2f\n", troco);
        printf("Obrigado! Até mais!\n");
    } while (valor_pagar < novo_total);
}

/**
 * @brief 
 * 
 * @param total 
 */
void faturar_prazo(float total){
    limpar_tela();

    printf("Total a pagar: R$ %.2f\n", total);

    int parcelas = buscar_parcelas();
    float valor_pagar = acrescentar_prazo(total, parcelas);
    
    float valor_parcelas = valor_pagar / parcelas;

    printf("\nNovo total: R$ %.2f\n", valor_pagar);
    printf("Parcela de R$ %.2f por %dx\n", valor_parcelas, parcelas);
}

/**
 * @brief 
 * 
 * @param total 
 * @return float 
 */
float calc_desconto_vista(float total) {
    float novo_total = 0;
    float desconto = 0;

    if (total <= 50.00)
        desconto = total * 0.05; // desconto de 5%
    else if (total > 50.00 && total < 100.00)
        desconto = total * 0.10; // desconto de 10%
    else
        desconto = total * 0.18; // desconto de 18%
    
    printf("\nDesconto de R$ %.2f aplicado!\n", desconto);
    novo_total = total - desconto;

    return novo_total;
}

/**
 * @brief 
 * 
 * @param total 
 * @param parcelas 
 * @return float 
 */
float acrescentar_prazo(float total, int parcelas) {
    float novo_total = 0, acresento = 0;

    if (parcelas > 0 && parcelas <= 3)
        acresento = (total * 0.05);
    else if (parcelas > 3)
        acresento = (total * 0.18);        
    else
        printf("\nOpção de parcelas invalida\n");

    novo_total = total + acresento;
    return novo_total;
}


/*
-----------------------------
|    Validação de Inputs    |
-----------------------------
*/

/**
 * @brief 
 * 
 * @param status 
 * @return int 
 */
int buscar_produtos(char status[10]) {
    int cod;

    // Terminar quando o usuario digitar o codigo valido
    do {
        // visualizar tabela diferente / para facilitar tornar esta função mais reutilizável
        if (strcmp(status, "estoque") == 0) {
            printf("\n\n\tLista de Produtos\t\t\n\n\n");

            visualizar_produtos();
        } else if (strcmp(status, "venda") == 0) {
            printf("\n\n\t\tCardapio\t\t\n\n\n");

            visualizar_estoque();
        }
        
        printf("Digite o codigo: ");
        scanf("%d", &cod);
        getchar();

    } while (!is_valid_input(cod)); // se o usuario digitar numero invalido, repete o codigo

    return (cod - 1); // só para não ter que fazer isto toda hora -> vetor[cod - 1]
}

/**
 * @brief 
 * 
 * @param item 
 * @param qty_atual 
 * @return int 
 */
int buscar_quantidade(char item[], int qty_atual) {
    int quantidade;

    do {
        printf("\nSelecionado item: [%s - %d]\n", item, qty_atual);
        printf("\nDigite a quantidade: ");
        scanf("%d", &quantidade);
        getchar();

        if (!is_natural_number(quantidade)) // se não é número natural ( numero menor que 0 ), satisfará esta condição 
            printf("\nQuantidade inválida, a mesma deve ser maior ou igual a zero\n\n");

    } while(!is_natural_number(quantidade));

    return quantidade;
}

/**
 * @brief 
 * 
 * @return int 
 */
int buscar_forma_de_pagamento() {
    limpar_tela();
    int opt;

    do {
        visualizar_formas_pgto();
        printf("Digite opção: ");
        scanf("%d", &opt);
        getchar();

        if (opt < 0 || opt > 3)
            printf("\nOpção de pagamento inválida\n");
    } while (opt < 0 || opt > 3);

    return opt;
}

/**
 * @brief 
 * 
 * @return int 
 */
int buscar_parcelas() {
    int parcelas;

    do {
        printf("\nDigite o numero de parcelas: ");
        scanf("%d", &parcelas);
        getchar();

        if (!is_natural_number(parcelas))
            printf("\nNúmero de parcelas inválido, digite um valor igual ou acima de 1 parcela.\n");

    } while (!is_natural_number(parcelas));

    return parcelas;
}

/**
 * @brief 
 * 
 * @param status 
 * @return int 
 */
int iniciar_venda(char status[10]) {
    char opt;

    while (1) {
        if (strcmp(status, "venda") == 0) {
            printf("\nGostaria de adicionar mais itens?\n\n[s] - Sim\n[n] - Não\n");
        } else if (strcmp(status, "pgto") == 0) {
            printf("\nPodemos prosseguir com o processo de pagamento?\n\n[s] - Sim\n[n] - Não\n");
        }

        printf("Digite aqui: ");
        scanf("%c", &opt);
        getchar();

        // validar se escolheu sim ou não para reiniciar a venda
        // 1 -> true
        // 0 -> false
        switch (tolower(opt)){
            case 's':
                return 1;
            case 'n':
                return 0;
            default:
                printf("\nOpção Invalida! Tenta novamente\n");
        }        
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param qty 
 * @param item_estoque 
 * @return int 
 */
int valid_quantity(int qty, int item_estoque) {
    if (qty <= item_estoque)
        return 1;
    
    printf("\nQuantidade inválida ou insuficiente\n");
    return 0;
}

/**
 * @brief 
 * 
 * Este função verifica se o usuario digitou um numero entr 1 e 5
 * 
 * @param cod 
 * @return int 
 */
int is_valid_input(int cod) {
    if (cod < 1 || cod > 5) {
        printf("\nCódigo inválido\n");
        return 0; // false
    }
    
    return 1; // true
}

/**
 * @brief 
 * 
 * @param num 
 * @return int 
 */
int is_natural_number(int num) {
    return num > 0;
}

void limpar_tela() {
    system("clear");
}

/*
------------------------------
|    Tela / Visualizações    |
------------------------------
*/

void visualizar_estoque() {
    printf("Código\t\tItem\t\t\t\tEstoque\n");
    printf("-------------------------------------------------------\n");

    for (int i = 0; i < 5; i++) {
        printf("%d\t\t%-15s\t- R$ %.2f\t%d\n", i + 1, produtos[i], precos[i], estoque[i]);
    }

    printf("-------------------------------------------------------\n");
}

void visualizar_produtos() {
    printf("Código do Item\tItem\n");
    printf("----------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        printf("%d\t\t%s\n", i + 1, produtos[i]);
    }

    printf("----------------------------------\n");
}

/**
 * @brief 
 * 
 * @param total 
 */
void visualizar_compra(float total) {
    limpar_tela();

    printf("\n\n\t\t\t\t\tReceipt\n\n\n");
    printf("Seq\t\tItem Name\t\tValor Unit\t\tQuantity\tSubtotal\n");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i <= cart_size; i++) {
        printf(
            "%d\t\t%-15s\t\tR$ %.2f\t\t\t%d\t\tR$ %.2f\n",
            i+1, produtos[cart[i]], precos[cart[i]], quantidades[i], sub_totals[i]
        );
    }

    printf("----------------------------------------------------------------------------------------\n");
    printf("Total\t\t\t\t\t\t\t\t\t\tR$ %.2f\n", total);
    printf("----------------------------------------------------------------------------------------\n");
}


void visualizar_carrinho() {
    
    printf("\n\t\tSeu carrinho\n\n");
    if (cart_size < 0){
        printf("\nCarrinho está vazio\n");
        return;
    }
    
    printf("--------------------------------------------------\n");
    for (int i = 0; i <= cart_size; i++)
        printf("%d. %-15s\t\t\t%d unidades\n", i+1, produtos[cart[i]], quantidades[i]);
    printf("--------------------------------------------------\n");

}

void visualizar_formas_pgto() {
    printf("\n\nPayment Options:\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("Option\tPayment Type\t\tCondition\t\t\t\tDiscount\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("1\tÀ vista\t\t\tAté (incluindo) R$ 50,00\t\t5%% de desconto\n\n");
    printf("\t\t\t\tAcima de R$ 50,00 e abaixo de\t\t10%% de desconto\n");
    printf("\t\t\t\tR$ 100,00\t\t\t\n\n");
    printf("\t\t\t\tAcima (incluindo) de R$ 100,00\t\t18%% de desconto\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("2\tA prazo\t\t\tEm até (incluindo) 3 parcelas\t\t5%% de acréscimo\n\n");
    printf("\t\t\t\tAcima de 3 parcelas\t\t\t8%% de acréscimo\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("3\tCancel Payment\n");
    printf("---------------------------------------------------------------------------------------\n");
}


// SEQ | Nome | Quantidade Total | Preco Unidade | Valor total | 

void visualizar_relatorio() {
    // Print table header
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("| SEQ\t|\tNome\t\t\t|\t Quantidade Total\t|\tPreco Unidade\t|\tValor Total\t|\n");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    // Print each row of the table
    for (int i = 0; i < 5; i++) {
        printf("|%3d\t|\t%-20s\t|\t%3d\t\t\t|\t%.2f\t\t|\t%.2f\t\t|\n", i, produtos[i], total_quantidades[i], precos[i], total_vendas[i]);
    }

    // Print table footer
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
}