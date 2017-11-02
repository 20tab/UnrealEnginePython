# The Collections API

The ICollectionManager class exposes methods to manage the assets collections:

```python
from unreal_engine import ICollectionManager

print(ICollectionManager.get_collections())
```

The api expects knowledge of a bunch of enums mapped as int:

`ECollectionShareType`

```c
enum Type
{
    CST_System,
    CST_Local,
    CST_Private,
    CST_Shared,
    CST_All,
}
```

`ECollectionStorageMode`

```c
enum Type
{
    Static,
    Dynamic,
}
```



### get_collections()

returns the list of collections as a python list of tuples with the form (name, ECollectionShareType)

### get_root_collections()

returns the list of root collections as a python list of tuples with the form (name, ECollectionShareType)

### get_child_collections(name, ECollectionShareType)

returns the list of child collections for a collection (given as name and type) as a python list of tuples with the form (name, ECollectionShareType)

### get_root_collection_names()

returns the names of root collections as a python list of strings

### get_collection_names()

returns the names of collections as a python list of strings

