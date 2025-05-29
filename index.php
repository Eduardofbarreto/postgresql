<?php

$db_host = "localhost";
$db_name = "postgres";
$db_user = "postgres";
$db_password = "root"; 
$db_port = "5432";

$tabela_pessoas = "teste";

function getPdoConnection($host, $port, $dbname, $user, $password) {
    $dsn = "pgsql:host=$host;port=$port;dbname=$dbname";
    try {
        $pdo = new PDO($dsn, $user, $password);
        $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); 
        echo "Conexão com o banco de dados '{$dbname}' estabelecida com sucesso!\n";
        return $pdo;
    } catch (PDOException $e) {
        die("Erro de conexão com o banco de dados: " . $e->getMessage());
    }
}

function adicionarPessoa($pdo, $tabela_pessoas) {
    echo "Digite o nome da pessoa: ";
    $nome = trim(fgets(STDIN)); 

    while (true) {
        echo "Digite a idade da pessoa: ";
        $idade_str = trim(fgets(STDIN));
        if (filter_var($idade_str, FILTER_VALIDATE_INT) !== false) {
            $idade = (int)$idade_str;
            break;
        } else {
            echo "Idade inválida. Por favor, digite um número inteiro.\n";
        }
    }

    try {
        $sql = "INSERT INTO {$tabela_pessoas} (nome, idade) VALUES (:nome, :idade)";
        $stmt = $pdo->prepare($sql);

        $stmt->execute([':nome' => $nome, ':idade' => $idade]);

        echo "Pessoa '{$nome}' de {$idade} anos salva com sucesso!\n";
    } catch (PDOException $e) {
        echo "Erro ao adicionar pessoa: " . $e->getMessage() . "\n";
    }
}

function consultarPessoas($pdo, $tabela_pessoas) {
    try {
        $sql = "SELECT id, nome, idade FROM {$tabela_pessoas} ORDER BY id";
        $stmt = $pdo->query($sql); 

        $pessoas = $stmt->fetchAll(PDO::FETCH_ASSOC); 

        if (empty($pessoas)) {
            echo "Nenhuma pessoa encontrada na tabela.\n";
        } else {
            echo "\n--- Pessoas Cadastradas ---\n";
            printf("%-5s %-20s %-5s\n", "ID", "Nome", "Idade"); 
            echo str_repeat("-", 32) . "\n"; 
            foreach ($pessoas as $pessoa) {
                printf("%-5d %-20s %-5d\n", $pessoa['id'], $pessoa['nome'], $pessoa['idade']); 
            }
        }
    } catch (PDOException $e) {
        echo "Erro ao consultar pessoas: " . $e->getMessage() . "\n";
    }
}


function main() {
    global $db_host, $db_port, $db_name, $db_user, $db_password, $tabela_pessoas;

    $pdo = getPdoConnection($db_host, $db_port, $db_name, $db_user, $db_password);

    while (true) {
        echo "\n--- MENU ---\n";
        echo "1. Adicionar nova pessoa\n";
        echo "2. Consultar todas as pessoas\n";
        echo "0. Sair\n";
        echo "Escolha uma opção: ";

        $escolha = trim(fgets(STDIN)); 

        switch ($escolha) {
            case '1':
                adicionarPessoa($pdo, $tabela_pessoas);
                break;
            case '2':
                consultarPessoas($pdo, $tabela_pessoas);
                break;
            case '0':
                echo "Saindo do programa. Adeus!\n";
                break 2; 
            default:
                echo "Opção inválida. Tente novamente.\n";
                break;
        }
    }


}


main();

?>