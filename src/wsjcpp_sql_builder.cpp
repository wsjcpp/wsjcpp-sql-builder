/**********************************************************************************
 * MIT License
 *
 * Copyright (c) 2025 Evgenii Sopov <mrseakg@gmail.com>
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
 ***********************************************************************************/

#include "wsjcpp_sql_builder.h"

// ---------------------------------------------------------------------
// WsjcppSqlBuilder

WsjcppSqlBuilder::WsjcppSqlBuilder(WsjcppSqlBuilderType nSqlType,
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

bool WsjcppSqlBuilder::sel(const std::string &sColumnName) {
  if (!checkName(sColumnName)) {
    return false;
  }
  m_sSqlQuery0 += sColumnName + ", ";
  return true;
}

bool WsjcppSqlBuilder::add(const std::string &sColumnName,
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

bool WsjcppSqlBuilder::add(const std::string &sColumnName, int nValue) {
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

bool WsjcppSqlBuilder::add(const std::string &sColumnName, long nValue) {
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

bool WsjcppSqlBuilder::where(const std::string &sColumnName,
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

bool WsjcppSqlBuilder::where(const std::string &sColumnName, int nValue) {
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

bool WsjcppSqlBuilder::where(const std::string &sColumnName, long nValue) {
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

std::string WsjcppSqlBuilder::getTextQuery() {
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

bool WsjcppSqlBuilder::isValid() { return m_bValid; }

std::string WsjcppSqlBuilder::getErrorMessage() { return m_sErrorMessage; }

bool WsjcppSqlBuilder::checkName(const std::string &sColumnName) {
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

std::string WsjcppSqlBuilder::prepareStringValue(const std::string &sValue) {
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
    : WsjcppSqlBuilder(WsjcppSqlBuilderType::SELECT, sSqlTable) {}

// ---------------------------------------------------------------------
// WsjcppSqlBuilderInsert

WsjcppSqlBuilderInsert::WsjcppSqlBuilderInsert(const std::string &sSqlTable)
    : WsjcppSqlBuilder(WsjcppSqlBuilderType::INSERT, sSqlTable) {}

// ---------------------------------------------------------------------
// WsjcppSqlBuilderUpdate

WsjcppSqlBuilderUpdate::WsjcppSqlBuilderUpdate(const std::string &sSqlTable)
    : WsjcppSqlBuilder(WsjcppSqlBuilderType::UPDATE, sSqlTable) {}
