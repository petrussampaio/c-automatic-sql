#include <stdio.h>
#include <stdlib.h>
#include "/usr/include/postgresql/libpq-fe.h"
#include <string.h>

void exibir_tabelas(PGconn *conn);
void remover_dados_tabela(PGconn *conn);
void exibir_dados_tabelas(PGconn *conn);
void criacao_tabela(PGconn *conn);
void insercao_valores(PGconn *conn);