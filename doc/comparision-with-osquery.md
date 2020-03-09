# Comparison with osquery
Osquery views operating system as table 
from the perspective of the system administrator. 
It provides osqueryd(daemon), osqueryi(interactive shell).  

We view operating system as table 
from the perspective of the system programmar.  

Our purpose is to provide them more compact and 
more efficient interface from this different perspective.

|   | ViST | [OsQuery](https://github.com/osquery/osquery) |
|---|---|---|
| Perspective | system developer | system administrator |
| Project Type | SAM | OPENM |
| Component | daemon, **library** | daemon, shell |
| **Additional<br>dependencies<br>(without tizen)** | jinja2-python<br>rapidjson | jinja2-python<br>rapidsjon<br>lz4<br>zstd<br>rocksdb<br>thrift<br>|
