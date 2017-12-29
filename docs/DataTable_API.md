# The DataTable API

DataTables allows you to store (as an asset) a "list" (or better a dictionary) of structures.

A DataTable is assigned to a UStruct, and then it can be filled with rows.

Each row has a unique name and a value. That value is a structure of the DataTable-associated UStruct.

So if you have a 'FWeaponStruct', you can build a DataTable full of 'FWeaponStruct's (one per-row)

You can create a DataTable (in addition to creating it directly from the content browser) directly from python:

```python
import unreal_engine as ue

from unreal_engine.classes import DataTableFactory
from unreal_engine.structs import Transform

dt_factory = DataTableFactory()
dt_factory.Struct = Transform

dt = dt_factory.factory_create_new('/Game/TransformDataTable')
```

Being a UStruct, Transform is a perfectly valid type for a DataTable (you can obviously use any structure, check https://github.com/20tab/UnrealEnginePython/blob/master/examples/create_user_defined_struct.py for creating custom structs)

Once you get a reference to a DataTable you can use the api:

### data_table_add_row(row_name, struct)

```python
# add an empty Transform
dt.data_table_add_row('FirstRow', Transform())
```

### data_table_remove_row(row_name)

### data_table_rename_row(old_row_name, new_row_name)

### data_table_as_dict()

### data_table_as_json(flags=0)

### data_table_find_row(row_name)

### data_table_get_all_rows()
