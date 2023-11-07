#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Define product structure
typedef struct  {
    int cod;
    char name[50];
    float price;
    int stock;
    int sales_count;
} Product;

Product* p;

// Define cart item structure
typedef struct  {
    int cod;
    char name[50];
    int quantity;
    float unit_price;
    float subtotal;
} CartItem;

CartItem *sacola;

// Funções Principaís para Produtos
void submenu_produtos();
void prod_exec(int opt);
void cadastrar_estoque();
void atualizar_estoque(int stock_index);
void excluir_item(int stock_index);
void salvar_dados(char* filename);
void ler_dados(char* filename);

// Funções Principaís para Vendas
void submenu_vendas();
void vendas_exec(int opt);
void realizar_venda();

// Sacola 
int update_cart(int cod);
void ordernar_sacola(); // loop
void reset_sacola(); // 0
void save_compra_to_file(float total);


// Cobranças e Calculos 
void cobrar(int forma_pgto, int total);
float calc_total();
void faturar_vista(float total);
void faturar_prazo(float total);
float calc_desconto_vista(float total);
float acrescentar_prazo(float total, int parcelas);


// Coleção de dados
int buscar_cod(char* msg, int (*validar)(int));
int buscar_quantidade();
char* buscar_nome();
float buscar_stock_value();
int buscar_forma_de_pagamento(); // loop
int buscar_parcelas(); // loop

// Validações e Funções utilitárias
int valid_product_code(int cod);
int is_existent(int cod);
int valid_quantity(int qty, int item_estoque);
int is_natural_number(int num);
int authorized(char* msg);

void limpar_tela();

// Algoritmos e coisas divertidos 
int pesquisa_prod(int target, void (*callback)(int));
int pesquisa_sacola(int target, int (*callback)(int));
int stricmp(char *s1, char *s2);


// Visualizações
void visualizar_sacola();
void visualizar_estoque();
void visualizar_compra(float total);
void visualizar_formas_pgto();
void visualizar_relatorio();

int stock_count = 0;
int cart_counter = 0;

