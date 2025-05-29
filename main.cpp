#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <limits> // Para std::numeric_limits

const std::string DB_HOST = "localhost";
const std::string DB_NAME = "postgres";
const std::string DB_USER = "postgres";
const std::string DB_PASSWORD = "root";
const std::string DB_PORT = "5432";

const std::string TABELA_PESSOAS = "teste";

std::string get_connection_string() {
    return "host=" + DB_HOST +
           " port=" + DB_PORT +
           " dbname=" + DB_NAME +
           " user=" + DB_USER +
           " password=" + DB_PASSWORD;
}

int main() {
    try {
        pqxx::connection C(get_connection_string());
        std::cout << "Conexão com o banco de dados '" << C.dbname() << "' estabelecida com sucesso!" << std::endl;

        int escolha;
        do {
            std::cout << "\n--- MENU ---" << std::endl;
            std::cout << "1. Adicionar nova pessoa" << std::endl;
            std::cout << "2. Consultar todas as pessoas" << std::endl;
            std::cout << "3. Deletar pessoa por ID" << std::endl;
            std::cout << "0. Sair" << std::endl;
            std::cout << "Escolha uma opção: ";
            std::cin >> escolha;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (escolha) {
                case 1: {
                    std::string nome;
                    int idade;
                    std::cout << "Digite o nome da pessoa: ";
                    std::getline(std::cin, nome);
                    std::cout << "Digite a idade da pessoa: ";
                    std::cin >> idade;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    pqxx::work W(C);
                    std::string sql = "INSERT INTO " + TABELA_PESSOAS + " (nome, idade) VALUES ($1, $2);";
                    W.exec_params(sql, nome, idade);
                    W.commit();
                    std::cout << "Pessoa '" << nome << "' de " << idade << " anos salva com sucesso!" << std::endl;
                    break;
                }
                case 2: {
                    std::string sql_select = "SELECT id, nome, idade FROM " + TABELA_PESSOAS + " ORDER BY id;";
                    pqxx::work W(C); 

                    pqxx::result R = W.exec(sql_select);

                    if (R.empty()) {
                        std::cout << "Nenhuma pessoa encontrada na tabela." << std::endl;
                    } else {
                        std::cout << "\n--- Pessoas Cadastradas ---" << std::endl;

                        std::cout << "ID\tNome\t\tIdade" << std::endl;
                        std::cout << "---------------------------------" << std::endl;

                        for (const auto& linha : R) {

                            int id = linha["id"].as<int>();
                            std::string nome = linha["nome"].as<std::string>();
                            int idade = linha["idade"].as<int>();

                            std::cout << id << "\t" << nome << "\t\t" << idade << std::endl;
                        }
                    }
                    break;
                }
                case 3: {
                    // delete
                    break;
                }
                case 0:
                    std::cout << "Saindo do programa!" << std::endl;
                    break;
                default:
                    std::cout << "Opção inválida. Tente novamente." << std::endl;
                    break;
            }
        } while (escolha != 0);

    } catch (const pqxx::sql_error &e) {
        std::cerr << "Erro SQL: " << e.what() << " (Query: " << e.query() << ")" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Erro geral: " << e.what() << std::endl;
    }

    return 0;
}