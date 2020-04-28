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
| Additional<br>dependencies<br>(without tizen) | rapidjson | jinja2-python<br>rapidsjon<br>lz4<br>zstd<br>rocksdb<br>thrift<br>|
| Code Lines | 16K (8K - osquery) | 120K |

---

## osquery 4.0.0

```
-----------------------------------------------------------------------------------
Language                         files          blank        comment           code
-----------------------------------------------------------------------------------
C++                                762          17268          16209          78648
C/C++ Header                       230           4622           9904          12151
CMake                              167           1768            927           7787
Python                              29            964            742           4428
Markdown                            49           1603              0           4065
Ruby                                67            598            234           2972
Objective C++                       20            450            380           2867
PowerShell                          19            474            463           2643
Bourne Shell                        26            419            286           1934
CSS                                  2            233             44           1019
XML                                  3              5              7            290
make                                 1             62              3            269
C                                    1             31             10            162
YAML                                 3             30              6            160
Bourne Again Shell                   1             18              6            140
Dockerfile                           1              0              5             45
JSON                                 1              0              0             24
DOS Batch                            3              4              7             20
Windows Resource File                1              0              0              3
-----------------------------------------------------------------------------------
SUM:                              1386          28549          29233         119627
-----------------------------------------------------------------------------------
```

---

## ViST 0.1.0

```
$ cloc . --exclude-dir=third-party
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                            118           2554           1910          10482
C/C++ Header                   101           1714           3371           4733
Markdown                         8             77              0            488
CMake                           30            127            411            472
Dockerfile                       1             12              9             34
SQL                              1              8             15             30
make                             1              4              0             13
XML                              1              0              0              1
-------------------------------------------------------------------------------
SUM:                           261           4496           5716          16253
-------------------------------------------------------------------------------
```

---

## osquery in ViST (about 7% of osquery)

```
$ cloc src/osquery/
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                             49           1274            787           6192
C/C++ Header                    41            844           2320           2094
CMake                            5             25             67             94
-------------------------------------------------------------------------------
SUM:                            95           2143           3174           8380
-------------------------------------------------------------------------------
```
