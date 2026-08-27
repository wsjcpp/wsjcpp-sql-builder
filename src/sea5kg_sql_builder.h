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

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sea5kg {

enum class query_type {
  SELECT,
  INSERT,
  UPDATE,
  DELETE,
};

enum class sql_where_type {
  LOGICAL_OPERATOR,
  CONDITION,
  SUB_CONDITION,
};

enum class sql_where_condition_type {
  NOT_EQUAL,
  EQUAL,
  MORE_THEN,
  LESS_THEN,
  LIKE,
};

enum class sql_builder_for_database {
  SQLITE3,
};

class SqlBuilderHelpers {
public:
  static std::string escapingStringValue(const std::string &sValue);
};

class sql_builder;
class sql_query;
class sql_insert;
class sql_update;
class sql_select;
class sql_delete;
template <class T> class sql_where;

class ISqlBuilder {
public:
  virtual bool hasErrors() = 0;
  virtual std::string sql() = 0;
  virtual void setDatabaseType(sql_builder_for_database dbType) = 0;
  virtual sql_builder_for_database databaseType() = 0;

protected:
  friend sql_where<sql_insert>;
  friend sql_where<sql_update>;
  friend sql_where<sql_delete>;
  friend sql_where<sql_select>;
  friend sql_query;
  virtual void addError(const std::string &err) = 0;
};

class sql_query {
public:
  sql_query(query_type sqlType, sql_builder *builder, const std::string &tableName);
  query_type sqlType();
  sql_builder &builder();
  sql_builder *builderRawPtr();
  const std::string &tableName();
  virtual std::string sql() = 0;

private:
  query_type m_sqlType;
  std::string m_tableName;
  sql_builder *m_builder;
};

class sql_where_base {
public:
  sql_where_base(sql_where_type type);
  sql_where_type type();
  virtual std::string sql() = 0;

private:
  sql_where_type m_type;
};

class SqlWhereOr : public sql_where_base {
public:
  SqlWhereOr();
  virtual std::string sql() override;
};

class SqlWhereAnd : public sql_where_base {
public:
  SqlWhereAnd();
  virtual std::string sql() override;
};

class sql_where_condition : public sql_where_base {
public:
  sql_where_condition(const std::string &name, sql_where_condition_type comparator, const std::string &value);
  sql_where_condition(const std::string &name, sql_where_condition_type comparator, int value);
  sql_where_condition(const std::string &name, sql_where_condition_type comparator, long value);
  sql_where_condition(const std::string &name, sql_where_condition_type comparator, double value);
  sql_where_condition(const std::string &name, sql_where_condition_type comparator, float value);
  const std::string &name();
  sql_where_condition_type comparator();
  const std::string &value();
  virtual std::string sql() override;

private:
  std::string m_name;
  std::string m_value;
  sql_where_condition_type m_comparator;
};

class sql_select;

template <class T> class sql_where : public sql_where_base {
public:
  sql_where(sql_where<T> *parent, sql_builder *builder, T *query)
      : sql_where_base(sql_where_type::SUB_CONDITION), m_parent(parent), m_builder(builder), m_query(query) {
  }

  template <typename TVal> sql_where<T> &notEqual(const std::string &name, TVal value) {
    cond(name, sql_where_condition_type::NOT_EQUAL, value);
    return *this;
  }

  template <typename TVal> sql_where<T> &equal(const std::string &name, TVal value) {
    cond(name, sql_where_condition_type::EQUAL, value);
    return *this;
  }

  template <typename TVal> sql_where<T> &moreThen(const std::string &name, TVal value) {
    cond(name, sql_where_condition_type::MORE_THEN, value);
    return *this;
  }

  template <typename TVal> sql_where<T> &lessThen(const std::string &name, TVal value) {
    cond(name, sql_where_condition_type::LESS_THEN, value);
    return *this;
  }

  sql_where<T> &like(const std::string &name, const std::string &value) {
    cond(name, sql_where_condition_type::LIKE, value);
    return *this;
  }

  sql_where<T> &or_() {
    if (m_conditions.size() > 0 && m_conditions[m_conditions.size() - 1]->type() == sql_where_type::LOGICAL_OPERATOR) {
      addError("[WARNING] sql_where. Last item already defined as logical_operator. current will be skipped.");
      return *this;
    }

    m_conditions.push_back(std::make_shared<SqlWhereOr>());
    return *this;
  }

  sql_where<T> &and_() {
    if (m_conditions.size() > 0 && m_conditions[m_conditions.size() - 1]->type() == sql_where_type::LOGICAL_OPERATOR) {
      addError("[WARNING] sql_where. Last item already defined as logical_operator. current will be skipped.");
      return *this;
    }
    m_conditions.push_back(std::make_shared<SqlWhereAnd>());
    return *this;
  }

  sql_where<T> &subCondition() {
    if (m_conditions.size() > 0 && m_conditions[m_conditions.size() - 1]->type() == sql_where_type::CONDITION) {
      and_(); // default add and_
    }
    auto sub_cond = std::make_shared<sql_where<T>>(this, m_builder, m_query);
    m_conditions.push_back(sub_cond);
    return *(sub_cond.get());
  }

  sql_where<T> &finishSubCondition() {
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
      if (item->type() == sql_where_type::SUB_CONDITION) {
        ret += "(" + item->sql() + ")";
      } else {
        ret += item->sql();
      }
    }
    return ret;
  }

private:
  template <typename TVal> sql_where<T> &cond(const std::string &name, sql_where_condition_type comparator, TVal value) {
    if (m_conditions.size() > 0 && m_conditions[m_conditions.size() - 1]->type() == sql_where_type::CONDITION) {
      and_(); // default add and_
    }
    m_conditions.push_back(std::make_shared<sql_where_condition>(name, comparator, value));
    return *this;
  }

  void addError(const std::string &err) {
    ((ISqlBuilder *)m_builder)->addError(err);
  }

  sql_builder *m_builder;
  T *m_query;
  sql_where<T> *m_parent;
  std::vector<std::shared_ptr<sql_where_base>> m_conditions;
};

