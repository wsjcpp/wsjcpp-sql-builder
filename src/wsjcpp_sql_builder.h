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

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>



enum class WsjcppSqlBuilderType { SELECT, INSERT, UPDATE, DELETE };

class WsjcppSqlQuery {
public:
  WsjcppSqlQuery(WsjcppSqlBuilderType nSqlType, const std::string &sSqlTable);
  bool sel(const std::string &sColumnName);
  bool add(const std::string &sColumnName, const std::string &sValue);
  bool add(const std::string &sColumnName, int nValue);
  bool add(const std::string &sColumnName, long nValue);
  bool where(const std::string &sColumnName, const std::string &sValue);
  bool where(const std::string &sColumnName, int nValue);
  bool where(const std::string &sColumnName, long nValue);

  std::string getTextQuery();
  bool isValid();
  std::string getErrorMessage();

  virtual std::string sql() { return ""; }; // TODO = 0;

private:
  std::string prepareStringValue(const std::string &sValue);
  bool checkName(const std::string &sColumnName);
  WsjcppSqlBuilderType m_nSqlType;
  std::string m_sSqlTable;
  std::string m_sErrorMessage;
  bool m_bValid;

  // query parts
  std::string m_sSqlQuery0;
  std::string m_sSqlQuery1;
  std::string m_sSqlQuery2;
  std::map<std::string, std::string> m_mapFields;
};

class WsjcppSqlBuilderSelect : public WsjcppSqlQuery {
public:
  WsjcppSqlBuilderSelect(const std::string &sSqlTable);
};

class WsjcppSqlBuilderInsert : public WsjcppSqlQuery {
public:
  WsjcppSqlBuilderInsert(const std::string &sSqlTable);
};

class WsjcppSqlBuilderUpdate : public WsjcppSqlQuery {
public:
  WsjcppSqlBuilderUpdate(const std::string &sSqlTable);
};

class WsjcppSqlBuilder2;

class WsjcppSqlWhere {
public:
  WsjcppSqlWhere();
private:

};


class WsjcppSqlSelect : public WsjcppSqlQuery {
public:
  WsjcppSqlSelect(const std::string &tableName, WsjcppSqlBuilder2 *builder);
  WsjcppSqlSelect &colum(const std::string &col, const std::string &col_as = "");

  // TODO where
  // TODO group by
  // TODO order by
  WsjcppSqlBuilder2 &compile();
  virtual std::string sql() override;

private:
  std::string m_tableName;
  WsjcppSqlBuilder2 *m_builder;
  std::vector<std::string> m_columns;
  std::map<std::string, std::string> m_columns_as;
};

class WsjcppSqlBuilder2 {
public:
  WsjcppSqlSelect &selectFrom(const std::string &sSqlTable);
  WsjcppSqlBuilder2 &makeInsert(const std::string &sSqlTable);
  WsjcppSqlBuilder2 &makeUpdate(const std::string &sSqlTable);
  WsjcppSqlBuilder2 &makeDelete(const std::string &sSqlTable);

  bool hasErrors();
  std::string sql();
  void clear();

protected:
  friend WsjcppSqlSelect;
  void addError(const std::string &err);

private:
  std::vector<std::string> m_errors;
  std::string m_tableName;
  WsjcppSqlBuilderType m_nSqlType;
  std::vector<std::shared_ptr<WsjcppSqlQuery>> m_queries;
};