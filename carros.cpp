#include <iostream>
#include <string>
#include <limits> // Para std::numeric_limits
#include <pqxx/pqxx> // Biblioteca para PostgreSQL

// Configurações de conexão PostgreSQL
const std::string DB_HOST = "127.0.0.1"; // Ou "localhost"
const std::string DB_NAME = "postgres";  // Nome do seu banco de dados
const std::string DB_USER = "postgres";  // Seu usuário PostgreSQL
const std::string DB_PASSWORD = "root";  // Sua senha PostgreSQL
const std::string DB_PORT = "5432";      // Porta padrão do PostgreSQL

const std::string TABELA_CARROS = "carros";

// Função para construir a string de conexão
std::string get_connection_string() {
    return "host=" + DB_HOST +
           " port=" + DB_PORT + // Espaço antes de 'port' é importante!
           " dbname=" + DB_NAME +
           " user=" + DB_USER +
           " password=" + DB_PASSWORD;
}

int main() {
    try {
        // Estabelece a conexão com o banco de dados PostgreSQL
        pqxx::connection C(get_connection_string());
        std::cout << "Conexão com o banco de dados '" << C.dbname() << "' estabelecida com sucesso!" << std::endl;

        // Cria a tabela 'carros' se ela não existir
        // Usando pqxx::work para executar a query de criação de tabela
        pqxx::work W_create(C);
        std::string create_table_sql = "CREATE TABLE IF NOT EXISTS " + TABELA_CARROS + " ("
                                       "id SERIAL PRIMARY KEY," // Sintaxe correta para auto-incremento no PostgreSQL
                                       "marca VARCHAR(50) NOT NULL,"
                                       "nome VARCHAR(100) NOT NULL,"
                                       "ano INT,"
                                       "cor VARCHAR(30),"
                                       "preco DECIMAL(10, 2)"
                                       ");";
        W_create.exec(create_table_sql);
        W_create.commit(); // Confirma a transação
        std::cout << "Tabela 'carros' verificada/criada com sucesso." << std::endl;

        int escolha;
        do {
            std::cout << "\n--- MENU ---" << std::endl;
            std::cout << "1. Adicionar novo carro" << std::endl;
            std::cout << "2. Consultar todos os carros" << std::endl;
            std::cout << "3. Deletar carros por ID" << std::endl;
            std::cout << "0. Sair" << std::endl;
            std::cout << "Escolha uma opção: ";
            std::cin >> escolha;

            // Limpa o buffer de entrada após ler um número inteiro
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (escolha) {
                case 1: { // Adicionar novo carro
                    std::string marca, nome, cor;
                    int ano;
                    double preco;

                    std::cout << "Digite a marca do veículo: ";
                    std::getline(std::cin, marca);
                    std::cout << "Digite o nome do veículo: ";
                    std::getline(std::cin, nome);
                    
                    std::cout << "Digite o ano do veículo: ";
                    while (!(std::cin >> ano)) {
                        std::cout << "Entrada inválida. Digite um número para o ano: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

                    std::cout << "Digite a cor do veículo: ";
                    std::getline(std::cin, cor);
                    
                    std::cout << "Digite o valor do veículo: ";
                    while (!(std::cin >> preco)) {
                        std::cout << "Entrada inválida. Digite um número para o preço: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    try {
                        // Inicia uma transação
                        pqxx::work W(C);
                        // Prepara a consulta SQL para inserção com placeholders ($1, $2, etc.)
                        std::string sql = "INSERT INTO " + TABELA_CARROS + "(marca, nome, ano, cor, preco) VALUES ($1, $2, $3, $4, $5);";
                        
                        // Executa a query vinculando os valores
                        W.exec_params(sql, marca, nome, ano, cor, preco);
                        W.commit(); // Confirma a transação
                        std::cout << "Carro adicionado com sucesso!" << std::endl;
                    } catch (const pqxx::sql_error& e) {
                        // CORREÇÃO: e.sqlstate()
                        std::cerr << "Erro ao adicionar carro: " << e.what() << " (SQL state: " << e.sqlstate() << ")" << std::endl;
                    } catch (const pqxx::broken_connection& e) {
                        std::cerr << "Erro de conexão: " << e.what() << std::endl;
                    }
                    break;
                }
                case 2: { // Consultar todos os carros
                    std::cout << "\n--- CARROS CADASTRADOS ---" << std::endl;
                    try {
                        // Inicia uma transação de leitura (nontransaction é mais leve para selects)
                        pqxx::nontransaction N(C);
                        pqxx::result R = N.exec("SELECT id, marca, nome, ano, cor, preco FROM " + TABELA_CARROS + ";");

                        if (R.empty()) {
                            std::cout << "Nenhum carro cadastrado." << std::endl;
                        } else {
                            // Itera sobre os resultados
                            for (pqxx::row row : R) {
                                int id = row["id"].as<int>();
                                std::string marca = row["marca"].as<std::string>();
                                std::string nome = row["nome"].as<std::string>();
                                int ano = row["ano"].as<int>();
                                std::string cor = row["cor"].as<std::string>();
                                double preco = row["preco"].as<double>();

                                std::cout << "ID: " << id
                                          << ", Marca: " << marca
                                          << ", Nome: " << nome
                                          << ", Ano: " << ano
                                          << ", Cor: " << cor
                                          << ", Preco: " << preco << std::endl;
                            }
                        }
                    } catch (const pqxx::sql_error& e) {
                        // CORREÇÃO: e.sqlstate()
                        std::cerr << "Erro ao consultar carros: " << e.what() << " (SQL state: " << e.sqlstate() << ")" << std::endl;
                    } catch (const pqxx::broken_connection& e) {
                        std::cerr << "Erro de conexão: " << e.what() << std::endl;
                    }
                    break;
                }
                case 3: { // Deletar carros por ID
                    int id_deletar;
                    std::cout << "Digite o ID do carro a ser deletado: ";
                    while (!(std::cin >> id_deletar)) {
                        std::cout << "Entrada inválida. Digite um número para o ID: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    try {
                        pqxx::work W(C);
                        std::string sql = "DELETE FROM " + TABELA_CARROS + " WHERE id = $1;";
                        pqxx::result R = W.exec_params(sql, id_deletar); // exec_params retorna um pqxx::result

                        if (R.affected_rows() > 0) { // Verifica o número de linhas afetadas
                            W.commit(); // Confirma a transação
                            std::cout << "Carro com ID " << id_deletar << " deletado com sucesso!" << std::endl;
                        } else {
                            W.abort(); // Aborta a transação se nenhuma linha foi afetada
                            std::cout << "Nenhum carro encontrado com o ID " << id_deletar << "." << std::endl;
                        }
                    } catch (const pqxx::sql_error& e) {
                        // CORREÇÃO: e.sqlstate()
                        std::cerr << "Erro ao deletar carro: " << e.what() << " (SQL state: " << e.sqlstate() << ")" << std::endl;
                    } catch (const pqxx::broken_connection& e) {
                        std::cerr << "Erro de conexão: " << e.what() << std::endl;
                    }
                    break;
                }
                case 0: {
                    std::cout << "Saindo..." << std::endl;
                    break;
                }
                default: {
                    std::cout << "Opção inválida. Tente novamente." << std::endl;
                    break;
                }
            }
        } while (escolha != 0);

    } catch (const pqxx::broken_connection& e) {
        // Captura erro de conexão inicial
        std::cerr << "Erro de conexão inicial com o banco de dados: " << e.what() << std::endl;
        return 1;
    } catch (const pqxx::sql_error& e) {
        // Captura outros erros SQL que podem ocorrer fora dos blocos try internos
        // CORREÇÃO: e.sqlstate()
        std::cerr << "Erro SQL: " << e.what() << " (SQL state: " << e.sqlstate() << ")" << std::endl;
        return 1;
    } catch (const std::exception& e) {
        // Captura outras exceções gerais
        std::cerr << "Erro inesperado: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}