# The Collections API

The ICollectionManager class exposes methods to manage the assets collections:

```python
from unreal_engine import ICollectionManager

print(ICollectionManager.get_collection())
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

returns the list of collections as a tuple with the form (name, ECollectionShareType)