int main() {    
    int opc;
    p = (Product*)malloc(stock_count * sizeof(Product));
    sacola = (CartItem *)malloc(cart_counter * sizeof(CartItem));

    if (p == NULL || sacola == NULL){
        printf("Memory allocation failed...\n");
        exit(1);
    }

    do {
        printf("\n\nMenu\n\n");
        printf("1. Produtos\n");
        printf("2. Vendas\n");
        printf("3. Sair\n");
        printf("\nEscolha uma opção:  ");
        scanf("%d", &opc);
        getchar();

        switch (opc) {
            case 1:
                limpar_tela();
                submenu_produtos();
                break;
            case 2:
                limpar_tela();
                submenu_vendas();
                break;
            case 3:
                printf("\nQuitting...\n");
                return 0;
            default:
                limpar_tela();
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opc != 3);

    free(p);
    free(sacola);
    return 0;
}

void submenu_produtos() {
    int opt;
    
    do {
        printf("\nProdutos\n");
        printf("1. Exibir\n");
        printf("2. Cadastrar\n");
        printf("3. Atualizar\n");
        printf("4. Excluir\n");
        printf("5. salvar_dados\n");
        printf("6. Ler dados\n");
        printf("7. Voltar\n");
        printf("Digite opcao: ");
        scanf("%d", &opt);
        
        // Executar a opcao selecionado
        prod_exec(opt);
    } while (opt != 7);
}

void prod_exec(int opt) {
    int update_status, del_status;

    switch (opt)
    {
        case 1:
            visualizar_estoque();
            break;
        case 2:
            cadastrar_estoque();
            break;
        case 3:
            update_status = pesquisa_prod(buscar_cod("codigo do produto para atualizar: ", is_existent), atualizar_estoque); // procura e atualiza
            if (update_status < 0)
                printf("\nProduto não encontrado!\n");

            break;
        case 4:
            del_status = pesquisa_prod(buscar_cod("codigo do produto para excluir: ", is_existent), excluir_item); // procura e excluir

            if (del_status < 0)
                printf("\nProduto não encontrado!\n");
            break;
        case 5:
            salvar_dados("berenice_dados.bin");
            break;
        case 6:
            ler_dados("berenice_dados.bin");
            break;
        case 7:
            printf("Voltando...");
            break;
        default:
            printf("Opcao invalida! Tente novamente\n\n");
            // return 0;
            break;
    }
}


void cadastrar_estoque() {
    Product* new_prod = (Product *) realloc(p, (stock_count+1) * sizeof(Product));
    if (new_prod == NULL){
        printf("Memory allocation failed...\n");
        return;
    }

    int cod = buscar_cod("Digite o codigo do cadastro: ", valid_product_code);
    char* nome = buscar_nome(); // DONE validate if the name does not exist already
    int quantidade = buscar_quantidade();
    float valor = buscar_stock_value();
    
    p = new_prod;
    
    p[stock_count].cod = cod;
    strcpy(p[stock_count].name, nome);
    p[stock_count].stock = quantidade;
    p[stock_count].price = valor;

    stock_count++;
}

void atualizar_estoque(int stock_index) {
    
    int opt;
    Product prod = p[stock_index];
    
    do {
        visualizar_estoque();
        printf("\n\nAtualizacao\n");
        printf("1. Codigo\n");
        printf("2. Nome\n");
        printf("3. Valor\n");
        printf("4. Estoque\n");
        printf("5. Sair\n");
        printf("Escolha oq queres alterar no produto: ");
        scanf("%d", &opt);

        switch (opt) {
            // Atualizar codigo
            case 1:
                printf("\nATUALIZAR CODIGO\n");
                int current_cod = prod.cod;
                int new_cod = buscar_cod("Digite o novo codigo: ", valid_product_code);

                prod.cod = new_cod;
                printf("\n\nAtualizado %d para %d com sucesso\n", current_cod, new_cod);
                break;
            // Atualizar Nome do produto
            case 2:
                printf("\nATUALIZAR NOME DO PRODUTO\n");
                const char* current_name = prod.name; // Use const to indicate it's a pointer to a constant string
                char* novo_nome = buscar_nome();

                strcpy(prod.name, novo_nome);
                printf("\n\nAtualizado %s para %s com sucesso\n", current_name, novo_nome);
                break;
            // Atualizar valor do produto
            case 3:
                printf("\nATUALIZAR VALOR DO PRODUTO\n");
                float current_price = prod.price;
                float new_value = buscar_stock_value();

                prod.price = new_value;
                printf("\n\nAtualizado %.2f para %.2f com sucesso\n", current_price, new_value);
                break;
            // Atualizar estoque
            case 4:
                printf("\nATUALIZAR ESTOQUE\n");
                int current_stock = prod.stock;
                int new_stock = buscar_quantidade();

                prod.stock = new_stock;
                printf("\n\nAtualizado %d para %d com sucesso\n", current_stock, new_stock);
                break;
            case 5:
                printf("\nSaindo...\n");
                break;
            default:
                printf("\nOpção inválida... tente novamente\n");
                break;
        }
    } while(opt != 5);

    p[stock_index] = prod;
}

void excluir_item(int stock_index) {
    visualizar_estoque();

    // Confirmar se o usuario realmente quer excluir o produto
    if (authorized("Realmente quer excluir este produto?")) {
        // mandar o produto para excluir pro fim do vetor
        for (int j = stock_index; j < stock_count - 1; j++) {
            p[j] = p[j+1];
        }

        stock_count--;

        p = (Product *)realloc(p, sizeof(Product) * stock_count);
        if (p == NULL){
            printf("\nMemory error\n");
            exit(1);
        }

        printf("\nProduct with codigo  %d deleted.\n", stock_index+1);
    } else {
        printf("\n\nExcluição cancelado...\n");
    }
}

void salvar_dados(char* filename) {
    FILE* arq = fopen(filename, "wb");
    if (arq == NULL) {
        perror("Error opening file");
        return;
    }

    fwrite(&stock_count, sizeof(int), 1, arq);

    for (int i = 0; i < stock_count; i++) {
        fwrite(&p[i], sizeof(Product), 1, arq);
    }

    fclose(arq);
}

// Ler dados em formato binaria 
void ler_dados(char* filename) {
    FILE* file = fopen(filename, "rb"); 

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int num_products;

    if (fread(&num_products, sizeof(int), 1, file) != 1) {
        perror("Error reading the number of products");
        fclose(file);
        return;
    }

    p = realloc(p, num_products * sizeof(Product));
    if (p == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    stock_count = num_products;

    for (int i = 0; i < num_products; i++) {
        if (fread(&p[i], sizeof(Product), 1, file) != 1) {
            perror("Error reading product data");
            break;
        }
    }

    fclose(file);
}

// NOTE VENDAS 

void submenu_vendas() {
    int opt;
    
    do {
        printf("\nProdutos\n");
        printf("1. Realizar venda\n");
        printf("2. Relatório de vendas\n");
        printf("3. Voltar\n");

        printf("Digite opcao: ");
        scanf("%d", &opt);
        
        // Executar a opcao selecionado
        vendas_exec(opt);
    } while (opt != 3);
}

void vendas_exec(int opt) {
    switch (opt) {
        case 1:
            realizar_venda();
            break;
        case 2:
            visualizar_relatorio();
            break;
        case 3:
            printf("\nVoltando...\n");
            return;
        default:
            printf("Opção Invalida, tenta novamente...");
            break;
    }
}

void realizar_venda() {
    int cod;
    float total;

    while (1) {
        visualizar_estoque();
        cod = buscar_cod("\nDigite o código do item desejado: ", is_existent);

        
        if (!update_cart(cod)) // senao atualizou com successo, repetir o loop
            continue;

        printf("Cart updated Successfully!\n");
        
        ordernar_sacola();
        visualizar_sacola();

        // Ver se o usuario quer repetir a venda, em caso de erro ou não
        if (!authorized("Adicionar mais itens?")){
            if (cart_counter < 0) return; // Se o carrinho está vazio e não quer fazer venda, sair para o menu principal

            break;
        }
    }

    total = calc_total();
    visualizar_compra(total);
    
    // se nao quer pagar, retornar os produtos e apagar tudo na sacola
    if (!authorized("Gostaria de pagar? ")) {
        for (int i = 0; i < stock_count; i++) 
            for (int j = 0; j < cart_counter; j++)
                if (sacola[j].cod == p[i].cod)
                    p[i].stock += sacola[j].quantity;

        reset_sacola();
        return;
    }
        
    cobrar(buscar_forma_de_pagamento(), total);
    save_compra_to_file(total);
    
    reset_sacola();
}

int update_cart(int cod){
    int product_index = pesquisa_prod(cod, NULL);
    Product item = p[product_index]; // procurar o index do nosso item
    
    int quant = buscar_quantidade();
    if (quant > item.stock) {
        printf("\nQuantidade inválida ou insuficiente\n");
        return 0;
    }
    item.stock -= quant;

    sacola = (CartItem *) realloc(sacola, (cart_counter+1) * sizeof(CartItem));
    if (sacola == NULL){
        printf("Memory allocation failed...\n");
        exit(1);
    }

    int index = pesquisa_sacola(cod, NULL); // atualiza existente ou adicionar no fim da sacola, index do item ou -1 

    sacola[index] = (CartItem) { cod, "", quant, item.price, quant * item.price };
    strcpy(sacola[index].name, item.name);

    item.sales_count += quant;

    // Atualizar com os novos dados
    p[product_index] = item;
    cart_counter++;

    return 1;
}

void ordernar_sacola() {
    int i, j;
    CartItem key;

    for (i = 1; i < cart_counter; i++) {
        key = sacola[i];
        j = i - 1;

        while (j >= 0 && sacola[j].subtotal < key.subtotal) {
            sacola[j + 1] = sacola[j];
            j = j - 1;
        }

        sacola[j + 1] = key;
    }
}

void reset_sacola() {
    cart_counter = 0;
    free(sacola);
    
    sacola = (CartItem *)malloc(cart_counter * sizeof(CartItem));

    if (sacola == NULL){
        printf("Memory allocation failed...\n");
        exit(1);
    }
}

void save_compra_to_file(float total) {
    time_t now;
    struct tm *timeinfo;
    char filename[50];

    time(&now);
    timeinfo = localtime(&now);
    strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S.txt", timeinfo);

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "\n\n\t\t\t\t\tReceipt\n\n\n");
    fprintf(file, "Seq\t\tItem Name\t\tValor Unit\t\tQuantity\tSubtotal\n");
    fprintf(file, "----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < cart_counter; i++) {
        fprintf(
            file,
            "%d\t\t%-15s\t\tR$ %.2f\t\t\t%d\t\tR$ %.2f\n",
            i + 1, sacola[i].name, sacola[i].unit_price, sacola[i].quantity, sacola[i].subtotal
        );
    }

    fprintf(file, "----------------------------------------------------------------------------------------\n");
    fprintf(file, "Total\t\t\t\t\t\t\t\t\t\tR$ %.2f\n", total);
    fprintf(file, "----------------------------------------------------------------------------------------\n");

    fclose(file);
}


// Cobrança

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

float calc_total() {
    float acc = 0; // Acumulador de valores

    for (int i = 0; i < cart_counter; i++)
        acc += sacola[i].subtotal;

    return acc;
}

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


void faturar_prazo(float total){
    limpar_tela();

    printf("Total a pagar: R$ %.2f\n", total);

    int parcelas = buscar_parcelas();
    float valor_pagar = acrescentar_prazo(total, parcelas);
    
    float valor_parcelas = valor_pagar / parcelas;

    printf("\nNovo total: R$ %.2f\n", valor_pagar);
    printf("Parcela de R$ %.2f por %dx\n", valor_parcelas, parcelas);
}

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

int buscar_cod(char* msg, int (*validar)(int)) {
    int cod;

    // Terminar quando o usuario digitar o codigo valido
    do {
        printf("%s", msg);
        scanf("%d", &cod);
        getchar();

        if (validar(cod)){
            printf("\nInvalid input...\n");
            continue;
        }

    } while (validar(cod)); // se o usuario digitar numero invalido, repete o codigo

    return cod;
}


int buscar_quantidade() {
    int quantidade;

    do {
        printf("\nDigite a quantidade: ");
        scanf("%d", &quantidade);
        getchar();

        if (!is_natural_number(quantidade)) // se não é número natural ( numero menor que 0 ), satisfará esta condição 
            printf("\nQuantidade inválida, a mesma deve ser maior ou igual a zero\n\n");

    } while(!is_natural_number(quantidade));

    return quantidade;
}

char* buscar_nome() {
    char buffer[50];
    
    while(1) {
        int existe = 0; 

        printf("\nDigite o nome do produto: ");
        if (scanf(" %[^\n]", buffer) != 1){
            printf("\nInput Error...\n");
            fflush(stdin);
            continue;
        }

        // Validar o nome... 
        for (int i = 0; i < stock_count; i++) {
            if (stricmp(buffer, p[i].name) == 0){
                printf("ja existe!\n");
                existe = 1;
            }
        }
        if (existe)
            continue;

        // limpar o nosso string e apagar o \n
        size_t length = strlen(buffer);
        if (length > 0 && buffer[length - 1] == '\n')
            buffer[length - 1] = '\0';

        return strdup(buffer);
    }
}

float buscar_stock_value(){
    float valor;

    do {
        printf("\nDigite o valor do produto: ");
        scanf("%f", &valor);

        if (valor <= 0)
            printf("\nInvalid value, digite novamente!\n");
            
    } while (valor <= 0);

    return valor;
}


// Validações e Funções utilitárias

int authorized(char* msg) {
    char opt;

    while (1) {
        printf("%s\n[s] - Sim\n[n] - Nao\n=> ", msg);
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
}

int valid_quantity(int qty, int item_estoque) {
    if (qty <= item_estoque)
        return 1;
    
    printf("\nQuantidade inválida ou insuficiente\n");
    return 0;
}

int is_existent(int cod) {
    if (pesquisa_prod(cod, NULL) >= 0){
        return 0;
    }

    return 1;
}

int valid_product_code(int cod) {
    if (pesquisa_prod(cod, NULL) > 0){
        return 1;
    }

    return 0;
}

int is_natural_number(int num) {
    return num > 0;
}


// Algoritmos e coisas divertidos 
int stricmp(char *s1, char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char) *s1) != tolower((unsigned char) *s2))
            return 1; // strings nao eh igual

        s1++;
        s2++;
    }

    return (*s1 || *s2) ? 1 : 0; // Check if one string is longer than the other
}

