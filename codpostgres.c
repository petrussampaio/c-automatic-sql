#include <stdio.h>
#include <stdlib.h>
#include "/usr/include/postgresql/libpq-fe.h"
#include <string.h>

void criacao_tabela(PGconn *conn)
{
    PGresult *res;
    char createTableQuery[500];
    printf("Digite o comando SQL para criar a tabela, ex: CREATE TABLE nome_table(id serial primary key, nome varchar(50), idade INT)\n");
    fgets(createTableQuery, 500, stdin);
    scanf("%[^\n]", createTableQuery);

    res = PQexec(conn, createTableQuery);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Erro na criação da tabela: %s\n", PQresultErrorMessage(res));
    } else {
        printf("Tabela criada com sucesso!\n");
    }

}

void insercao_valores(PGconn *conn)
{
    PGresult *res;
    char insertQuery[500];
    printf("Digite o comando SQL para inserir os valores, ex: INSERT INTO nome_table(nome, idade) VALUES ()\n");
    fgets(insertQuery, 500, stdin);
    scanf("%[^\n]", insertQuery);

    res = PQexec(conn, insertQuery);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Erro na inserção de valores: %s\n", PQresultErrorMessage(res));
    } else {
        printf("Valores inseridos com sucesso!\n");
    }
    
}

int main() {
    PGconn *conn;
    PGresult *res;

    // Estabelece a conexão com o banco de dados
    printf("Digite as informações de conexão (formato: \"dbname=postgres user=postgres password=123456 port=5432 host=localhost\"):\n");
    char conninfo[256];
    fgets(conninfo, 256, stdin);

    conn = PQconnectdb(conninfo);
    printf("Tentando se conectar ao banco de dados...\n");

    // Verifica se a conexão foi bem sucedida
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Falha na conexão: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    printf("Conexão bem sucedida!\n");

    while (1) {
        int option;
        printf("\nOpções:\n");
        printf("1. Criar tabela\n");
        printf("2. Inserir valores\n");
        printf("3. Encerrar o programa\n");
        printf("Digite sua opção: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                criacao_tabela(conn);
                break;

            case 2:
                insercao_valores(conn);
                break;

            case 3:
                printf("Encerrando o programa...\n");
                PQclear(res);
                PQfinish(conn);
                return 0;

            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    }
}
