#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// Funções de Setup e Menu
void setup_stock();


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

// Coleção de dados
int buscar_cod(char* msg, int (*validar)(int));
int buscar_quantidade();
char* buscar_nome();
float buscar_stock_value();


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
void visualizar_prod(Product prod);
void visualizar_estoque();

int stock_count = 0;
int cart_counter = 0;

void setup_stock() {
    
    Product initial_stock[5] = {
        {1, "Pão de Forma", 7.50, 5},
        {2, "Pão de Centeio", 8.69, 6},
        {3, "Broa de Milho", 5.0, 2},
        {4, "Sonho", 4.5, 3},
        {5, "Tubaína", 3.25, 8}
    };

    for (int i = 0; i < stock_count; i++) {
        p[i] = initial_stock[i];
    }
}

int main() {    
    int opc;
    // stock_count = 5;
    p = (Product*)malloc(stock_count * sizeof(Product));
    sacola = (CartItem *)malloc(sizeof(CartItem));

    if (p == NULL || sacola == NULL){
        printf("Memory allocation failed...\n");
        exit(1);
    }

    // setup_stock();
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
    switch (opt)
    {
        case 1:
            visualizar_estoque();
            break;
        case 2:
            cadastrar_estoque();
            break;
        case 3:
            visualizar_estoque();

            // procura e atualiza
            int update_status = pesquisa_prod(buscar_cod("Qual produto quer atualizar (digite o codigo): ", is_existent), atualizar_estoque);

            if (update_status < 0)
                printf("\nProduto não encontrado!\n");
            break;
        case 4:
            visualizar_estoque();

            // procura e excluir
            int del_status = pesquisa_prod(buscar_cod("Digite o codigo do produto para excluir: ", is_existent), excluir_item);

            if (del_status < 0)
                printf("\nProduto não encontrado!\n");
            break;
        case 5:
            salvar_dados("berenice_dados.txt");
            break;
        case 6:
            ler_dados("berenice_dados.txt");
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
    // printf("%s", nome);
    int quantidade = buscar_quantidade();
    float valor = buscar_stock_value();
    
    p = new_prod;
    // printf("p foi atualizado");
    
    p[stock_count].cod = cod;
    // printf("p")
    strcpy(p[stock_count].name, nome);
    p[stock_count].stock = quantidade;
    p[stock_count].price = valor;

    stock_count++;
}

void atualizar_estoque(int stock_index) {
    int opt;
    Product prod = p[stock_index];
    
    do {
        visualizar_prod(prod);
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
    FILE* arq = fopen(filename, "w");
    if (arq == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(arq, "%d\n\n", stock_count); // quantidade_produtos

    for (int i = 0; i < stock_count; i++) {
        fprintf(arq, "%d\n", p[i].cod);
        fprintf(arq, "%s\n", p[i].name);
        fprintf(arq, "%.2f\n", p[i].price);
        fprintf(arq, "%d\n", p[i].stock);
        fprintf(arq, "%d\n\n", p[i].sales_count); // Initialize quant_vendida as 0
    }

    fclose(arq);
}

void ler_dados(char* filename) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int num_products;

    if (fscanf(file, "%d", &num_products) != 1) {
        perror("Error reading the number of products");
        fclose(file);
        return;
    }

    // Initialize or resize 'p' to accommodate the number of products
    p = realloc(p, num_products * sizeof(Product));
    if (p == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    stock_count = num_products;

    for (int i = 0; i < num_products; i++) {
        if (fscanf(file, "%d\n%49[^\n]\n%f\n%d\n", &p[i].cod, p[i].name, &p[i].price, &p[i].stock) != 4) {
            perror("Error reading product data");
            break;
        }

        int quant_vendida;
        if (i < num_products - 1) { // verificar se não estamos lendo o último linha no arquivo, pois estamos separar os produtos com "\n"
            if (fscanf(file, "%d\n", &quant_vendida) != 1) { // Ler um vaor inteiro seguido com "\n"
                perror("Error reading product quant_vendida");
                break;
            }
        }

        p[i].sales_count = quant_vendida;
    }

    fclose(file); // Close the file when done
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
    } while (opt != 7);
}

void vendas_exec(int opt) {
    switch (opt) {
        case 1:
            realizar_venda();
            break;
        case 2:

            break;
        case 3:
            printf("\nVoltando...\n");
            break;
        default:
            printf("Opção Invalida, tenta novamente...");
            break;
    }
}

void realizar_venda() {
    int cod;

    while (1) {
        visualizar_estoque();
        cod = buscar_cod("\nDigite o código do item desejado: ", is_existent);

        limpar_tela();

        int update_status = update_cart(cod);
        if (!update_status)
            continue;

        printf("Cart updated Successfully!\n");
        
        ordernar_sacola();
        visualizar_sacola();

        // Ver se o usuario quer repetir a venda, em caso de erro ou não
        if (!authorized){
            if (cart_counter < 0) return; // Se o carrinho está vazio e não quer fazer venda, sair para o menu principal

            break;
        }
    }

    // cobrança aqui... 
}


int update_cart(int cod){
    Product item = p[cod];
    
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
    p[cod] = item;
    // sacola = new_sacola;
    cart_counter++;
    return 1;
}

void ordernar_sacola() {
    int i, j;
    CartItem key;

    for (i = 1; i < cart_counter; i++) {
        key = sacola[i];
        j = i - 1;

        while (j >= 0 && sacola[j].unit_price > key.unit_price) {
            sacola[j + 1] = sacola[j];
            j = j - 1;
        }

        sacola[j + 1] = key;
    }
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

        if (valor <= 0) {
            printf("\nInvalid value, digite novamente!\n");
        }
    } while (valor <= 0);

    return valor;
}


// Validações e Funções utilitárias

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
            
            return (i-1); // callback is null, just return the unprocessed index
        }
    }

    return cart_counter; // Found nothing
}

int authorized(char* msg) {
    char opt;

    while (1) {
        printf("%s", msg);
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

void limpar_tela() {
    system("clear");
}

void visualizar_sacola() {
    printf("\n\t\tSeu carrinho\n\n");
    if (cart_counter == 0){
        printf("\nCarrinho está vazio\n");
        return;
    }
    
    printf("----------------------------------------------------------\n");
    for (int i = 0; i <= cart_counter; i++)
        printf("%d. %-15s\t\t\t%d unidades\t\tSubtotal: %.2f\n", sacola[i].cod, sacola[i].name, sacola[i].quantity, sacola[i].subtotal);
    printf("----------------------------------------------------------\n");

}


void visualizar_prod(Product prod) {
    printf("Código\t\tItem\t\tValor\t\tEstoque\n");
    printf("-------------------------------------------------------\n");

    printf("%d\t\t%-15s\t- R$ %.2f\t%d\n", prod.cod, prod.name, prod.price, prod.stock);

    printf("-------------------------------------------------------\n");
}

void visualizar_estoque() {
    printf("Código\t\tItem\t\tValor\t\tEstoque\n");
    printf("-------------------------------------------------------\n");

    for (int i = 0; i < stock_count; i++) {
        printf("%d\t\t%-25s\t- R$ %.2f\t%d\n", p[i].cod, p[i].name, p[i].price, p[i].stock);
    }

    printf("-------------------------------------------------------\n");
}