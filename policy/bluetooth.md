# Bluetooth Policy Virtual Table

| Table  | Column | Type | Value |
|---|---|---|---|
| bluetooth | state | int | 0 = off , 1 = on |
|   | desktopConnectivity | int | 0 = off , 1 = on |
|   | tethering | int | 0 = off , 1 = on |
|   | pairing | int | 0 = off , 1 = on |

## Get bluetooth policies
```sql
  SELECT * FROM bluetooth
  SELECT state FROM bluetooth
  SELECT desktopConnectivity FROM bluetooth
  SELECT pairing FROM bluetooth
  SELECT tethering FROM bluetooth
```

## Set bluetooth policies
```sql
  UPDATE bluetooth SET state = 1 # on
  UPDATE bluetooth SET desktopConnectivity = 0 # off
  UPDATE bluetooth SET pairing = 1 # on
  UPDATE bluetooth SET tethering = 0 # off
  UPDATE bluetooth SET state = 1, pairing = 0
```

# Query Builder
## Schema
```cpp
  struct Bluetooth {
    int state;
    int desktopConnectivity;
    int pairing;
    int tethering; 

    DECLARE_COLUMN(State, "state", &Bluetooth::state);
    DECLARE_COLUMN(DesktopConnectivity, "desktopConnectivity", &Bluetooth::DesktopConnectivity);
    DECLARE_COLUMN(Pairing, "pairing", &Bluetooth::pairing);
    DECLARE_COLUMN(Tethering, "tethering", &Bluetooth::tethering);
  };
  
  DECLARE_TABLE(bluetoothTable, "bluetooth", Bluetooth::State,
                                             Bluetooth::DesktopConnectivity,
                                             Bluetooth::Pairing,
                                             Bluetooth::Tethering);
```

## Generate type-safed query

```cpp
// SELECT * FROM bluetooth
  bluetoothTable.selectAll();
  
// SELECT state FROM bluetooth
  bluetoothTable.selectAll(Bluetooth::State);

// UPDATE bluetooth SET state = 1
  bluetoothTable.update(Bluetooth::State = 1);

// UPDATE bluetooth SET state = 1, pairing = 0
  bluetoothTable.update(Bluetooth::State = 1, Bluetooth::Pairing = 0);
```
