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
There are some opensource projects
what generate query or ORM.
In Java,
[JPA](https://github.com/spring-projects/spring-data-jpa),
do not guarantee to generate type-safe query
but [QueryDSL](https://github.com/querydsl/querydsl)
guarantees to generate type-safe query.
In C++,
both [sqlpp11](https://github.com/rbock/sqlpp11)
and [sqlite_orm](https://github.com/fnc12/sqlite_orm)
guarantee to generate type-safe query.

Refer [this](https://github.sec.samsung.net/RS7-SECIOTSW/tizen-osquery/tree/master/src/vist/query-builder)
to compare ours and other opensource projects.
