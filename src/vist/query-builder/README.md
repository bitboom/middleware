# Query Builder to generate Type Safe Query
Guarantee type safety by preventing type error 
between sql query and sql schema at compile time.

|   | ViST | [sqlpp11](https://github.com/rbock/sqlpp11) | [sqlite_orm](https://github.com/fnc12/sqlite_orm) |
|---|---|---|---|
| **Tpye** | query builder | orm | orm |
| **Language** | c++17 | c++11 ([WIP to c++17](https://github.com/rbock/sqlpp17)) | c++14 |
| **Schema Location** | header file | header file | source file<br>(This makes hard to expose API.) |
| **Intuition** | simple | complex | simple |
| **Usability** | high | low | high |
| **Backend** | sqlite3<br>(with virtual table) | mysql<br>sqlite3<br>(without virtual table)<br>postgresql<br>odbc | sqlite3<br>(without virtual table) |
| **Feature** | basic(crud) | variety | variety |

# Sample Code
## ours
```cpp
/// header
struct User {
  int id;
  std::string name;
  int age;
  
  DECLARE_COLUMN(Id, "id", &User::id);
  DECLARE_COLUMN(Name, "name", &User::name);
  DECLARE_COLUMN(Age, "age", &User::Age);
}

DECLARE_TABLE(UserTable, "user", User::Id, User::Name, User::Age);

/// source file
UserTable.select(User::Id, User::Name, User::Age);
/// => SELECT id, name, age FROM, user
UserTable.insert(User::Id = 3, User::Name = "Tom", User::Age = 33);
/// => INSERT INTO user (id, name, age) VALUES (3, 'Tom', 33)
UserTable.update(User::Age = 36).where(User::Name == "Tom");
/// => UPDATE user SET id = 3, name = 'Tom', age = 33 WHERE name = 'Tom'
UserTable.remove().where(User::Name == "Tom");
/// => DELETE FROM user WHERE name = 'Tom'
```

## sqlpp11
```cpp
/// header
namespace TabSample_
{
  struct Alpha
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "alpha"; }
      template<typename T>
        struct _member_t
        {
          T alpha;
        };
    };
    using _value_type = sqlpp::bigint;
    struct _column_type
    {
      using _must_not_insert = std::true_type;
      using _must_not_update = std::true_type;
      using _can_be_null = std::true_type;
      using _trivial_value_is_null = std::true_type;
      using _foreign_key = decltype(TabFoo::omega);
    };
   };

  struct Beta
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "beta"; }
      template<typename T>
        struct _member_t
        {
          T beta;
        };
    };
    using _value_type = sqlpp::varchar;
    struct _column_type
    {
      using _can_be_null = std::true_type;
      using _trivial_value_is_null = std::true_type;
      using _must_not_update = std::true_type;
    };
  };
}

struct TabSample: sqlpp::table_base_t<
                  TabSample, 
                  TabSample_::Alpha, 
                  TabSample_::Beta>
{
  using _value_type = sqlpp::no_value_t;
  struct _name_t
  {
    static constexpr const char* _get_name() { return "tab_sample"; }
    template<typename T>
      struct _member_t
      {
        T tabSample;
      };
  };
  template<typename Db>
    void serialize_impl(std::ostream& os, Db& db) const
    {
      os << _name_t::_get_name();
    }
};

/// source code
const auto t = test::TabBar{};
select(all_of(t)).from(t).unconditionally());
insert_into(t).set(t.gamma = true, t.beta = "kirschauflauf");
update(t).set(t.delta = sqlpp::null).unconditionally();
remove_from(t).where(t.beta != "transparent"), printer);
```

## sqlite_orm
```cpp
/// header
struct User{
  int id;
  std::string firstName;
  std::string lastName;
  int birthDate;
  std::unique_ptr<std::string> imageUrl;
  int typeId;
};

/// source code
auto table = make_table("users",
                        make_column("id", &User::id, autoincrement(), primary_key()),
                        make_column("first_name", &User::firstName),
                        make_column("last_name", &User::lastName),
                        make_column("birth_date", &User::birthDate),
                        make_column("image_url", &User::imageUrl),
                        make_column("type_id", &User::typeId));
                        
table.select(columns(&Employee::id, &Employee::name, &Employee::salary));
table.insert(Employee{
        -1,
        "Allen",
        25,
        "Texas",
        15000.00,
});
table.update_all(set(c(&Employee::address) = "Texas", c(&Employee::salary) = 20000.00),
                 where(c(&Employee::age) < 30));

```
