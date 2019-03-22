# Programming interface for client

## Check smack table schema
### 'smack' table
| name | type |
|---|---|
| subject_label | TEXT |
| object_label | TEXT |
| access_type | TEXT |
| permission | TEXT |


## Use OsqueryManager for getting smack information
```cpp
  using namespace osquerypp;
  
  // 1. Write query as std::string
  std::string query = "SELECT subject_label, object_label FROM smack WHERE access_type = 'read'";
   
  // 2. Load OsqueryManager
  auto manager = OsqueryManager::Load();
   
  // 3. Execute Query by using OsqueryManager
  auto rows = OsqueryManager::executeQuery(query);
   
  // 4. Get result
  for (const auto& row : rows) {
      std::string slabel = row["subject_label"];
      std::string olabel = row["object_label];
  }
```
