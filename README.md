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

**We make type-safe query-builder more intuitively.**

Refer [this](https://github.sec.samsung.net/RS7-SECIOTSW/tizen-osquery/tree/master/src/vist/query-builder)
to compare ours and other opensources.

# Architecture
ViST adopts 3-tier layerd architecture to separate of concern.
- Interface layer: Provide API to client to generate query
- Service layer: Parse the query and bind query to virtual table
- Logic layer: Include callback routine about virtual table executed via query 

<img src="https://github.sec.samsung.net/storage/user/692/files/b8eeed80-5a49-11ea-85cb-3b4a975b2343" alt="layered architecture" width="780" height="610">

# Design
Our design philoshophy is "Make usable and robust security framework.".

## Programming Abstractions
// TODO: Use SF keyword to overall  
Security functions in ViST
are built around
two abstractions.
One is the producer
which provides security functions
via virtual tables. 
Another is the consumer
which calls security functions
via type-safe query.

#### Security Function Producer
Producers is the subject who has own security functions.
They produce security functions and provide it to consumers.
There are many things to consider to producers.
Two concerns of those things are below.

- How to provide security functions to customer?
- How to design usable API?

To solve two concerns, we made functional requirement.
- <a name=FR1>FR1</a>: Provide feature what can be bounded security functions.
- <a name=FR2>FR2</a>: Provide a way to design API security functions.

#### Secuirty Function Consumer
Consumer is the subject who calls security functions.
Secuirty functions are not enemy of consumer.
We should provide unified interface to consumer.
The interface should be usable and robust according to our design philosophy.
From this, two functional requirements are added.

- <a name=FR3>FR3</a>: Provide unified interface of secuirty functions.
- <a name=FR4>FR4</a>: Provide a way to make unified interface more rubust.

#### ViST Framework
ViST framework provides **four main features and three types of API**
to support producer and consumer.
Main features are describe below and
APIs are describe at [programming interface](#ProgrammingInterface).

- Security virtual table: The object which be bound security functions ([FR1](#FR1))
- Struct-based schema: The schema of security functions ([FR2](#FR2))
- Query-based unified interface: The interface between query and virtual table ([FR3](#FR3))
- Type-safe query builder: The query builder with type-error checking at compile time ([FR4](#FR4))

## <a name="ProgrammingInterface">Programming Interface</a>
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
