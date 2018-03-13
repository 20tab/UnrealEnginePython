
![MaterialExpressionStaticSwitch Screenshot](https://github.com/20tab/UnrealEnginePython/blob/master/examples/MaterialExpressionStaticSwitch.png)

```python
import unreal_engine as ue
from unreal_engine.classes import MaterialFactoryNew
from unreal_engine.classes import MaterialExpressionStaticSwitch, MaterialExpressionStaticSwitchParameter, MaterialExpressionVectorParameter
from unreal_engine.structs import ColorMaterialInput, ExpressionInput
import time

material_factory = MaterialFactoryNew()
material = material_factory.factory_create_new('/Game/Materials/Test' + str(int(time.time())))

material.modify()

static_switch_node = MaterialExpressionStaticSwitch('', material)
static_switch_node.MaterialExpressionEditorX = -200
static_switch_node.MaterialExpressionEditorY = 50

static_switch_parameter_node_true = MaterialExpressionStaticSwitchParameter('', material)
static_switch_parameter_node_true.MaterialExpressionEditorX = -400
static_switch_parameter_node_true.MaterialExpressionEditorY = 0
static_switch_parameter_node_true.ParameterName = 'True'

static_switch_parameter_node_false = MaterialExpressionStaticSwitchParameter('', material)
static_switch_parameter_node_false.MaterialExpressionEditorX = -400
static_switch_parameter_node_false.MaterialExpressionEditorY = 200
static_switch_parameter_node_false.ParameterName = 'False'

static_switch_node.A = ExpressionInput(Expression=static_switch_parameter_node_true)
static_switch_node.B = ExpressionInput(Expression=static_switch_parameter_node_false)

vector_parameter_one = MaterialExpressionVectorParameter('', material)
vector_parameter_one.MaterialExpressionEditorX = -600
vector_parameter_one.MaterialExpressionEditorY = -150
vector_parameter_one.ParameterName = 'One'

vector_parameter_two = MaterialExpressionVectorParameter('', material)
vector_parameter_two.MaterialExpressionEditorX = -800
vector_parameter_two.MaterialExpressionEditorY = -50
vector_parameter_two.ParameterName = 'Two'

vector_parameter_three = MaterialExpressionVectorParameter('', material)
vector_parameter_three.MaterialExpressionEditorX = -600
vector_parameter_three.MaterialExpressionEditorY = 150
vector_parameter_three.ParameterName = 'Three'

vector_parameter_four = MaterialExpressionVectorParameter('', material)
vector_parameter_four.MaterialExpressionEditorX = -800
vector_parameter_four.MaterialExpressionEditorY = 250
vector_parameter_four.ParameterName = 'Four'


static_switch_parameter_node_true.A = ExpressionInput(Expression=vector_parameter_one)
static_switch_parameter_node_true.B = ExpressionInput(Expression=vector_parameter_two)

static_switch_parameter_node_false.A = ExpressionInput(Expression=vector_parameter_three)
static_switch_parameter_node_false.B = ExpressionInput(Expression=vector_parameter_four)

material.BaseColor = ColorMaterialInput(Expression=static_switch_node)

material.Expressions = [
                           static_switch_node,
                           static_switch_parameter_node_true,
                           static_switch_parameter_node_false,
                           vector_parameter_one,
                           vector_parameter_two,
                           vector_parameter_three,
                           vector_parameter_four
                       ]

material.post_edit_change()

ue.open_editor_for_asset(material)
```
