# ViST, Virtual Security Table
ViST(Virtual Security Table) is a security monitoring framework using SQL query.
- Provides APIs for monitoring and controlling security resources.
- Views security resources as virtual tables and manipulates them through SQL queries.
- Adopts a plug-in architecture and uses [osquery](https://osquery.io/) as the query analysis engine.

# Architecture (Layered View)
![vist-architecture](https://github.sec.samsung.net/storage/user/692/files/82d63880-fa6c-11e9-91d2-af36faed1869)

# Programming Abstraction
ViST provides three types of API.  
One is for data structure and the other is for functional.

## Schema API
Schema API represents the data structure of Virtua Tables.  
This is referred to by Client API and Admin API.

## Client API (SELECT)
Client API is a functioanl API for monitoring Virtual Tables.  
Since Client API generates 'select query' by using query-builder, it doesn't need to write a query directly.

```cpp
  /// Querying device policies using Client API
  vist::VirtualTable<Policy> table;
  for (const auto& row : table) {
    vist::schema::Policy policy = { row[&Policy::name], row[&Policy::value] };
    std::cout << "Policy name: " << policy.name << ", ";
    std::cout << "Policy value: " << policy.value << "\n";
  }
```

## Admin API (SELECT, INSERT, UPDATE, DELETE)
Admin API is a functioanl API for manipulating Virtual Tables.  
This executes the query statement for the virtual table.
```cpp
   /// Registering policy admin using Admin API
   vist::Query::Execute("INSERT INTO policy_admin (name, uid) VALUES ('admin', 0)");
   
   /// rows includes [name:admin, uid:0]
   auto rows = vist::Query::Execute("SELECT * FROM policy_admin");
   
   /// Excluding policy admin using Admin API
   vist::Query::Execute("DELETE FROM policy_admin WHERE name = 'testAdmin' AND uid = 1");
```

# Contacts
- Sangwan Kwon (sangwan.kwon@samsung.com)
- Jaemin Ryu (jm77.ryu@samsung.com)
