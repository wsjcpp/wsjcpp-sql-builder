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

class WsjcppSqlBuilderHelpers {
public:
  static std::string escapingStringValue(const std::string &sValue);
};



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


enum class WsjcppSqlWhereType { LOGICAL_OPERATOR, CONDITION, SUB_CONDITION };

class WsjcppSqlWhereBase {
public:
  WsjcppSqlWhereBase(WsjcppSqlWhereType type);
  WsjcppSqlWhereType type();
  virtual std::string sql() = 0;

private:
  WsjcppSqlWhereType m_type;
};

class WsjcppSqlWhereOr : public WsjcppSqlWhereBase {
public:
  WsjcppSqlWhereOr();
  virtual std::string sql() override;
};

class WsjcppSqlWhereAnd : public WsjcppSqlWhereBase {
public:
  WsjcppSqlWhereAnd();
  virtual std::string sql() override;
};

enum class WsjcppSqlWhereConditionType { NOT_EQUAL, EQUAL, MORE_THEN, LESS_THEN, LIKE };

class WsjcppSqlWhereCondition : public WsjcppSqlWhereBase {
public:
  WsjcppSqlWhereCondition(const std::string &name, WsjcppSqlWhereConditionType comparator, const std::string &value);
  WsjcppSqlWhereCondition(const std::string &name, WsjcppSqlWhereConditionType comparator, int value);
  WsjcppSqlWhereCondition(const std::string &name, WsjcppSqlWhereConditionType comparator, double value);
  WsjcppSqlWhereCondition(const std::string &name, WsjcppSqlWhereConditionType comparator, float value);
  const std::string &name();
  WsjcppSqlWhereConditionType comparator();
  const std::string &value();
  virtual std::string sql() override;
private:
  std::string m_name;
  std::string m_value;
  WsjcppSqlWhereConditionType m_comparator;
};

class WsjcppSqlSelect;

template<class T>
class WsjcppSqlWhere : public WsjcppSqlWhereBase {
public:
  WsjcppSqlWhere(WsjcppSqlWhere<T> *parent, WsjcppSqlBuilder2 *builder, T *query)
    : WsjcppSqlWhereBase(WsjcppSqlWhereType::SUB_CONDITION), m_parent(parent), m_builder(builder), m_query(query) { }

  template <typename TVal>
  WsjcppSqlWhere<T> &notEqual(const std::string &name, TVal value) {
    cond(name, WsjcppSqlWhereConditionType::NOT_EQUAL, value);
    return *this;
  }

  template <typename TVal>
  WsjcppSqlWhere<T> &equal(const std::string &name, TVal value) {
    cond(name, WsjcppSqlWhereConditionType::EQUAL, value);
    return *this;
  }

  template <typename TVal>
  WsjcppSqlWhere<T> &moreThen(const std::string &name, TVal value) {
    cond(name, WsjcppSqlWhereConditionType::MORE_THEN, value);
    return *this;
  }

  template <typename TVal>
  WsjcppSqlWhere<T> &lessThen(const std::string &name, TVal value) {
    cond(name, WsjcppSqlWhereConditionType::LESS_THEN, value);
    return *this;
  }

  WsjcppSqlWhere<T> &like(const std::string &name, const std::string &value) {
    cond(name, WsjcppSqlWhereConditionType::LIKE, value);
    return *this;
  }

  WsjcppSqlWhere<T> &or_() {
    if (
      m_conditions.size() > 0
      && m_conditions[m_conditions.size()-1]->type() == WsjcppSqlWhereType::LOGICAL_OPERATOR
    ) {
      // TODO
      // m_builder->addError("[WARNING] WsjcppSqlWhere. Last item alredy defined as logical_operator. current will be skipped.");
      return *this;
    }

    m_conditions.push_back(std::make_shared<WsjcppSqlWhereOr>());
    return *this;
  }

