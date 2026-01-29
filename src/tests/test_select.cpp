/**********************************************************************************
 * MIT License
 *
 * Copyright (c) 2025-2026 Evgenii Sopov <mrseakg@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Official Source Code: https://github.com/wsjcpp/wsjcpp-sql-builder
 *
 ***********************************************************************************/

#include <iostream>
#include <wsjcpp_sql_builder.h>

int main() {
  WsjcppSqlBuilder2 builder;
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
      .lessThen("col4", "...")
    .endWhere() // need only for groupBy havingBy and etc
  ;
  if (builder.hasErrors()) {
    return -1;
  }
  std::string sqlQuery = builder.sql();
  std::string sqlQueryExpected =
    "SELECT col1, col2 AS c3, col3, col4 "
    "FROM table1 "
    "WHERE col1 = \"1\" OR col2 <> \"2\" OR (c3 = \"4\" AND col2 = \"5\") OR col4 < \"...\"";
  if (sqlQuery != sqlQueryExpected) {
    std::cerr
      << "Expected:" << std::endl
      << "   " << sqlQueryExpected << std::endl
      << ", but got:" << std::endl
      << "   " << sqlQuery << std::endl
    ;
    return -1;
  }

  builder.clear();
  sqlQuery = builder.sql();
  if (sqlQuery != "") {
    std::cerr
      << "Expected empty, but got: " << std::endl
      << "   " << sqlQuery << std::endl
    ;
    return -1;
  }

  return 0;
}