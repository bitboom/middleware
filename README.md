<p align="center">
<b> ViST, Virtual Security Table &#127912 </b><br>
Query-based Universial Security API
</p>

---

> Project ViST is inspired by [osquery](https://osquery.io/).   

The purpose of ViST project is
**"Provide unified interface
to security software designer
and developer
via type-safe query".**  
For this we must achieve two things:
1. Provide **unified interface via query**
2. Generate **type-safe query via query-builder**

We believe that ViST makes security software usable and universial.

## Terminology
### Virtual Table
SQL Query is an interface that allows access and manipulate table resources.
SQLite provides two types of table.
- (Real) Table: Provide interface to handle database file.
- [Virtual Table](https://sqlite.org/vtab.html): 
Provide interface to invoke the callback methods of object(virtual table).  

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

**We makes query-builder to generate type-safe query.**  

Refer [this](https://github.sec.samsung.net/RS7-SECIOTSW/tizen-osquery/tree/master/src/vist/query-builder)
to compare ours and other opensources.

# Difference with osquery
Osquery views operating system as table 
from the perspective of the system administrator. 
It provides osqueryd(daemon), osqueryi(interactive shell).  

We view operating system as table 
from the perspective of the system programmar.  
Our purpose is to provide them more compact and 
more efficient interface from this different perspective.

# ViST, Virtual Security Table
ViST(Virtual Security Table) is a security monitoring framework using SQL query.
- Provides APIs for monitoring and controlling security resources.
- Views security resources as virtual tables and manipulates them through SQL queries.
- Adopts a plug-in architecture and uses [osquery](https://osquery.io/) as the query analysis engine.

# Architecture (Layered View)
<img src="https://github.sec.samsung.net/storage/user/692/files/9badb280-20db-11ea-8c37-a314f094a3aa" alt="layered architecture" width="650" height="650">

# Programming Abstraction
ViST provides three types of API.  
One is for data structure and the other is for functional.

## Schema API
Schema API represents the data structure of Virtua Tables.  
This is referred to by Client API and Admin API.
```cpp
/// policy schema API
template <typename T>
struct Policy {
  std::string name;
  T value;
};

/// process schema API
struct Processes {
  long long int pid;
  std::string name;
  std::string path;
  std::string cmdline;
  long long int uid;
  long long int gid;
  long long int euid;
  long long int egid;
  int on_disk;
  long long int resident_size;
  long long int parent;
};
```


## Client API (SELECT)
Client API is a functioanl API for monitoring Virtual Tables.  
Since Client API generates 'select query' by using query-builder, it doesn't need to write a query directly.

```cpp
  /// Querying device policies using Client API
  vist::VirtualTable<Policy<int>> table;
  for (const auto& row : table) {
    vist::schema::Policy<int> policy = { row[&Policy<int>::name], row[&Policy<int>::value] };
    std::cout << "Policy name: " << policy.name << ", ";
    std::cout << "Policy value: " << policy.value << "\n";
  }
```

## Admin API (SELECT, INSERT, UPDATE, DELETE)
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