int pesquisa_prod(int target, void (*callback)(int)) {
    int index;
    
    for (int i = 0; i < stock_count; i++) {
        if (p[i].cod == target) {
            if (callback != NULL)
                callback(i); // execute a callback to process the index
            
            return i; // callback is null, just return the unprocessed index
        }
    }

    return -1; // Found nothing
}

int pesquisa_sacola(int target, int (*callback)(int)) {
    int index;
    
    for (int i = 0; i < cart_counter; i++) {
        if (sacola[i].cod == target) {
            if (callback != NULL)
                callback(i); // execute a callback to process the index
            
            return i; // callback is null, just return the unprocessed index
        }
    }

    return cart_counter; // Found nothing
}



void limpar_tela() {
    system("clear");
}

void visualizar_sacola() {
    printf("\n\n\t\tSeu carrinho\n\n");
    if (cart_counter == 0){
        printf("\nCarrinho está vazio\n");
        return;
    }
    
    printf("--------------------------------------------------------------------------\n");
    for (int i = 0; i < cart_counter; i++)
        printf("%d. %-15s\t\t\t%d unidades\t\tSubtotal: %.2f\n", sacola[i].cod, sacola[i].name, sacola[i].quantity, sacola[i].subtotal);
    printf("--------------------------------------------------------------------------\n\n");

}

