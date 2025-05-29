#include<iostream>
#include<string>
#include<pqxx/pqxx>
#include<limits>

const std::string DB_HOST = "127.0.0.1";
const std::string DB_NAME = "postgres";
const std::string DB_USER = "postgres";
const std::string DB_PASSWORD = "root";
const std::string DB_PORT = "5432";

const std::string TABELA_CARROS = "carros";

std::string get_connection_string(){
    return "host=" + DB_HOST +
            "port=" + DB_PORT +
            "dbname=" + DB_NAME +
            "user=" + DB_USER +
            "password=" + DB_PASSWORD;
}

int main(){

    try{
        pqxx::connection C(get_connection_string());
        std::cout<<"Conexão com o banco de dados '"<<C.dbname()<<"' estabelecida com sucesso!"<<std::endl;

        int escolha;
        do{
            std::cout<<"\n--- MENU ---"<<std::endl;
            std::cout<<"1. Adicionar novo carro"<<std::endl;
            std::cout<<"2. Consultar todos os carros"<<std::endl;
            std::cout<<"3. Deletar carros por ID"<<std::endl;
            std::cout<<"0. Sair"<<std::endl;
            std::cout<<"Escolha uma opção: "<<std::endl;
            std::cin>>escolha;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            

        }
    }

}