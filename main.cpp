#include <iostream>
#include <string>
#include <limits> // Para std::numeric_limits
#include <sqlite3.h> // Para a API C do SQLite (SQLiteC++ usa ela por baixo)
#include <SQLiteCpp/SQLiteCpp.h> // A biblioteca SQLiteC++

// Nome do arquivo do banco de dados SQLite
const std::string DB_FILE = "banco.db"; // <-- ALTERADO AQUI!

// Nome da tabela
const std::string TABELA_CARROS = "carros"; // Já estava correto

int main() {
    try {
        // Abre o banco de dados SQLite. Se o arquivo não existir, ele será criado.
        SQLite::Database db(DB_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        std::cout << "Conexão com o banco de dados '" << DB_FILE << "' estabelecida com sucesso!" << std::endl;

        // Cria a tabela 'carros' se ela não existir
        db.exec("CREATE TABLE IF NOT EXISTS carros ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "marca VARCHAR(50) NOT NULL,"
                "nome VARCHAR(100) NOT NULL,"
                "ano INT,"
                "cor VARCHAR(30),"
                "preco DECIMAL(10, 2)"
                ");");
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

                    std::cout << "Digite a marca do carro: ";
                    std::getline(std::cin, marca);
                    std::cout << "Digite o nome/modelo do carro: ";
                    std::getline(std::cin, nome);
                    std::cout << "Digite o ano do carro: ";
                    // Loop para garantir entrada numérica válida para 'ano'
                    while (!(std::cin >> ano)) {
                        std::cout << "Entrada inválida. Digite um número para o ano: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa buffer após 'ano'

                    std::cout << "Digite a cor do carro: ";
                    std::getline(std::cin, cor);
                    std::cout << "Digite o preço do carro: ";
                    // Loop para garantir entrada numérica válida para 'preco'
                    while (!(std::cin >> preco)) {
                        std::cout << "Entrada inválida. Digite um número para o preço: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa buffer após 'preco'

                    try {
                        // Prepara a consulta SQL para inserção com placeholders (?)
                        SQLite::Statement query(db, "INSERT INTO " + TABELA_CARROS + " (marca, nome, ano, cor, preco) VALUES (?, ?, ?, ?, ?);");
                        // Vincula os valores aos placeholders
                        query.bind(1, marca);
                        query.bind(2, nome);
                        query.bind(3, ano);
                        query.bind(4, cor);
                        query.bind(5, preco);
                        // Executa a consulta
                        query.exec();
                        std::cout << "Carro adicionado com sucesso!" << std::endl;
                    } catch (const SQLite::Exception& e) {
                        std::cerr << "Erro ao adicionar carro: " << e.what() << std::endl;
                    }
                    break;
                }
                case 2: { // Consultar todos os carros
                    std::cout << "\n--- CARROS CADASTRADOS ---" << std::endl;
                    try {
                        // Prepara a consulta SQL para seleção
                        SQLite::Statement query(db, "SELECT id, marca, nome, ano, cor, preco FROM " + TABELA_CARROS + ";");

                        // Itera sobre os resultados
                        while (query.executeStep()) {
                            int id = query.getColumn(0);
                            std::string marca = query.getColumn(1);
                            std::string nome = query.getColumn(2);
                            int ano = query.getColumn(3);
                            std::string cor = query.getColumn(4);
                            double preco = query.getColumn(5);

                            std::cout << "ID: " << id
                                      << ", Marca: " << marca
                                      << ", Nome: " << nome
                                      << ", Ano: " << ano
                                      << ", Cor: " << cor
                                      << ", Preco: " << preco << std::endl;
                        }
                        // Verifica se houve alguma mudança (ou seja, se leu algum registro)
                        // Note: getTotalChanges() pode não ser o melhor indicador aqui para 'nenhum registro encontrado'
                        // Uma abordagem melhor seria contar os resultados ou verificar se o primeiro executeStep() retornou true
                        // Por simplicidade, mantive o que tinha, mas se precisar de mais precisão, podemos ajustar.
                         // Para este caso, uma forma melhor seria:
                         // if (!query.hasRow()) { std::cout << "Nenhum carro cadastrado." << std::endl; }
                        // Mas 'executeStep()' já retorna false se não houver mais linhas.
                        // Então, se o loop não rodar, não há carros.
                        // Podemos adicionar um contador:
                        int count_cars = 0;
                        SQLite::Statement count_query(db, "SELECT COUNT(*) FROM " + TABELA_CARROS + ";");
                        if (count_query.executeStep()) {
                            count_cars = count_query.getColumn(0);
                        }
                        if (count_cars == 0) {
                            std::cout << "Nenhum carro cadastrado." << std::endl;
                        }

                    } catch (const SQLite::Exception& e) {
                        std::cerr << "Erro ao consultar carros: " << e.what() << std::endl;
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
                        // Prepara a consulta SQL para deleção com placeholder
                        SQLite::Statement query(db, "DELETE FROM " + TABELA_CARROS + " WHERE id = ?;");
                        query.bind(1, id_deletar);
                        int rows_affected = query.exec(); // exec() retorna o número de linhas afetadas

                        if (rows_affected > 0) {
                            std::cout << "Carro com ID " << id_deletar << " deletado com sucesso!" << std::endl;
                        } else {
                            std::cout << "Nenhum carro encontrado com o ID " << id_deletar << "." << std::endl;
                        }
                    } catch (const SQLite::Exception& e) {
                        std::cerr << "Erro ao deletar carro: " << e.what() << std::endl;
                    }
                    break;
                }
                case 0: {
                    std::cout << "Saindo..." << std::endl;
                    break;
                }
                default: {
                    std::cout << "Opção inválida. Tente novamente." << std.endl;
                    break;
                }
            }
        } while (escolha != 0);

    } catch (const SQLite::Exception& e) {
        // Captura exceções da biblioteca SQLiteC++ (ex: erro ao abrir o BD)
        std::cerr << "Erro no banco de dados: " << e.what() << std::endl;
        return 1; // Indica erro
    } catch (const std::exception& e) {
        // Captura outras exceções gerais
        std::cerr << "Erro inesperado: " << e.what() << std::endl;
        return 1;
    }

    return 0; // Indica sucesso
}