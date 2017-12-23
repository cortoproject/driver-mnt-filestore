# filestore
Store and retrieve objects in/from JSON files

## Usage
The easiest way to add a filestore to your project is to add a configuration file to your project and then load this file with `corto_use` in your application. Here's an example that recursively persists all objects under `/data`, and stores the files in `/tmp/myProjectStore`:

filestore.json:
```javascript
{
  "type": "driver/mnt/filestore/mount",
  "value": {
    "storedir": "/tmp/myProjectStore",
    "query": {
      "select": "//",
      "from": "data"
    },
    "policy.ownership": "LOCAL_SOURCE"
  }
}
```
By setting `policy.ownership` to `LOCAL_SOURCE`, objects in the store will be owned by the application. This ensures that when you restart the application, initial values from persisted objects are overwritten with the latest value from the store.

You can load this file in your application, by doing:
```c
if (corto_use("filestore.json", 0, NULL)) {
    corto_error("%s", corto_lasterr());
}
```
