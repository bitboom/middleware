<p align="center">
<b> ViST, Virtual Security Table &#127912 </b><br>
Query-based Universial Security API &#128196<br>
of the developer, by the developer, for the developer &#128187
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

# Architecture
<img src="https://github.sec.samsung.net/storage/user/692/files/6829c580-5a46-11ea-86d5-4091ecdfc1ea" alt="layered architecture" width="780" height="610">

# Design
## Programming Abstraction
/// XXX - Struct-based schema, query builder, query parser, virtual table 

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
Query builder API is
a functioanl API
to generate type-safed query.  
This generate the query statement
via Schema API and
check type-error
for type-safe query. 

The CRUD clause is provided at ViST v1.

```cpp
DECLARE_COLUMN(${COLUMN_INSTANCE}, ${COLUMN_NAME}, ${COLUMN_IDENTIFIER});
DECLARE_TABLE(${TABLE_INSTANCE}, ${TABLE_NAME}, ${COLUMN_INSTANCE});

%{TABLE_INSTANCE}.select(${COLUMN_INSTANCE});
%{TABLE_INSTANCE}.insert(${COLUMN_INSTANCE} = ${COLUMN_VALUE});
%{TABLE_INSTANCE}.update(${COLUMN_INSTANCE} = ${COLUMN_VALUE});
%{TABLE_INSTANCE}.remove(${COLUMN_INSTANCE});
```

### Virtual table API
Virtual table API is a functioanl API to invoke callback of virtual table.  
This execute the query statement about the virtual table
and listen the event of it.

```cpp
VirtualTable::Execute(${QUERY_STATEMENT});
VirtualTable::Listen(${TABLE}, ${HANDLER}); // NOT IMPLEMENTED YET
```

# Contacts
- Sangwan Kwon (sangwan.kwon@samsung.com)
- Jaemin Ryu (jm77.ryu@samsung.com)
