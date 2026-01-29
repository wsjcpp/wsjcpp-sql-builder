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
// WsjcppSqlQuery

WsjcppSqlQuery::WsjcppSqlQuery(WsjcppSqlBuilderType nSqlType,
                                   const std::string &sSqlTable) {
  m_nSqlType = nSqlType;
  m_sSqlTable = sSqlTable;
  m_bValid = true;
  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    m_sSqlQuery0 = "SELECT ";
    m_sSqlQuery1 = " FROM " + sSqlTable;
    m_sSqlQuery2 = "";
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    m_sSqlQuery0 = "INSERT INTO " + sSqlTable + "(";
    m_sSqlQuery1 = ") VALUES (";
    m_sSqlQuery2 = ");";
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    m_sSqlQuery0 = "UPDATE " + sSqlTable + " SET ";
    m_sSqlQuery1 = " WHERE ";
  } else {
    m_sErrorMessage = "Unknown sql type";
    m_bValid = false;
  }
}

bool WsjcppSqlQuery::sel(const std::string &sColumnName) {
  if (!checkName(sColumnName)) {
    return false;
  }
  m_sSqlQuery0 += sColumnName + ", ";
  return true;
}

bool WsjcppSqlQuery::add(const std::string &sColumnName,
                           const std::string &sValue) {
  if (!checkName(sColumnName)) {
    return false;
  }
  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    m_sErrorMessage = "For select you could not use 'add'";
    m_bValid = false;
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    m_sSqlQuery0 += sColumnName + ", ";
    m_sSqlQuery1 += prepareStringValue(sValue) + ", ";
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    m_sSqlQuery0 += sColumnName + " = " + prepareStringValue(sValue);
  } else {
    m_sErrorMessage = "Unknown sql type";
    m_bValid = false;
  }

  // m_sSqlQuery = sBefore + sEscapedValue + sAfter;

  //   while (true) {
  //   /* Locate the substring to replace. */
  //   index = str.find(sName, index);
  //   if (index == std::string::npos)
  //     return false;

  //   /* Make the replacement. */
  //   str.replace(index, 3, "def");

  //   /* Advance index forward so the next iteration doesn't pick it up as
  //   well. */ index += 3;
  // }
  return true;
}

bool WsjcppSqlQuery::add(const std::string &sColumnName, int nValue) {
  if (!checkName(sColumnName)) {
    return false;
  }

  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    m_sErrorMessage = "For select you could not use 'add'";
    m_bValid = false;
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    m_sSqlQuery0 += sColumnName + ", ";
    m_sSqlQuery1 += std::to_string(nValue) + ", ";
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    m_sSqlQuery0 += sColumnName + " = " + std::to_string(nValue);
  }

  return true;
}

bool WsjcppSqlQuery::add(const std::string &sColumnName, long nValue) {
  if (!checkName(sColumnName)) {
    return false;
  }

  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    m_sErrorMessage = "For select you could not use 'add'";
    m_bValid = false;
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    m_sSqlQuery0 += sColumnName + ", ";
    m_sSqlQuery1 += std::to_string(nValue) + ", ";
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    m_sSqlQuery0 += sColumnName + " = " + std::to_string(nValue);
  }

  return true;
}

bool WsjcppSqlQuery::where(const std::string &sColumnName,
                             const std::string &sValue) {
  if (!checkName(sColumnName)) {
    return false;
  }
  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    m_sSqlQuery2 += sColumnName + " = " + prepareStringValue(sValue);
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    m_sErrorMessage = "where can be in insert";
    return false;
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    m_sSqlQuery1 += sColumnName + " = " + prepareStringValue(sValue);
  }

  return true;
}

bool WsjcppSqlQuery::where(const std::string &sColumnName, int nValue) {
  if (!checkName(sColumnName)) {
    return false;
  }
  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    m_sSqlQuery2 += sColumnName + " = " + std::to_string(nValue);
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    m_sErrorMessage = "where can be in insert";
    return false;
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    m_sSqlQuery1 += sColumnName + " = " + std::to_string(nValue);
  }
  return true;
}

bool WsjcppSqlQuery::where(const std::string &sColumnName, long nValue) {
  if (!checkName(sColumnName)) {
    return false;
  }
  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    m_sSqlQuery2 += sColumnName + " = " + std::to_string(nValue);
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    m_sErrorMessage = "where can be in insert";
    return false;
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    m_sSqlQuery1 += sColumnName + " = " + std::to_string(nValue);
  }
  return true;
}

