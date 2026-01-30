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


WsjcppSqlQuery::WsjcppSqlQuery(WsjcppSqlQueryType sqlType, WsjcppSqlBuilder *builder, const std::string &tableName)
  : m_sqlType(sqlType), m_builder(builder), m_tableName(tableName) {

}

WsjcppSqlQueryType WsjcppSqlQuery::sqlType() {
  return m_sqlType;
}

WsjcppSqlBuilder &WsjcppSqlQuery::builder() {
  return *m_builder;
}

WsjcppSqlBuilder *WsjcppSqlQuery::builderRawPtr() {
  return m_builder;
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
// WsjcppSqlSelect

WsjcppSqlSelect::WsjcppSqlSelect(const std::string &tableName, WsjcppSqlBuilder *builder)
: WsjcppSqlQuery(WsjcppSqlQueryType::SELECT, builder, tableName) {
  // TODO multitype table names with AS
}

WsjcppSqlSelect &WsjcppSqlSelect::colum(const std::string &col, const std::string &col_as) {
  auto it = std::find(m_columns.begin(), m_columns.end(), col);
  if (it != m_columns.end()) {
    builder().addError("Column '" + col + "' already added to select");
  } else {
    m_columns.push_back(col);
    m_columns_as[col] = col_as;
  }
  return *this;
}

WsjcppSqlWhere<WsjcppSqlSelect> &WsjcppSqlSelect::where() {
  if (!m_where) {
    m_where = std::make_shared<WsjcppSqlWhere<WsjcppSqlSelect>>(nullptr, builderRawPtr(), this);
  }
  return *(m_where.get());
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
    ret += tableName();
  }

  if (m_where) {
    ret += " WHERE " + m_where->sql();
  }

  // TODO group by
  // TODO order by
  return ret;
}

// ---------------------------------------------------------------------
// WsjcppSqlInsert

WsjcppSqlInsert::WsjcppSqlInsert(const std::string &tableName, WsjcppSqlBuilder *builder)
: WsjcppSqlQuery(WsjcppSqlQueryType::INSERT, builder, tableName) {

}

WsjcppSqlInsert &WsjcppSqlInsert::colum(const std::string &col) {
  m_columns.push_back(col);
  return *this;
}

WsjcppSqlInsert &WsjcppSqlInsert::addColums(const std::vector<std::string> &cols) {
  for (auto col : cols) {
    m_columns.push_back(col);
  }
  return *this;
}

WsjcppSqlInsert &WsjcppSqlInsert::clearValues() {
  m_values.clear();
  return *this;
}

WsjcppSqlInsert &WsjcppSqlInsert::val(const std::string &val) {
  m_values.push_back(WsjcppSqlBuilderHelpers::escapingStringValue(val));
  return *this;
}

WsjcppSqlInsert &WsjcppSqlInsert::val(int val) {
  m_values.push_back(std::to_string(val));
  return *this;
}

WsjcppSqlInsert &WsjcppSqlInsert::val(float val) {
  m_values.push_back(std::to_string(val));
  return *this;
}

WsjcppSqlInsert &WsjcppSqlInsert::val(double val) {
  m_values.push_back(std::to_string(val));
  return *this;
}

std::string WsjcppSqlInsert::sql() {
  std::string ret = "INSERT INTO " + tableName();

  // TODO if columns is empty
  ret += "(";
  bool first = true;
  for (auto col : m_columns) {
    if (!first) {
      ret += ", ";
    }
    ret += col;
    first = false;
  }
  ret += ")";

  ret += " VALUES(";
  first = true;
  for (auto val : m_values) {
    if (!first) {
      ret += ", ";
    }
    ret += val;
    first = false;
  }
  ret += ")";

  return ret;
};

// ---------------------------------------------------------------------
// WsjcppSqlUpdate

WsjcppSqlUpdate::WsjcppSqlUpdate(const std::string &tableName, WsjcppSqlBuilder *builder)
  : WsjcppSqlQuery(WsjcppSqlQueryType::UPDATE, builder, tableName) {

}

WsjcppSqlUpdate &WsjcppSqlUpdate::set(const std::string &name, const std::string &val) {
  return setValue(name, WsjcppSqlBuilderHelpers::escapingStringValue(val));
}

WsjcppSqlUpdate &WsjcppSqlUpdate::set(const std::string &name, int val) {
  return setValue(name, std::to_string(val));
}

WsjcppSqlUpdate &WsjcppSqlUpdate::set(const std::string &name, float val) {
  return setValue(name, std::to_string(val));
}

WsjcppSqlUpdate &WsjcppSqlUpdate::set(const std::string &name, double val) {
  return setValue(name, std::to_string(val));
}

