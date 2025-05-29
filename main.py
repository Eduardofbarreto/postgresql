import psycopg2

DB_HOST = "localhost"
DB_NAME = "postgres"
DB_USER = "postgres"
DB_PASSWORD = "root" 
DB_PORT = "5432"

TABELA_PESSOAS = "teste"

def get_connection_string():
    """Retorna a string de conexão para o psycopg2."""
    return f"host={DB_HOST} port={DB_PORT} dbname={DB_NAME} user={DB_USER} password={DB_PASSWORD}"

def adicionar_pessoa(conn):
    """Função para adicionar uma nova pessoa ao banco de dados."""
    nome = input("Digite o nome da pessoa: ")
    while True:
        try:
            idade = int(input("Digite a idade da pessoa: "))
            break
        except ValueError:
            print("Idade inválida. Por favor, digite um número inteiro.")

    try:
        with conn.cursor() as cur:
            sql = f"INSERT INTO {TABELA_PESSOAS} (nome, idade) VALUES (%s, %s);"
            cur.execute(sql, (nome, idade))
        conn.commit() 
        print(f"Pessoa '{nome}' de {idade} anos salva com sucesso!")
    except psycopg2.Error as e:
        print(f"Erro ao adicionar pessoa: {e}")
        conn.rollback() 

def consultar_pessoas(conn):
    """Função para consultar e exibir todas as pessoas no banco de dados."""
    try:
        with conn.cursor() as cur:
            sql = f"SELECT id, nome, idade FROM {TABELA_PESSOAS} ORDER BY id;"
            cur.execute(sql)
            pessoas = cur.fetchall()

        if not pessoas:
            print("Nenhuma pessoa encontrada na tabela.")
        else:
            print("\n--- Pessoas Cadastradas ---")
            print("ID\tNome\t\tIdade")
            print("---------------------------------")
            for pessoa in pessoas:
                
                print(f"{pessoa[0]}\t{pessoa[1]}\t\t{pessoa[2]}")
    except psycopg2.Error as e:
        print(f"Erro ao consultar pessoas: {e}")

def main():
    """Função principal do programa."""
    conn = None 
    try:
        conn_string = get_connection_string()
        conn = psycopg2.connect(conn_string)
        print(f"Conexão com o banco de dados '{DB_NAME}' estabelecida com sucesso!")

        while True:
            print("\n--- MENU ---")
            print("1. Adicionar nova pessoa")
            print("2. Consultar todas as pessoas")
            print("0. Sair")
            escolha = input("Escolha uma opção: ")

            if escolha == '1':
                adicionar_pessoa(conn)
            elif escolha == '2':
                consultar_pessoas(conn)
            elif escolha == '0':
                print("Saindo do programa. Adeus!")
                break
            else:
                print("Opção inválida. Tente novamente.")

    except psycopg2.Error as e:
        print(f"Erro de conexão com o banco de dados: {e}")
    finally:
        if conn:
            conn.close() 

if __name__ == "__main__":
    main()