class sql_select : public sql_query {
public:
  sql_select(const std::string &tableName, sql_builder *builder);
  sql_select &column(const std::string &col, const std::string &col_as = "");

  sql_where<sql_select> &where();
  // TODO group by
  // TODO order by
  virtual std::string sql() override;

private:
  std::shared_ptr<sql_where<sql_select>> m_where;
  std::vector<std::string> m_columns;
  std::map<std::string, std::string> m_columns_as;
};

class sql_insert : public sql_query {
public:
  sql_insert(const std::string &tableName, sql_builder *builder);
  sql_insert &column(const std::string &col);
  sql_insert &addColumns(const std::vector<std::string> &cols);
  sql_insert &clearValues();

  sql_insert &val(const std::string &val);
  sql_insert &val(int val);
  sql_insert &val(long val);
  sql_insert &val(float val);
  sql_insert &val(double val);

  virtual std::string sql() override;

private:
  std::vector<std::string> m_columns;
  std::vector<std::string> m_values;
};

class sql_update : public sql_query {
public:
  sql_update(const std::string &tableName, sql_builder *builder);

  sql_update &set(const std::string &name, const std::string &val);
  sql_update &set(const std::string &name, int val);
  sql_update &set(const std::string &name, long val);
  sql_update &set(const std::string &name, float val);
  sql_update &set(const std::string &name, double val);

  sql_where<sql_update> &where();

  virtual std::string sql() override;

private:
  sql_update &setValue(const std::string &name, const std::string &val);

  std::shared_ptr<sql_where<sql_update>> m_where;
  std::vector<std::string> m_columns;
  std::map<std::string, std::string> m_values;
};

class sql_delete : public sql_query {
public:
  sql_delete(const std::string &tableName, sql_builder *builder);
  sql_where<sql_delete> &where();
  virtual std::string sql() override;

private:
  std::shared_ptr<sql_where<sql_delete>> m_where;
};

class sql_builder : public ISqlBuilder {
public:
  sql_builder(sql_builder_for_database dbType = sql_builder_for_database::SQLITE3);

  // TODO begin / end transaction can be added here

  sql_select &selectFrom(const std::string &tableName);
  sql_insert &insertInto(const std::string &tableName);
  sql_insert &findInsertOrCreate(const std::string &tableName);
  sql_update &update(const std::string &tableName);
  sql_update &findUpdateOrCreate(const std::string &tableName);
  sql_delete &deleteFrom(const std::string &sSqlTable);
  sql_delete &findDeleteOrCreate(const std::string &tableName);

  void clear();

  virtual bool hasErrors() override;
  virtual std::string sql() override;

  virtual void setDatabaseType(sql_builder_for_database dbType) override;
  virtual sql_builder_for_database databaseType() override;

protected:
  friend sql_select;
  friend sql_insert;
  friend sql_update;
  friend sql_where<sql_select>;
  virtual void addError(const std::string &err) override;

private:
  std::vector<std::string> m_errors;
  std::vector<std::shared_ptr<sql_query>> m_queries;
  sql_builder_for_database m_dbType;
};

} // namespace sea5kg