void visualizar_estoque() {
    printf("Código\t\tItem\t\tValor\t\tEstoque\n");
    printf("-------------------------------------------------------\n");

    for (int i = 0; i < stock_count; i++) {
        printf("%d\t\t%-25s\t- R$ %.2f\t%d\n", p[i].cod, p[i].name, p[i].price, p[i].stock);
    }

    printf("-------------------------------------------------------\n");
}

void visualizar_compra(float total) {
    limpar_tela();

    printf("\n\n\t\t\t\t\tReceipt\n\n\n");
    printf("Seq\t\tItem Name\t\tValor Unit\t\tQuantity\tSubtotal\n");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < cart_counter; i++) {
        printf(
            "%d\t\t%-15s\t\tR$ %.2f\t\t\t%d\t\tR$ %.2f\n",
            i+1, sacola[i].name, sacola[i].unit_price, sacola[i].quantity, sacola[i].subtotal
        );
    }

    printf("----------------------------------------------------------------------------------------\n");
    printf("Total\t\t\t\t\t\t\t\t\t\tR$ %.2f\n", total);
    printf("----------------------------------------------------------------------------------------\n");
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

void visualizar_relatorio() {
    // Print table header
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("| Codigo\t|\tNome\t\t\t|\t Quantidade Total\t|\tPreco Unidade\t|\tValor Total\t|\n");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    // Print each row of the table
    for (int i = 0; i < stock_count; i++) {
        printf("|%3d\t|\t%-20s\t|\t%3d\t\t\t|\t%.2f\t\t|\t%.2f\t\t|\n", p[i].cod, p[i].name, p[i].sales_count, p[i].price, (p[i].sales_count * p[i].price));
    }

    // Print table footer
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
}

