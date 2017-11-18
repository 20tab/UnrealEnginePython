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

### add_to_collection(name, ECollectionShareType, objectPath)

add an asset to a static collection using asset's path

### collection_exists(name, ECollectionShareType)

Returns true if a collection already exists

### create_unique_collection_name(name, ECollectionShareType)

returns a string with a unique collection name

### destroy_collection(name, ECollectionShareType)

deletes an existing collection and returns true on success and false on failure

### empty_collection(name, ECollectionShareType)

removes all the existing assets from a collection. Returns true on success and false on failure

### get_dynamic_query_text(name, ECollectionShareType)

returns the search text from a dynamic collection. If you use it on a static collection you will get an error

### set_dynamic_query_text(name, ECollectionShareType, searchText)

sets the search text to a dynamic collection. Returns true on success and false on failure

### remove_from_collection(name, ECollectionShareType, objectPath)

removes an asset from a static collection using asset's path. Returns true on success and false on failure

### get_assets_in_collection(name, ECollectionShareType, recursion(optional))

returns a list with all the asset paths. There is a third optional parameter for recursion. Default value is 1. This function works only with static collections

### get_objects_in_collection(name, ECollectionShareType, recursion(optional))

returns a list with all the object paths. There is a third optional parameter for recursion. Default value is 1. This function works only with static collections

### get_classes_in_collection(name, ECollectionShareType, recursion(optional))

returns a list with all the Classes. There is a third optional parameter for recursion. Default value is 1. This function works only with static collections

### get_parent_collection(name, ECollectionShareType)

returns the parent collection. If the collection has no parent, it returns None.

### has_collections()

returns True or False if the project has or doesn't have any collections.
