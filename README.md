# wsjcpp-sql-builder

C++ Class-helper for build SQL queries

## How to integrate to your project

via wsjcpp:
```
$ wsjcpp install https://github.com/wsjcpp/wsjcpp-sql-builder:master
```

or include this files:

- src/wsjcpp_sql_builder.h
- src/wsjcpp_sql_builder.cpp


## How use this

Example main func:
```
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
```

Example output:
```
$ ./wsjcpp-sql-builder
INSERT INTO TABLE_NAME(COL1, COL2) VALUES ('val1', 1);
```