std::string WsjcppSqlQuery::getTextQuery() {
  std::string sSqlQuery = "";
  size_t size0 = m_sSqlQuery0.size();
  size_t size1 = m_sSqlQuery1.size();
  size_t size2 = m_sSqlQuery2.size();
  if (m_nSqlType == WsjcppSqlBuilderType::SELECT) {
    // TODO refactor this to vector and join
    sSqlQuery = m_sSqlQuery0;
    if (size0 > 2 && m_sSqlQuery0[size0 - 1] == ' ' &&
        m_sSqlQuery0[size0 - 2] == ',') {
      sSqlQuery += m_sSqlQuery0.substr(0, size0 - 2);
    }
    sSqlQuery += m_sSqlQuery1;
    if (size2 > 2 && m_sSqlQuery2[size2 - 1] == ' ' &&
        m_sSqlQuery2[size2 - 2] == ',') {
      sSqlQuery += m_sSqlQuery2.substr(0, size2 - 2);
    }
  } else if (m_nSqlType == WsjcppSqlBuilderType::INSERT) {
    if (size0 > 2 && m_sSqlQuery0[size0 - 1] == ' ' &&
        m_sSqlQuery0[size0 - 2] == ',') {
      sSqlQuery += m_sSqlQuery0.substr(0, size0 - 2);
    }
    if (size1 > 2 && m_sSqlQuery1[size1 - 1] == ' ' &&
        m_sSqlQuery1[size1 - 2] == ',') {
      sSqlQuery += m_sSqlQuery1.substr(0, size1 - 2);
    }
    sSqlQuery += m_sSqlQuery2;
  } else if (m_nSqlType == WsjcppSqlBuilderType::UPDATE) {
    sSqlQuery = m_sSqlQuery0 + m_sSqlQuery1 + m_sSqlQuery2;
  }
  return sSqlQuery;
}

bool WsjcppSqlQuery::isValid() { return m_bValid; }

std::string WsjcppSqlQuery::getErrorMessage() { return m_sErrorMessage; }

bool WsjcppSqlQuery::checkName(const std::string &sColumnName) {
  if (sColumnName.size() < 2) {
    m_sErrorMessage =
        "Parameter '" + sColumnName + "' must more than 2 characters";
    m_bValid = false;
    return false;
  }
  // TODO check alphabet

  // if (sName[0] != ':') {
  //   m_sErrorMessage = "Parameter '" + sName + "' must starts with ':'";
  //   m_bValid = false;
  //   return false;
  // }
  // nIndex = m_sSqlQuery.find(sName, 0);
  // if (nIndex == std::string::npos) {
  //   m_sErrorMessage = "Not found '" + sName + "' in " + m_sSqlQuery;
  //   m_bValid = false;
  //   return false;
  // }
  return true;
}

std::string WsjcppSqlQuery::prepareStringValue(const std::string &sValue) {
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

// ---------------------------------------------------------------------
// WsjcppSqlBuilderSelect

WsjcppSqlBuilderSelect::WsjcppSqlBuilderSelect(const std::string &sSqlTable)
    : WsjcppSqlQuery(WsjcppSqlBuilderType::SELECT, sSqlTable) {}

// ---------------------------------------------------------------------
// WsjcppSqlBuilderInsert

WsjcppSqlBuilderInsert::WsjcppSqlBuilderInsert(const std::string &sSqlTable)
    : WsjcppSqlQuery(WsjcppSqlBuilderType::INSERT, sSqlTable) {}

// ---------------------------------------------------------------------
// WsjcppSqlBuilderUpdate

WsjcppSqlBuilderUpdate::WsjcppSqlBuilderUpdate(const std::string &sSqlTable)
    : WsjcppSqlQuery(WsjcppSqlBuilderType::UPDATE, sSqlTable) {}


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
)
  : WsjcppSqlWhereBase(WsjcppSqlWhereType::CONDITION), m_name(name), m_comparator(comparator), m_value(value) {

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
  ret += "\"" + m_value + "\""; // TODO validate and escaping
  return ret;
}



// ---------------------------------------------------------------------
// WsjcppSqlBuilderUpdate

WsjcppSqlSelect::WsjcppSqlSelect(const std::string &tableName, WsjcppSqlBuilder2 *builder)
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

WsjcppSqlBuilder2 &WsjcppSqlSelect::compile() {
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
// WsjcppSqlBuilder2

WsjcppSqlSelect &WsjcppSqlBuilder2::selectFrom(const std::string &tableName) {
  m_tableName = tableName;
  m_nSqlType = WsjcppSqlBuilderType::SELECT;
  m_queries.push_back(std::make_shared<WsjcppSqlSelect>(m_tableName, this));
  // TODO check must be select last one;
  return *(WsjcppSqlSelect *)(m_queries[m_queries.size() -1].get());
}

WsjcppSqlBuilder2 &WsjcppSqlBuilder2::makeInsert(const std::string &tableName) {
  m_tableName = tableName;
  m_nSqlType = WsjcppSqlBuilderType::INSERT;
  return *this;
}

WsjcppSqlBuilder2 &WsjcppSqlBuilder2::makeUpdate(const std::string &tableName) {
  m_tableName = tableName;
  m_nSqlType = WsjcppSqlBuilderType::UPDATE;
  return *this;
}

WsjcppSqlBuilder2 &WsjcppSqlBuilder2::makeDelete(const std::string &tableName) {
  m_tableName = tableName;
  m_nSqlType = WsjcppSqlBuilderType::DELETE;
  return *this;
}

bool WsjcppSqlBuilder2::hasErrors() {
  return m_errors.size() > 0;
}

void WsjcppSqlBuilder2::addError(const std::string &err) {
  m_errors.push_back(err);
}

std::string WsjcppSqlBuilder2::sql() {
  std::string ret = "";
  for (auto query : m_queries) {
    if (ret.size() > 0) {
      ret += "\n";
    }
    ret += query->sql();
  }
  return ret;
}

void WsjcppSqlBuilder2::clear() {
  m_queries.clear();
}