<p align="center">
<b> ViST, Virtual Security Table &#127912 </b><br>
Query-based Universial Security API
</p>

---

> Project ViST is forked from [osquery](https://osquery.io/).   

The purpose of ViST project is
**"Provide unified interface
to security software designer
and developer
via type-safe query".**
For this we must achieve two things:
1. Provide **unified interface via query**
2. Generate **type-safe query via query-builder**

We believe that ViST makes security software usable and universial.

# Terminology
### Virtual Table
SQL Query is an interface that allows access and manipulate table resources.
SQLite provides two types of table.
- (Real) Table: Provide interface to handle database file.
- [Virtual Table](https://sqlite.org/vtab.html): 
Provides interface to invoke the callback methods of object(virtual table).  

We can call any routines with query via virtual table.  

**We make virtual tables about device policy at ViST v1.**

### Type-safe Query 
> In computer science, [type safety](https://en.wikipedia.org/wiki/Type_safety)
> is the extent to
> which a programming language
> discourages or prevents type errors.

Type-safe query is what checked for type error at compile time.

Basically query statement is not type-safe.
Most of opensource projects
what generate query like
[JPA(Java)](https://github.com/spring-projects/spring-data-jpa),
[sqlpp11(C++)](https://github.com/rbock/sqlpp11)
and [sqlite_orm(C++)](https://github.com/fnc12/sqlite_orm)
do not guarantee to generate type-safe query.
But [QueryDSL(Java)](https://github.com/querydsl/querydsl)
guarantees to generate type-safe query.

**We make query-builder to generate type-safe query.**  

Refer [this](https://github.sec.samsung.net/RS7-SECIOTSW/tizen-osquery/tree/master/src/vist/query-builder)
to compare ours and other opensources.

# Main Features
To provide intuitive and robustness security framework,
ViST takes below features.
- Query-based unified interface
- Struct-based schema
- Type-safe query builder
- Runtime policy plugin

# Architecture (Layered View)
<img src="https://github.sec.samsung.net/storage/user/692/files/9badb280-20db-11ea-8c37-a314f094a3aa" alt="layered architecture" width="650" height="650">

# Design
## Programming Abstraction
- Struct-based schema
1. query builder
2. query parser
3. virtual table 

## Programming Interface
ViST provides three types of API.  
One is for data structure(schema)
and the other is for functional.

### Schema API (Header only library)
Schema API is a data structure API
to represent struct-based schema
referred by query builder API.  
This inform virtual table schema to client and make type-safe query possible.

- The schema of virtual table is correspond with struct.
- The column of schema is correspond with struct member.

#### Table schema
| Table  | Column | Type |
|---|---|---|
| ${TABLE_NAME} | ${COLUMN_NAME_1} | ${COLUMN_TYPE} |
| | ${COLUMN_NAME_2} | ${COLUMN_TYPE} |
| | ${COLUMN_NAME_3} | ${COLUMN_TYPE} |

#### Struct-based schema
```cpp
struct ${TABLE_NAME} {
    ${COLUMN_TYPE} ${COLUMN_NAME_1};
    ${COLUMN_TYPE} ${COLUMN_NAME_2};
    ${COLUMN_TYPE} ${COLUMN_NAME_3};
};
```

### Query builder API (Header only library)
Query builder API is a functioanl API to generate type-safed query.  
This generate query statement via Schema API and check type-error for type-safe query. 

The CRUD clause is provided at ViST v1.

```
DECLARE_COLUMN(${COLUMN_INSTANCE}, ${COLUMN_NAME}, ${COLUMN_IDENTIFIER});
DECLARE_TABLE(${TABLE_INSTANCE}, ${TABLE_NAME}, ${COLUMN_INSTANCE});

%{TABLE_INSTANCE}.select(${COLUMN_INSTANCE});
%{TABLE_INSTANCE}.insert(${COLUMN_INSTANCE} = ${COLUMN_VALUE});
%{TABLE_INSTANCE}.update(${COLUMN_INSTANCE} = ${COLUMN_VALUE});
%{TABLE_INSTANCE}.remove(${COLUMN_INSTANCE});
```
### Query execution API
Admin API is a functioanl API for manipulating Virtual Tables.  
This executes the query statement for the virtual table.
```cpp
   /// Registering policy admin using Admin API
   vist::Query::Execute("INSERT INTO policy_admin (name) VALUES ('testAdmin')");
   
   /// rows includes [name:testAdmin, activated:0]
   auto rows = vist::Query::Execute("SELECT * FROM policy_admin");
   
   /// Activating policy admin
   vist::Query::Execute("UPDATE policy_admin SET activated = 1 where name = 'testAdmin'");
   
   /// Excluding policy admin using Admin API
   vist::Query::Execute("DELETE FROM policy_admin WHERE name = 'testAdmin'");
```

# Contacts
- Sangwan Kwon (sangwan.kwon@samsung.com)
- Jaemin Ryu (jm77.ryu@samsung.com)
