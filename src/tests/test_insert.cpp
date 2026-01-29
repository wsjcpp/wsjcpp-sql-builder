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
    WsjcppSqlBuilder builder;
    builder.insertInto("table2")
        .colum("col1")
        .addColums({"col2", "col3"})
        .val("val1")
        .val(1)
        .val(2.0)
    ;

    if (builder.hasErrors()) {
        std::cerr << "Select builder has some errors" << std::endl;
        return -1;
    }
    std::string sqlQuery = builder.sql();
    std::string sqlQueryExpected = "INSERT INTO table2(col1, col2, col3) VALUES('val1', 1, 2.000000)";
    if (sqlQuery != sqlQueryExpected) {
        std::cerr
            << "Expected:" << std::endl
            << "   {" << sqlQueryExpected << "}" << std::endl
            << ", but got:" << std::endl
            << "   {" << sqlQuery << "}" << std::endl
        ;
        return -1;
    }

    builder.findInsertOrCreate("table2")
        .clearValues()
        .val("val2")
        .val(2)
        .val(10.0)
    ;

    sqlQuery = builder.sql();
    sqlQueryExpected = "INSERT INTO table2(col1, col2, col3) VALUES('val2', 2, 10.000000)";
    if (sqlQuery != sqlQueryExpected) {
        std::cerr
            << "Expected:" << std::endl
            << "   {" << sqlQueryExpected << "}" << std::endl
            << ", but got:" << std::endl
            << "   {" << sqlQuery << "}" << std::endl
        ;
        return -1;
    }

    return 0;
}