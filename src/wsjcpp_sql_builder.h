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

#pragma once

#include <string>

enum class WsjcppSqlBuilderType { SELECT, INSERT, UPDATE };

class WsjcppSqlBuilder {
public:
  WsjcppSqlBuilder(WsjcppSqlBuilderType nSqlType, const std::string &sSqlTable);
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
};

class WsjcppSqlBuilderSelect : public WsjcppSqlBuilder {
public:
  WsjcppSqlBuilderSelect(const std::string &sSqlTable);
};

class WsjcppSqlBuilderInsert : public WsjcppSqlBuilder {
public:
  WsjcppSqlBuilderInsert(const std::string &sSqlTable);
};

class WsjcppSqlBuilderUpdate : public WsjcppSqlBuilder {
public:
  WsjcppSqlBuilderUpdate(const std::string &sSqlTable);
};
