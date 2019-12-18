# Assets migration

Assets migration is a procedure allowing you to export the assets of a project into another unreal engine project.

When triggering it from the editor, you will get a list of packages to export and where to save them.

Here we will see how to automate the task from python

## Step1: getting asset dependencies

An important topic you need to understand, is that you export packages not single assets. Packages are mapped into real files.

Packages can contain multiple assets (albeit highly discouraged).

When we want to migrate an asset we need to get its package and all othe packages referenced by the asset (as an example, a blueprint
with a StaticMeshComponent, holds a reference to the StaticMesh asset, so weneed to export it to).

The unreal_engine.get_asset_dependencies(package_name) will do the trick:

```python
import unreal_engine as ue

# get the currently selected asset
asset = ue.get_selected_assets()[0]

# retrieve the package path
package_path = asset.get_outermost().get_path_name()
print(package_path)

def recursive_deps(pkg_path, deps):
    deps.append(pkg_path)
    for _dep in ue.get_asset_dependencies(pkg_path):
        if _dep not in deps:
            if not _dep.startswith('/Engine') and not _dep.startswith('/Script'):
                recursive_deps(_dep, deps)


deps = []

recursive_deps(package_path, deps)

print(deps)
```

## Step2: fixing dirty packages

A package is marked as dirty when it has been modified without saving it. In this step we will check each package we need to
migrate and we will eventually save it:

```python
for _dep in deps:
    # load the package
    pkg = ue.load_package(_dep)
    # check for dirty packages
    if pkg.package_is_dirty():
        pkg.save_package()
```

## Step3: copying .uasset files in the destination project

This is the last step, we retieve the filename of each package and will copy it in the destination directory.

Full script for migration:

```python
import unreal_engine as ue
import os
from shutil import copyfile

# get the currently selected asset
asset = ue.get_selected_assets()[0]

# retrieve the package path
package_path = asset.get_outermost().get_path_name()
print(package_path)

def recursive_deps(pkg_path, deps):
    deps.append(pkg_path)
    for _dep in ue.get_asset_dependencies(pkg_path):
        if _dep not in deps:
            if not _dep.startswith('/Engine') and not _dep.startswith('/Script'):
                recursive_deps(_dep, deps)


deps = []

recursive_deps(package_path, deps)

print(deps)

destination = 'D:/UdemyCast/Content'

for _dep in deps:
    # check for dirty packages
    pkg = ue.load_package(_dep)
    if pkg.package_is_dirty():
        pkg.save_package()
    src = pkg.package_get_filename()
    sub = src.split('/Content/', 1)[1]
    dst = os.path.join(destination, sub)
    copyfile(src, dst)

print('asset migrated')
```

Ensure the destination directory is a /Content directory in UE4 project
