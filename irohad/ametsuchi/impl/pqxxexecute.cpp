//
// Created by Victor Drobny on 15/05/2018.
//

#include <string>
#include <pqxx/nontransaction>
#include <pqxx/result>

pqxx::result execute(pqxx::nontransaction &transaction, const std::string &statement) {
  return transaction.exec(statement);
}
