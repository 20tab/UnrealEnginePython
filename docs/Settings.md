Managing Settings
=

Settings (both Editor and User-defined) are stored in standard UObject classes. The "mutable default object" if those classes is what stores the specific settings.

To access the "mutable default object" of a class you can use the ```unreal_engine.get_mutable_default(class)``` function.

The following example code shows how to change the Editor/Appearance settings:

```python
import unreal_engine as ue
from unreal_engine.classes import EditorProjectAppearanceSettings
from unreal_engine.enums import EUnit

# access the editor appearance settings

appearance_settings = ue.get_mutable_default(EditorProjectAppearanceSettings)
# print properties
ue.log(appearance_settings.properties())

# assign meters as the default distance/length unit (as list of values)
appearance_settings.DistanceUnits = [EUnit.Meters]
```