WsjcppSqlUpdate &WsjcppSqlUpdate::setValue(const std::string &name, const std::string &val) {
  auto it = std::find(m_columns.begin(), m_columns.end(), name);
  if (it != m_columns.end()) {
    m_values[name] = val;
    // builder().addError("Column '" + name + "' already added to select");
  } else {
    m_columns.push_back(name);
    m_values[name] = val;
  }
  return *this;
}

WsjcppSqlWhere<WsjcppSqlUpdate> &WsjcppSqlUpdate::where() {
  if (!m_where) {
    m_where = std::make_shared<WsjcppSqlWhere<WsjcppSqlUpdate>>(nullptr, builderRawPtr(), this);
  }
  return *(m_where.get());
}

std::string WsjcppSqlUpdate::sql() {
  std::string ret = "UPDATE " + tableName() + " SET ";

  // TODO if columns is empty
  bool first = true;
  for (auto col : m_columns) {
    if (!first) {
      ret += ", ";
    }
    ret += col + " = " + m_values[col];
    first = false;
  }

  if (m_where) {
    ret += " WHERE " + m_where->sql();
  }

  return ret;
};


// ---------------------------------------------------------------------
// WsjcppSqlDelete

WsjcppSqlDelete::WsjcppSqlDelete(const std::string &tableName, WsjcppSqlBuilder *builder)
  : WsjcppSqlQuery(WsjcppSqlQueryType::DELETE, builder, tableName) {

}

WsjcppSqlWhere<WsjcppSqlDelete> &WsjcppSqlDelete::where() {
  if (!m_where) {
    m_where = std::make_shared<WsjcppSqlWhere<WsjcppSqlDelete>>(nullptr, builderRawPtr(), this);
  }
  return *(m_where.get());
}

std::string WsjcppSqlDelete::sql() {
  std::string ret = "DELETE FROM " + tableName();

  if (m_where) {
    ret += " WHERE " + m_where->sql();
  }

  return ret;
};

// ---------------------------------------------------------------------
// WsjcppSqlBuilder

WsjcppSqlBuilder::WsjcppSqlBuilder(WsjcppSqlBuilderForDatabase dbType) : m_dbType(dbType) {

}

WsjcppSqlSelect &WsjcppSqlBuilder::selectFrom(const std::string &tableName) {
  m_queries.push_back(std::make_shared<WsjcppSqlSelect>(tableName, this));
  // TODO check must be select last one;
  return *(WsjcppSqlSelect *)(m_queries[m_queries.size() -1].get());
}

WsjcppSqlInsert &WsjcppSqlBuilder::insertInto(const std::string &tableName) {
  m_queries.push_back(std::make_shared<WsjcppSqlInsert>(tableName, this));
  return *(WsjcppSqlInsert *)(m_queries[m_queries.size() -1].get());
}

WsjcppSqlInsert &WsjcppSqlBuilder::findInsertOrCreate(const std::string &tableName) {
  for (auto query : m_queries) {
    if (query->sqlType() == WsjcppSqlQueryType::INSERT && query->tableName() == tableName) {
      return *(WsjcppSqlInsert *)(query.get());
    }
  }
  return insertInto(tableName);
}

WsjcppSqlUpdate &WsjcppSqlBuilder::update(const std::string &tableName) {
  m_queries.push_back(std::make_shared<WsjcppSqlUpdate>(tableName, this));
  return *(WsjcppSqlUpdate *)(m_queries[m_queries.size() -1].get());
}

WsjcppSqlUpdate &WsjcppSqlBuilder::findUpdateOrCreate(const std::string &tableName) {
  for (auto query : m_queries) {
    if (query->sqlType() == WsjcppSqlQueryType::UPDATE && query->tableName() == tableName) {
      return *(WsjcppSqlUpdate *)(query.get());
    }
  }
  return update(tableName);
}

WsjcppSqlDelete &WsjcppSqlBuilder::deleteFrom(const std::string &tableName) {
  m_queries.push_back(std::make_shared<WsjcppSqlDelete>(tableName, this));
  return *(WsjcppSqlDelete *)(m_queries[m_queries.size() -1].get());
}

WsjcppSqlDelete &WsjcppSqlBuilder::findDeleteOrCreate(const std::string &tableName) {
  for (auto query : m_queries) {
    if (query->sqlType() == WsjcppSqlQueryType::DELETE && query->tableName() == tableName) {
      return *(WsjcppSqlDelete *)(query.get());
    }
  }
  return deleteFrom(tableName);
}

void WsjcppSqlBuilder::clear() {
  m_queries.clear();
}

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


void WsjcppSqlBuilder::setDatabaseType(WsjcppSqlBuilderForDatabase dbType) {
  m_dbType = dbType;
}

WsjcppSqlBuilderForDatabase WsjcppSqlBuilder::databaseType() {
  return m_dbType;
}

