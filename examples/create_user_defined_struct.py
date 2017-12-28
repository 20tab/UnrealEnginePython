import unreal_engine as ue

from unreal_engine.classes import StructureFactory
from unreal_engine.structs import StructVariableDescription, EdGraphTerminalType
from unreal_engine.enums import EPinContainerType

from unreal_engine.classes import Pawn, Actor
from unreal_engine.structs import Vector, Rotator, Transform

factory = StructureFactory()

new_struct = factory.factory_create_new('/Game/DumbStruct2')


variable = StructVariableDescription()
variable.VarName = 'AStringArray'
variable.FriendlyName = 'A String Array'
variable.ContainerType = EPinContainerType.Array
variable.ToolTip = 'A tooltip for the string array'
variable.Category = 'string'

# struct_add_variable returns the Guid of the newly created var
new_guid = new_struct.struct_add_variable(variable)

# remove default variable (created by the factory using its Guid)
# wecannot do this before, bacause structs cannot be empty
first_var = new_struct.struct_get_variables()[0]
new_struct.struct_remove_variable(first_var.VarGuid)

variable = StructVariableDescription()
variable.VarName = 'AObjectRef'
variable.FriendlyName = 'A Object Ref'
variable.ToolTip = 'A reference to an object'
variable.Category = 'object'
variable.SubCategoryObject = Pawn

new_struct.struct_add_variable(variable)

variable = StructVariableDescription()
variable.VarName = 'AClassRef'
variable.FriendlyName = 'A Class Ref'
variable.ToolTip = 'A reference to a class'
variable.Category = 'class'
variable.SubCategoryObject = Actor

new_struct.struct_add_variable(variable)

variable = StructVariableDescription()
variable.VarName = 'AVector'
variable.FriendlyName = 'A Vector'
variable.ToolTip = 'A Vector'
variable.Category = 'struct'
variable.SubCategoryObject = Vector
variable.DefaultValue = '17,22,30'

new_struct.struct_add_variable(variable)

variable = StructVariableDescription()
variable.VarName = 'ATransform'
variable.FriendlyName = 'A Transform'
variable.ToolTip = 'A Transform'
variable.Category = 'struct'
variable.SubCategoryObject = Transform
variable.DefaultValue = '17,22,30|30,60,90|1,1,1'

new_struct.struct_add_variable(variable)


variable = StructVariableDescription()
variable.VarName = 'ADictionary'
variable.FriendlyName = 'A Dictionary'
variable.ToolTip = 'A Dictionary'
variable.Category = 'string'
variable.ContainerType = EPinContainerType.Map
# the dictionary value type must be specified with a EdGraphTerminalType struct
terminal_type = EdGraphTerminalType()
terminal_type.TerminalCategory = 'float'
variable.PinValueType = terminal_type

new_guid = new_struct.struct_add_variable(variable)

new_struct.struct_move_variable_up(new_guid)
new_struct.struct_move_variable_up(new_guid)
new_struct.struct_move_variable_up(new_guid)
new_struct.struct_move_variable_up(new_guid)
new_struct.struct_move_variable_down(new_guid)

new_struct.save_package()

for description in new_struct.struct_get_variables():
    print(description.as_dict())

ue.open_editor_for_asset(new_struct)
