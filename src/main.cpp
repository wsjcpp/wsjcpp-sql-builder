#include <iostream>
#include <wsjcpp_sql_builder.h>

int main(int argc, const char* argv[]) {
    WsjcppSqlBuilderInsert sql("TABLE_NAME");
    sql.add("COL1", "val1"); // will be escaped
    sql.add("COL2", 1);
    // sql.add("COL3", 1.1);
    if (!sql.isValid()) {
        std::cerr << "Something wrong with query: " << sql.getErrorMessage() << std::endl;
        return -1;
    }

    std::cout << sql.getTextQuery() << std::endl;

    return 0;
}

