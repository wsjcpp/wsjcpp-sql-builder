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

#include "wsjcpp_sql_builder.h"
#include <algorithm>


// ---------------------------------------------------------------------
// WsjcppSqlBuilderHelpers

std::string WsjcppSqlBuilderHelpers::escapingStringValue(const std::string &sValue) {
  // escaping simbols NUL (ASCII 0), \n, \r, \, ', ", и Control-Z.
  std::string sResult;
  sResult.reserve(sValue.size() * 2);
  sResult.push_back('\'');
  for (int i = 0; i < sValue.size(); i++) {
    char c = sValue[i];
    if (c == '\n') {
      sResult.push_back('\\');
      sResult.push_back('n');
    } else if (c == '\r') {
      sResult.push_back('\\');
      sResult.push_back('r');
    } else if (c == '\\' || c == '"') {
      sResult.push_back('\\');
      sResult.push_back(c);
    } else if (c == '\'') {
      sResult.push_back('\'');
      sResult.push_back(c);
    } else if (c == 0) {
      sResult.push_back('\\');
      sResult.push_back('0');
    } else {
      sResult.push_back(c);
    }
  }
  sResult.push_back('\'');
  return sResult;
}


WsjcppSqlQuery::WsjcppSqlQuery(WsjcppSqlBuilderType sqlType, const std::string &tableName)
  : m_sqlType(sqlType), m_tableName(tableName) {

}

WsjcppSqlBuilderType WsjcppSqlQuery::sqlType() {
  return m_sqlType;
}

const std::string &WsjcppSqlQuery::tableName() {
  return m_tableName;
}

// ---------------------------------------------------------------------
// WsjcppSqlWhereBase

WsjcppSqlWhereBase::WsjcppSqlWhereBase(WsjcppSqlWhereType type) : m_type(type) {

};

WsjcppSqlWhereType WsjcppSqlWhereBase::type() {
  return m_type;
}

// ---------------------------------------------------------------------
// WsjcppSqlWhereOr

WsjcppSqlWhereOr::WsjcppSqlWhereOr() : WsjcppSqlWhereBase(WsjcppSqlWhereType::LOGICAL_OPERATOR) { }
std::string WsjcppSqlWhereOr::sql() { return " OR "; };

// ---------------------------------------------------------------------
// WsjcppSqlWhereAnd

WsjcppSqlWhereAnd::WsjcppSqlWhereAnd() : WsjcppSqlWhereBase(WsjcppSqlWhereType::LOGICAL_OPERATOR) { }
std::string WsjcppSqlWhereAnd::sql() { return " AND "; };

// ---------------------------------------------------------------------
// WsjcppSqlWhereCondition

WsjcppSqlWhereCondition::WsjcppSqlWhereCondition(
  const std::string &name,
  WsjcppSqlWhereConditionType comparator,
  const std::string &value
) : WsjcppSqlWhereBase(WsjcppSqlWhereType::CONDITION), m_name(name), m_comparator(comparator) {
  // TODO in different databases different quotes, mssql have a column names in double quotes
  m_value = WsjcppSqlBuilderHelpers::escapingStringValue(value);
}

WsjcppSqlWhereCondition::WsjcppSqlWhereCondition(
  const std::string &name,
  WsjcppSqlWhereConditionType comparator,
  int value
) : WsjcppSqlWhereBase(WsjcppSqlWhereType::CONDITION), m_name(name), m_comparator(comparator) {
  m_value = std::to_string(value);
}

WsjcppSqlWhereCondition::WsjcppSqlWhereCondition(
  const std::string &name,
  WsjcppSqlWhereConditionType comparator,
  double value
) : WsjcppSqlWhereBase(WsjcppSqlWhereType::CONDITION), m_name(name), m_comparator(comparator) {
  m_value = std::to_string(value);
}

WsjcppSqlWhereCondition::WsjcppSqlWhereCondition(
  const std::string &name,
  WsjcppSqlWhereConditionType comparator,
  float value
) : WsjcppSqlWhereBase(WsjcppSqlWhereType::CONDITION), m_name(name), m_comparator(comparator) {
  m_value = std::to_string(value);
}

const std::string &WsjcppSqlWhereCondition::name() {
  return m_name;
}

WsjcppSqlWhereConditionType WsjcppSqlWhereCondition::comparator() {
  return m_comparator;
}

const std::string &WsjcppSqlWhereCondition::value() {
  return m_value;
}

