# Osquery for Tizen
> [Osquery](https://osquery.io/) is open-source project which means "Performant endpoint visibility".  
> It can be used as security monitoring, audit, malware anaysis.

We fork osquery and improve it for **device security which manages device-policy and auditing**.  
The supporting elements can be extended like security analyzer, network security, kernel integrity.

# Advantages of tizen-osquery better than osquery
- More available
  - We support not only monitor-feature("SELECT") but also control-feature("UPDATE") additionally.
- More strict
  - We can catch query errors at build time by query-builder.
- More compact
  - We focus the device security.

# Branch info
- devel: osquery for tizen
- upstream: full osquery code (facebook/osquery)
