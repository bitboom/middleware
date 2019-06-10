# Osquery for Tizen [![CircleCI](https://circleci.sec.samsung.net/gh/RS7-SECIOTSW/tizen-osquery.svg?style=svg)](https://circleci.sec.samsung.net/gh/RS7-SECIOTSW/tizen-osquery)
> [Osquery](https://osquery.io/) is open-source project which means "Performant endpoint visibility".  
> It can be used as security monitoring, audit, malware anaysis.

We fork osquery and improve it for **device security which manages device-policy and auditing**.  
The supporting elements can be extended like security analyzer, network security, kernel integrity.

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
