# Programming interface for client

---

# #1. Execute query
## Check smack table schema
### 'smack' table
| name | type |
|---|---|
| subject_label | TEXT |
| object_label | TEXT |
| access_type | TEXT |
| permission | TEXT |

## Use OsqueryManager to get smack information
```cpp
  using namespace osquerypp;
  
  // 1. Write query as std::string
  std::string query = "SELECT subject_label, object_label FROM smack WHERE access_type = 'read'";
   
  // 2. Load OsqueryManager
  auto manager = OsqueryManager::Load();
   
  // 3. Execute query by using OsqueryManager
  auto rows = manager->execute(query);
   
  // 4. Get result
  for (const auto& row : rows) {
      std::string slabel = row["subject_label"];
      std::string olabel = row["object_label];
  }
```

# #2. Subscribe event table
## Check smack_deny_events table schema
- The name of the event table ends with *_events.
### 'smack_deny_events' table
| name | type |
|---|---|
| action | TEXT |
| subject_label | TEXT |
| object_label | TEXT |
| access_type | TEXT |
| permission | TEXT |
| requested | TEXT |
| pid | INTEGER |

## Use OsqueryManager to subscribe smack-deny-events
```cpp
  using namespace osquerypp;
  
  // 1. Write callback function
  auto  onDeny = [&](const EventContext& ec, const Row& row)
    {
      // get data of event table
      std::cout << row["action"] << std::endl;
      std::cout << row["permission"] << std::endl;
      std::cout << row["subject_label"] << std::endl;
      std::cout << row["object_label"] << std::endl;
    }
    
  // 2. Load OsqueryManager
  auto manager = OsqueryManager::Load();
   
  // 3. Register callback with event_table by using OsqueryManager
  manager->subscribe("smack_deny_events", onDeny);
```
