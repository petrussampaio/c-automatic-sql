#include "c.h"

void exibir_tabelas(PGconn *conn) 
{
    PGresult *res;

    res = PQexec(conn, "SELECT table_name FROM information_schema.TABLES WHERE table_schema = 'public';");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Ocorreu um erro na query: %s\n", PQresultErrorMessage(res));
    }
    else
    {
        int numRows = PQntuples(res); // Número de linhas retornadas pela query
        if (numRows == 0) // Se não houverem linhas, não há tabelas
        {
            printf("Não foram encontradas tabelas.\n");
        }
        else
        {
            printf("Tabelas encontradas:\n");
            for (int i = 0; i < numRows; i++)
            {
                printf("- %s\n", PQgetvalue(res, i, 0)); // Exibe o nome da tabela
            }
        }
    }
    PQclear(res);
}


void remover_dados_tabela(PGconn *conn) {
    PGresult *res;
    char tabela[20];

    printf("Digite o nome da tabela: ");
    fgets(tabela, 20, stdin);
    scanf("%[^\n]", tabela);

    // Consulta os dados existentes na tabela
    char querySelect[300];
    snprintf(querySelect, sizeof(querySelect), "SELECT * FROM %s;", tabela);
    res = PQexec(conn, querySelect);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) { // Verifica se a consulta foi bem sucedida
        printf("Erro ao consultar dados da tabela: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        return;
    }

    int numRows = PQntuples(res);
    if (numRows == 0) {
        printf("Não existem dados na tabela %s.\n", tabela);
        PQclear(res);
        return;
    }

    printf("Dados na tabela %s:\n", tabela);

    // Exibe os dados existentes na tabela
    for (int i = 0; i < numRows; i++) { // Percorre as linhas retornadas pela consulta
        for (int j = 0; j < PQnfields(res); j++) {
            printf("%s: %s\n", PQfname(res, j), PQgetvalue(res, i, j)); // Exibe o nome da coluna e o valor da célula
        }
        printf("-------------------------------\n");
    }

    printf("\nOpções de filtro:\n"); 
    printf("1. Remover todos os registros\n");
    for (int j = 0; j < PQnfields(res); j++) { // Percorre as colunas retornadas pela consulta
        printf("%d. Remover por %s\n", j + 2, PQfname(res, j));
    }
    printf("Digite sua opção: ");

    int option;
    scanf("%d", &option);

    char filtro[300];
    if (option == 1) { // Remove todos os registros
        snprintf(filtro, sizeof(filtro), "TRUE");
    } else if (option >= 2 && option <= PQnfields(res) + 1) {
        printf("Digite o valor do campo %s para filtrar: ", PQfname(res, option - 2)); // Remove por um campo específico
        fgets(filtro, 300, stdin);
        scanf("%[^\n]", filtro);
        char fieldValue[250];
        snprintf(fieldValue, sizeof(fieldValue), "'%s'", filtro); // Adiciona aspas simples ao valor do campo
        snprintf(filtro, sizeof(filtro), "%s=%s", PQfname(res, option - 2), fieldValue); // Monta o filtro
    } else {
        printf("Opção inválida.\n");
        PQclear(res);
        return;
    }

    char query[300];
    snprintf(query, sizeof(query), "DELETE FROM %s WHERE %s;", tabela, filtro); // Monta a query de remoção
    res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Erro ao remover dados da tabela: %s\n", PQresultErrorMessage(res));
    } else {
        printf("Dados removidos com sucesso da tabela %s!\n", tabela);
    }

    PQclear(res);
}




void exibir_dados_tabelas(PGconn *conn)
{
    PGresult *res;
    char tabelainfo[20];

    printf("Digite o nome da tabela para a qual você deseja obter informações: ");
    fgets(tabelainfo, 20, stdin);
    scanf("%[^\n]", tabelainfo);

    char query[300];
    snprintf(query, sizeof(query), "SELECT column_name, data_type, ordinal_position FROM information_schema.COLUMNS WHERE table_schema = 'public' AND table_name = '%s';", tabelainfo);

    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Ocorreu um erro na query: %s\n", PQresultErrorMessage(res));
    }
    else
    {
        int numRows = PQntuples(res);
        if (numRows == 0)
        {
            printf("Não foram encontradas informações para a tabela '%s'.\n", tabelainfo);
        }
        else // Exibe as informações da tabela
        {
            printf("Informações da tabela '%s':\n", tabelainfo);
            printf("------------------------------------------------------------------\n");
            printf("| %-20s | %-20s | %-20s |\n", "Column Name", "Data Type", "Ordinal Position");
            printf("------------------------------------------------------------------\n");
            for (int i = 0; i < numRows; i++)
            {
                printf("| %-20s | %-20s | %-20s |\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2)); // Exibe os dados retornados pela consulta
            }
            printf("------------------------------------------------------------------\n");
        }
    }
    PQclear(res);
}

