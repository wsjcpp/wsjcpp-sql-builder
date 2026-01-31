#include <iostream>
#include <wsjcpp_sql_builder.h>

int main(int argc, const char* argv[]) {

    wsjcpp::SqlBuilder builder;
        builder.selectFrom("users")
        .colum("id")
        .colum("name")
        .colum("pass")
        .where()
            .equal("pass", "64d878e22cd5046d569bf826869034caf5feab24")
    ;

    std::string sql = builder.sql();
    std::cout << sql << std::endl;

    return 0;
}

