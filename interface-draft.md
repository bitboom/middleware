# *Programming interface for client*

# Type-safe getter API
## Senario #1 Get process informations
### Check Processes Struct
```cpp
// api/schema/processes.h
struct Processes {
	int pid;
	std::string name;
	std::string path;
	std::string cmdline;
/// ...
	std::string user_time;
	std::string system_time;
	std::string start_time;
};
```

### Use Properties API
```cpp
#include <property.h>
#include <schema/processes.h>

Properties<Processes> processes;
for (auto& p : processes) {
	std::cout << p[&Processes::pid] << std::endl;
	std::cout << p[&Processes::name] << std::endl;
	std::cout << p[&Processes::path] << std::endl;
	std::cout << p[&Processes::cmdline] << std::endl;
	std::cout << p[&Processes::uid] << std::endl;
	std::cout << p[&Processes::gid] << std::endl;
	std::cout << p[&Processes::euid] << std::endl;
	std::cout << p[&Processes::egid] << std::endl;
}
```

## Senario #2 Get system time
### Check Time Struct
```cpp
// api/schema/time.h
struct Time {
	int hour;
	int minutes;
	int seconds;
};
```
### Use Property API
```cpp
Property<Time> time;
std::cout << time[&Time::hour] << std::endl;
std::cout << time[&Time::minutes] << std::endl;
std::cout << time[&Time::seconds] << std::endl;
```
---

# Osquery F/W API
## #1. Query execution
### Check smack table schema
| name | type |
|---|---|
| subject_label | TEXT |
| object_label | TEXT |
| access_type | TEXT |
| permission | TEXT |

### Use OsqueryManager to get smack information
```cpp
using namespace osquerypp;

// 1. Write query as std::string
std::string query = "SELECT subject_label, object_label FROM smack WHERE access_type = 'read'";

// 2. Execute query by using OsqueryManager
auto rows = OsqueryManager::execute(query);

// 3. Get result
for (const auto& row : rows) {
  std::string slabel = row["subject_label"];
  std::string olabel = row["object_label];
}
```

## #2. Event subscription
### Check smack_deny_events table schema
- The name of the event table ends with *_events.  
| name | type |
|---|---|
| action | TEXT |
| subject_label | TEXT |
| object_label | TEXT |
| access_type | TEXT |
| permission | TEXT |
| requested | TEXT |
| pid | INTEGER |

### Use OsqueryManager to subscribe smack-deny-events
```cpp
using namespace osquerypp;

// 1. Write callback function
auto onDeny = [&](const EventContext& ec, const Row& row)
{
  // get data of event table
  std::cout << row["action"] << std::endl;
  std::cout << row["permission"] << std::endl;
  std::cout << row["subject_label"] << std::endl;
  std::cout << row["object_label"] << std::endl;
}

// 2. Register callback with event_table by using OsqueryManager
OsqueryManager::subscribe("smack_deny_events", onDeny);
```