  WsjcppSqlWhere<T> &and_() {
    if (
      m_conditions.size() > 0
      && m_conditions[m_conditions.size()-1]->type() == WsjcppSqlWhereType::LOGICAL_OPERATOR
    ) {
      // TODO
      // m_builder->addError("[WARNING] WsjcppSqlWhere. Last item alredy defined as logical_operator. current will be skipped.");
      return *this;
    }
    m_conditions.push_back(std::make_shared<WsjcppSqlWhereAnd>());
    return *this;
  }

  WsjcppSqlWhere<T> &subCondition() {
    if (
      m_conditions.size() > 0
      && m_conditions[m_conditions.size()-1]->type() == WsjcppSqlWhereType::CONDITION
    ) {
        and_(); // default add and_
    }
    auto sub_cond = std::make_shared<WsjcppSqlWhere<T>>(this, m_builder, m_query);
    m_conditions.push_back(sub_cond);
    return *(sub_cond.get());
  }

  WsjcppSqlWhere<T> &finishSubCondition() {
    // TODO return parent
    if (m_parent != nullptr) {
      return *m_parent;
    }
    // default return current where
    // TODO warning to builder
    return *this;
  }

  T &endWhere() {
    return *m_query;
  }

  virtual std::string sql() override {
    std::string ret = "";
    for (auto item : m_conditions) {
      if (item->type() == WsjcppSqlWhereType::SUB_CONDITION) {
        ret += "(" + item->sql() + ")";
      } else {
        ret += item->sql();
      }
    }
    return ret;
  }

private:
  template <typename TVal>
  WsjcppSqlWhere<T> &cond(const std::string &name, WsjcppSqlWhereConditionType comparator, TVal value) {
    if (
      m_conditions.size() > 0
      && m_conditions[m_conditions.size()-1]->type() == WsjcppSqlWhereType::CONDITION
    ) {
        and_(); // default add and_
    }
    m_conditions.push_back(std::make_shared<WsjcppSqlWhereCondition>(name, comparator, value));
    return *this;
  }

  WsjcppSqlBuilder2 *m_builder;
  T *m_query;
  WsjcppSqlWhere<T> *m_parent;
  std::vector<std::shared_ptr<WsjcppSqlWhereBase>> m_conditions;
};

class WsjcppSqlSelect : public WsjcppSqlQuery {
public:
  WsjcppSqlSelect(const std::string &tableName, WsjcppSqlBuilder2 *builder);
  WsjcppSqlSelect &colum(const std::string &col, const std::string &col_as = "");

  WsjcppSqlWhere<WsjcppSqlSelect> &where();
  // TODO group by
  // TODO order by
  WsjcppSqlBuilder2 &compile();
  virtual std::string sql() override;

private:
  std::string m_tableName;
  WsjcppSqlBuilder2 *m_builder;
  std::shared_ptr<WsjcppSqlWhere<WsjcppSqlSelect>> m_where;
  std::vector<std::string> m_columns;
  std::map<std::string, std::string> m_columns_as;
};

class WsjcppSqlBuilder2 {
public:
  // TODO begin / end transaction can be added here

  WsjcppSqlSelect &selectFrom(const std::string &sSqlTable);
  WsjcppSqlBuilder2 &makeInsert(const std::string &sSqlTable);
  WsjcppSqlBuilder2 &makeUpdate(const std::string &sSqlTable);
  WsjcppSqlBuilder2 &makeDelete(const std::string &sSqlTable);

  bool hasErrors();
  std::string sql();
  void clear();

protected:
  friend WsjcppSqlSelect;
  friend WsjcppSqlWhere<WsjcppSqlSelect>;
  void addError(const std::string &err);

private:
  std::vector<std::string> m_errors;
  std::string m_tableName;
  WsjcppSqlBuilderType m_nSqlType;
  std::vector<std::shared_ptr<WsjcppSqlQuery>> m_queries;
};