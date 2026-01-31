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
```cpp
#include <iostream>
#include <wsjcpp_sql_builder.h>

int main(int argc, const char* argv[]) {
  wsjcpp::SqlBuilder builder;
  builder.selectFrom("table1")
  .colum("col1")
  .colum("col2", "c3")
  .colum("col3")
  .colum("col4")
  .where()
    .equal("col1", "1")
    .or_()
    .notEqual("col2", "2")
    .or_()
    .subCondition()
      .equal("c3", "4")
      // .and_() // be default must be added and
      .equal("col2", "5")
    .finishSubCondition()
    .or_()
    .lessThen("col4", 111)
    .endWhere() // need only for groupBy havingBy and etc
  ;
  std::cout << builder.sql() << std::endl;
}
```

Example output:
```
$ ./wsjcpp-sql-builder
SELECT col1, col2 AS c3, col3, col4 FROM table1 WHERE col1 = '1' OR col2 <> '2' OR (c3 = '4' AND col2 = '5') OR col4 < 111
```