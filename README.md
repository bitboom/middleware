# ViST, Virtual Security Table
ViST(Virtual Security Table) is a security monitoring framework using SQL query.
- Provides APIs for monitoring and controlling security resources.
- Views security resources as virtual tables and manipulates them through SQL queries.
- Adopts a plug-in architecture and uses [osquery](https://osquery.io/) as the query analysis engine.

# Architecture
![tizen-osquery-architecture](https://github.sec.samsung.net/storage/user/692/files/74819c00-4c95-11e9-9648-54e02513e338)

# Advantages of tizen-osquery better than osquery
- More available
  - We support not only monitor-feature("SELECT") but also control-feature("UPDATE") additionally.
- More strict
  - We can catch query errors at build time by query-builder.
- More compact
  - We focus the device security.

# Branch info
- master: osquery for tizen
- upstream: full osquery code (facebook/osquery)