void criacao_tabela(PGconn *conn)
{
    PGresult *res;
    char createTableQuery[500];
    char nomeTabela[50];
    char nomeCampo[50];
    char tipoCampo[50];
    char menuName[10];
    int tamanhoCampo;
    int menuOption;
    int option;

    printf("Digite o nome da tabela: ");
    fgets(nomeTabela, 50, stdin);
    scanf("%[^\n]", nomeTabela);
    printf("Quantos campos terá a tabela? ");
    scanf("%d", &menuOption);
    fgets(menuName, 10, stdin); // Para limpar o caractere de nova linha do buffer
    char query[500];
    snprintf(query, sizeof(query), "CREATE TABLE %s (", nomeTabela);

    for (int i = 1; i <= menuOption; i++)
    {
        printf("Digite o nome do campo %d: ", i);
        fgets(nomeCampo, 50, stdin);
        scanf("%[^\n]", nomeCampo);
        printf("\nOpções:\n"); // Menu de opções para o tipo do campo
            printf("1. Tipo inteiro para seu campo %d\n", i);
            printf("2. Tipo texto para seu campo %d\n", i);
            printf("3. Tipo decimal para seu campo %d\n", i);
            printf("4. Tipo data para seu campo %d\n", i);
            printf("5. Voltar a opções anteriores\n");
            printf("Digite sua opção: ");
            scanf("%d", &option);
        switch(option) // Define o tipo do campo
        {
            case 1:
                snprintf(tipoCampo, sizeof(tipoCampo), "int");
                break;
            
            case 2:
                printf("Digite o tamanho do texto do campo %d: ", i);
                scanf("%d", &tamanhoCampo);
                snprintf(tipoCampo, sizeof(tipoCampo), "varchar(%d)", tamanhoCampo); // Define o tamanho do campo
                break;
            
            case 3:
                snprintf(tipoCampo, sizeof(tipoCampo), "decimal");
                break;
            
            case 4:
                snprintf(tipoCampo, sizeof(tipoCampo), "date");
                break;
            
            case 5:
                printf("Voltando para as opções anteriores...\n"); // Volta para o comando anterior
                i--;
                continue;
            
            default:
            printf("Opção inválida.\n");
            PQclear(res);
            return;
        
        }

        // Remova os caracteres de nova linha dos nomes e tipos de campos
        strtok(nomeCampo, "\n");
        strtok(tipoCampo, "\n");

        strcat(query, nomeCampo);
        strcat(query, " ");
        strcat(query, tipoCampo);
        if (i != menuOption)
        {
            strcat(query, ", ");
        }
    }

    strcat(query, ");"); // Finaliza a query de criação da tabela
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        printf("Erro na criação da tabela: %s\n", PQresultErrorMessage(res));
    }
    else
    {
        printf("Tabela criada com sucesso!\n");
    }
}

void insercao_valores(PGconn *conn)
{
    PGresult *res;

    // Pega nome da tabela do usuário
    char tabela[20];
    printf("Digite o nome da tabela para inserir os valores: ");
    fgets(tabela, 20, stdin);
    scanf("%[^\n]", tabela);

    // Busca nomes e tipos de coluna para a tabela
    char query[300];
    snprintf(query, sizeof(query), // Monta a query
             "SELECT column_name, data_type FROM "
             "information_schema.COLUMNS WHERE table_schema = 'public' AND "
             "table_name = '%s';",
             tabela);
    res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Ocorreu um erro na query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        return;
    }

    int numColumns = PQntuples(res);
    if (numColumns == 0)
    {
        printf("Não foram encontrados campos para a tabela '%s'.\n", tabela);
        PQclear(res);
        return;
    }

    // Solicita ao usuário os valores de cada coluna
    printf("Inserindo valores na tabela '%s':\n", tabela);
    char insertQuery[500];
    snprintf(insertQuery, sizeof(insertQuery), "INSERT INTO %s (", tabela);

    char inputValues[numColumns][50]; // Array para armazenar os valores de entrada do usuário

    for (int i = 0; i < numColumns; i++)
    {
        char columnName[50];
        char columnType[50];

        strncpy(columnName, PQgetvalue(res, i, 0), sizeof(columnName)); // Copia o nome da coluna
        strncpy(columnType, PQgetvalue(res, i, 1), sizeof(columnType)); // Copia o tipo da coluna

        // Tira todos os espaços à esquerda/à direita do nome da coluna
        char *trimmedColumnName = strtok(columnName, " ");
        if (trimmedColumnName != NULL)
        {
            strncpy(columnName, trimmedColumnName, sizeof(columnName));
        }

        // Solicita ao usuário o valor de cada coluna
        printf("Digite o valor para a coluna '%s' (%s): ", columnName, columnType);

        // Lê o valor de entrada para cada coluna
        fgets(inputValues[i], sizeof(inputValues[i]), stdin); 
        scanf("%[^\n]", inputValues[i]);
        strtok(inputValues[i], "\n");

        // Concatena o nome da coluna para a query INSERT
        strcat(insertQuery, columnName);

        // Adiciona vírgula se não for a última coluna
        if (i < numColumns - 1)
        {
            strcat(insertQuery, ", ");
        }
    }

    // Completa a query INSERT com os VALUES
    strcat(insertQuery, ") VALUES (");

    for (int i = 0; i < numColumns; i++) // Concatena os valores de entrada para a query INSERT
    {
        strcat(insertQuery, "'");
        strcat(insertQuery, inputValues[i]);
        strcat(insertQuery, "'");

        // Adiciona vírgula se não for a última coluna
        if (i < numColumns - 1)
        {
            strcat(insertQuery, ", ");
        }
    }

    strcat(insertQuery, ");");
    res = PQexec(conn, insertQuery);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        printf("Erro ao inserir valores: %s\n", PQresultErrorMessage(res));
    }
    else
    {
        printf("Valores inseridos com sucesso!\n");
    }

    PQclear(res); // Limpa o resultado da query
}