std::string WsjcppSqlWhereCondition::sql() {
  std::string ret;
  ret += m_name; // TODO validate and escaping
  switch (m_comparator) {
    case WsjcppSqlWhereConditionType::NOT_EQUAL:
      ret += " <> ";
      break;
    case WsjcppSqlWhereConditionType::EQUAL:
      ret += " = ";
      break;
    case WsjcppSqlWhereConditionType::MORE_THEN:
      ret += " > ";
      break;
    case WsjcppSqlWhereConditionType::LESS_THEN:
      ret += " < ";
      break;
    case WsjcppSqlWhereConditionType::LIKE:
      ret += " LIKE ";
      break;
    default:
      ret += " unknwon_operator ";
      break;
  }
  ret += m_value;
  return ret;
}

// ---------------------------------------------------------------------
// WsjcppSqlBuilderUpdate

WsjcppSqlSelect::WsjcppSqlSelect(const std::string &tableName, WsjcppSqlBuilder *builder)
: WsjcppSqlQuery(WsjcppSqlBuilderType::SELECT, tableName) {
  // TODO multitype table names with AS
  m_tableName = tableName;
  m_builder = builder;
}

WsjcppSqlSelect &WsjcppSqlSelect::colum(const std::string &col, const std::string &col_as) {
  auto it = std::find(m_columns.begin(), m_columns.end(), col);
  if (it != m_columns.end()) {
    m_builder->addError("Column '" + col + "' already added to select");
  } else {
    m_columns.push_back(col);
    m_columns_as[col] = col_as;
  }
  return *this;
}

WsjcppSqlWhere<WsjcppSqlSelect> &WsjcppSqlSelect::where() {
  if (!m_where) {
    m_where = std::make_shared<WsjcppSqlWhere<WsjcppSqlSelect>>(nullptr, m_builder, this);
  }

  return *(m_where.get());
}

WsjcppSqlBuilder &WsjcppSqlSelect::compile() {
  return *m_builder;
}

std::string WsjcppSqlSelect::sql() {
  std::string ret = "SELECT ";
  // TODO TOP OR LIMIT for different databases

  if (m_columns.size() == 0) {
    ret += "*";
  } else {
    bool first = true;
    for (auto col : m_columns) {
      if (!first) {
        ret += ", ";
      }
      ret += col;
      if (m_columns_as[col] != "") {
        ret += " AS " + m_columns_as[col];
      }
      first = false;
    }
    ret += " FROM ";
    ret += m_tableName;
  }

  if (m_where) {
    ret += " WHERE " + m_where->sql();
  }

  // TODO where
  // TODO group by
  // TODO order by
  return ret;
}

// ---------------------------------------------------------------------
// WsjcppSqlBuilder

WsjcppSqlSelect &WsjcppSqlBuilder::selectFrom(const std::string &tableName) {
  m_tableName = tableName;
  m_nSqlType = WsjcppSqlBuilderType::SELECT;
  m_queries.push_back(std::make_shared<WsjcppSqlSelect>(m_tableName, this));
  // TODO check must be select last one;
  return *(WsjcppSqlSelect *)(m_queries[m_queries.size() -1].get());
}

WsjcppSqlBuilder &WsjcppSqlBuilder::insertInto(const std::string &tableName) {
  m_tableName = tableName;
  m_nSqlType = WsjcppSqlBuilderType::INSERT;
  return *this;
}

// WsjcppSqlBuilder &WsjcppSqlBuilder::makeUpdate(const std::string &tableName) {
//   m_tableName = tableName;
//   m_nSqlType = WsjcppSqlBuilderType::UPDATE;
//   return *this;
// }

// WsjcppSqlBuilder &WsjcppSqlBuilder::makeDelete(const std::string &tableName) {
//   m_tableName = tableName;
//   m_nSqlType = WsjcppSqlBuilderType::DELETE;
//   return *this;
// }

bool WsjcppSqlBuilder::hasErrors() {
  return m_errors.size() > 0;
}

void WsjcppSqlBuilder::addError(const std::string &err) {
  m_errors.push_back(err);
}

std::string WsjcppSqlBuilder::sql() {
  std::string ret = "";
  for (auto query : m_queries) {
    if (ret.size() > 0) {
      ret += "\n";
    }
    ret += query->sql();
  }
  return ret;
}

void WsjcppSqlBuilder::clear() {
  m_queries.clear();
}