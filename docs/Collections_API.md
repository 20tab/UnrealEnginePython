# The Collections API

The ICollectionManager class exposes methods to manage the assets collections:

```python
from unreal_engine import ICollectionManager

print(ICollectionManager.get_collections())
# 1 means ECollectionShareType::CST_Local
print(ICollectionManager.get_child_collections('FooBar', 1))
```

Collections can be 'static' (they contain references to assets) or 'dynamic' (they apply a search string to get a list of assets)

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

### create_static_collection(name, ECollectionShareType)

create a new static collections given a name and a type

### create_dynamic_collection(name, ECollectionShareType)

create a new dynamic collections given a name and a type

### create_collection(name, ECollectionShareType, ECollectionStorageMode)

create a new dynamic collections given a name, a type and a storage mode

### reparent_collection(name, ECollectionShareType, new_name, ECollectionShareType)

move the 'name' collection to the 'new_name' parent


