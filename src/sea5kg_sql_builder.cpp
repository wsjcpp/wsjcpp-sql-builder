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
 * Official Source Code: https://github.com/sea5kg/sea5kg-sql-builder
 *
 ***********************************************************************************/

#include "sea5kg_sql_builder.h"
#include <algorithm>

namespace sea5kg {

// ---------------------------------------------------------------------
// SqlBuilderHelpers

std::string SqlBuilderHelpers::escapingStringValue(const std::string &sValue) {
  // escaping symbols NUL (ASCII 0), \n, \r, \, ', ", и Control-Z.
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

sql_query::sql_query(query_type sqlType, sql_builder *builder, const std::string &tableName)
    : m_sqlType(sqlType), m_builder(builder), m_tableName(tableName) {
}

query_type sql_query::sqlType() {
  return m_sqlType;
}

sql_builder &sql_query::builder() {
  return *m_builder;
}

sql_builder *sql_query::builderRawPtr() {
  return m_builder;
}

const std::string &sql_query::tableName() {
  return m_tableName;
}

// ---------------------------------------------------------------------
// sql_where_base

sql_where_base::sql_where_base(sql_where_type type)
    : m_type(type) {

      };

sql_where_type sql_where_base::type() {
  return m_type;
}

// ---------------------------------------------------------------------
// SqlWhereOr

SqlWhereOr::SqlWhereOr() : sql_where_base(sql_where_type::LOGICAL_OPERATOR) {
}
std::string SqlWhereOr::sql() {
  return " OR ";
};

// ---------------------------------------------------------------------
// SqlWhereAnd

SqlWhereAnd::SqlWhereAnd() : sql_where_base(sql_where_type::LOGICAL_OPERATOR) {
}
std::string SqlWhereAnd::sql() {
  return " AND ";
};

// ---------------------------------------------------------------------
// sql_where_condition

sql_where_condition::sql_where_condition(const std::string &name, sql_where_condition_type comparator,
                                     const std::string &value)
    : sql_where_base(sql_where_type::CONDITION), m_name(name), m_comparator(comparator) {
  // TODO in different databases different quotes, mssql have a column names in double quotes
  m_value = SqlBuilderHelpers::escapingStringValue(value);
}

sql_where_condition::sql_where_condition(const std::string &name, sql_where_condition_type comparator, int value)
    : sql_where_base(sql_where_type::CONDITION), m_name(name), m_comparator(comparator) {
  m_value = std::to_string(value);
}

sql_where_condition::sql_where_condition(const std::string &name, sql_where_condition_type comparator, long value)
    : sql_where_base(sql_where_type::CONDITION), m_name(name), m_comparator(comparator) {
  m_value = std::to_string(value);
}

sql_where_condition::sql_where_condition(const std::string &name, sql_where_condition_type comparator, double value)
    : sql_where_base(sql_where_type::CONDITION), m_name(name), m_comparator(comparator) {
  m_value = std::to_string(value);
}

sql_where_condition::sql_where_condition(const std::string &name, sql_where_condition_type comparator, float value)
    : sql_where_base(sql_where_type::CONDITION), m_name(name), m_comparator(comparator) {
  m_value = std::to_string(value);
}

const std::string &sql_where_condition::name() {
  return m_name;
}

sql_where_condition_type sql_where_condition::comparator() {
  return m_comparator;
}

const std::string &sql_where_condition::value() {
  return m_value;
}

std::string sql_where_condition::sql() {
  std::string ret;
  ret += m_name; // TODO validate and escaping
  switch (m_comparator) {
  case sql_where_condition_type::NOT_EQUAL:
    ret += " <> ";
    break;
  case sql_where_condition_type::EQUAL:
    ret += " = ";
    break;
  case sql_where_condition_type::MORE_THEN:
    ret += " > ";
    break;
  case sql_where_condition_type::LESS_THEN:
    ret += " < ";
    break;
  case sql_where_condition_type::LIKE:
    ret += " LIKE ";
    break;
  default:
    ret += " unknown_operator ";
    break;
  }
  ret += m_value;
  return ret;
}

// ---------------------------------------------------------------------
// sql_select

sql_select::sql_select(const std::string &tableName, sql_builder *builder)
    : sql_query(query_type::SELECT, builder, tableName) {
  // TODO multitype table names with AS
}

sql_select &sql_select::column(const std::string &col, const std::string &col_as) {
  auto it = std::find(m_columns.begin(), m_columns.end(), col);
  if (it != m_columns.end()) {
    builder().addError("Column '" + col + "' already added to select");
  } else {
    m_columns.push_back(col);
    m_columns_as[col] = col_as;
  }
  return *this;
}

sql_where<sql_select> &sql_select::where() {
  if (!m_where) {
    m_where = std::make_shared<sql_where<sql_select>>(nullptr, builderRawPtr(), this);
  }
  return *(m_where.get());
}

std::string sql_select::sql() {
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
// sql_insert

sql_insert::sql_insert(const std::string &tableName, sql_builder *builder)
    : sql_query(query_type::INSERT, builder, tableName) {
}

sql_insert &sql_insert::column(const std::string &col) {
  m_columns.push_back(col);
  return *this;
}

sql_insert &sql_insert::addColumns(const std::vector<std::string> &cols) {
  for (auto col : cols) {
    m_columns.push_back(col);
  }
  return *this;
}

sql_insert &sql_insert::clearValues() {
  m_values.clear();
  return *this;
}

sql_insert &sql_insert::val(const std::string &val) {
  m_values.push_back(SqlBuilderHelpers::escapingStringValue(val));
  return *this;
}

sql_insert &sql_insert::val(int val) {
  m_values.push_back(std::to_string(val));
  return *this;
}

sql_insert &sql_insert::val(long val) {
  m_values.push_back(std::to_string(val));
  return *this;
}

sql_insert &sql_insert::val(float val) {
  m_values.push_back(std::to_string(val));
  return *this;
}

sql_insert &sql_insert::val(double val) {
  m_values.push_back(std::to_string(val));
  return *this;
}

std::string sql_insert::sql() {
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
// sql_update

sql_update::sql_update(const std::string &tableName, sql_builder *builder)
    : sql_query(query_type::UPDATE, builder, tableName) {
}

sql_update &sql_update::set(const std::string &name, const std::string &val) {
  return setValue(name, SqlBuilderHelpers::escapingStringValue(val));
}

sql_update &sql_update::set(const std::string &name, int val) {
  return setValue(name, std::to_string(val));
}

sql_update &sql_update::set(const std::string &name, long val) {
  return setValue(name, std::to_string(val));
}

sql_update &sql_update::set(const std::string &name, float val) {
  return setValue(name, std::to_string(val));
}

sql_update &sql_update::set(const std::string &name, double val) {
  return setValue(name, std::to_string(val));
}

sql_update &sql_update::setValue(const std::string &name, const std::string &val) {
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

sql_where<sql_update> &sql_update::where() {
  if (!m_where) {
    m_where = std::make_shared<sql_where<sql_update>>(nullptr, builderRawPtr(), this);
  }
  return *(m_where.get());
}

std::string sql_update::sql() {
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
// sql_delete

sql_delete::sql_delete(const std::string &tableName, sql_builder *builder)
    : sql_query(query_type::DELETE, builder, tableName) {
}

sql_where<sql_delete> &sql_delete::where() {
  if (!m_where) {
    m_where = std::make_shared<sql_where<sql_delete>>(nullptr, builderRawPtr(), this);
  }
  return *(m_where.get());
}

std::string sql_delete::sql() {
  std::string ret = "DELETE FROM " + tableName();

  if (m_where) {
    ret += " WHERE " + m_where->sql();
  }

  return ret;
};

// ---------------------------------------------------------------------
// sql_builder

sql_builder::sql_builder(sql_builder_for_database dbType) : m_dbType(dbType) {
}

sql_select &sql_builder::selectFrom(const std::string &tableName) {
  m_queries.push_back(std::make_shared<sql_select>(tableName, this));
  // TODO check must be select last one;
  return *(sql_select *)(m_queries[m_queries.size() - 1].get());
}

sql_insert &sql_builder::insertInto(const std::string &tableName) {
  m_queries.push_back(std::make_shared<sql_insert>(tableName, this));
  return *(sql_insert *)(m_queries[m_queries.size() - 1].get());
}

sql_insert &sql_builder::findInsertOrCreate(const std::string &tableName) {
  for (auto query : m_queries) {
    if (query->sqlType() == query_type::INSERT && query->tableName() == tableName) {
      return *(sql_insert *)(query.get());
    }
  }
  return insertInto(tableName);
}

sql_update &sql_builder::update(const std::string &tableName) {
  m_queries.push_back(std::make_shared<sql_update>(tableName, this));
  return *(sql_update *)(m_queries[m_queries.size() - 1].get());
}

sql_update &sql_builder::findUpdateOrCreate(const std::string &tableName) {
  for (auto query : m_queries) {
    if (query->sqlType() == query_type::UPDATE && query->tableName() == tableName) {
      return *(sql_update *)(query.get());
    }
  }
  return update(tableName);
}

sql_delete &sql_builder::deleteFrom(const std::string &tableName) {
  m_queries.push_back(std::make_shared<sql_delete>(tableName, this));
  return *(sql_delete *)(m_queries[m_queries.size() - 1].get());
}

sql_delete &sql_builder::findDeleteOrCreate(const std::string &tableName) {
  for (auto query : m_queries) {
    if (query->sqlType() == query_type::DELETE && query->tableName() == tableName) {
      return *(sql_delete *)(query.get());
    }
  }
  return deleteFrom(tableName);
}

void sql_builder::clear() {
  m_queries.clear();
}

bool sql_builder::hasErrors() {
  return m_errors.size() > 0;
}

void sql_builder::addError(const std::string &err) {
  m_errors.push_back(err);
}

std::string sql_builder::sql() {
  std::string ret = "";
  for (auto query : m_queries) {
    if (ret.size() > 0) {
      ret += "\n";
    }
    ret += query->sql();
  }
  return ret;
}

void sql_builder::setDatabaseType(sql_builder_for_database dbType) {
  m_dbType = dbType;
}

sql_builder_for_database sql_builder::databaseType() {
  return m_dbType;
}

} // namespace sea5kg
