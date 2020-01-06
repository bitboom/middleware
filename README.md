Project ViST is inspired by [osquery](https://osquery.io/).  
osquery views operating system as table 
from the perspective of the system administrator. 

We view operating system as table 
from the perspective of the system programmar.  
We want to provide them more compact and 
more efficient library from this different perspective.


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
