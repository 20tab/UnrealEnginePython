# The DataTable API

DataTables allows you to store (as an asset) a "list" (or better a dictionary) of structures.

A DataTable is assigned to a UStruct, and then it can be filled with rows.

Each row has a unique name and a value. That value is a structure of the DataTable-associated UStruct.

So if you have a 'FWeaponStruct', you can build a DataTable full of 'FWeaponStruct's (one per-row)

You can create a DataTable (in addition to creating it directly from the content browser) directly from python:

```python
```

data_table_add_row(row_name, struct)

data_table_remove_row(row_name)

data_table_rename_row(old_row_name, new_row_name)

data_table_as_dict()

data_table_as_json(flags=0)

data_table_find_row(row_name)

data_table_get_all_rows()
