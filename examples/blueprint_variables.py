import unreal_engine as ue

from unreal_engine.classes import Material, BlueprintFactory, Blueprint, Actor, Texture2D, SkeletalMesh
from unreal_engine.structs import EdGraphPinType, Vector, Rotator, EdGraphTerminalType
from unreal_engine.enums import EPinContainerType

import time

bp = ue.create_blueprint(Actor, '/Game/FooActor' + str(int(time.time())))

pin = EdGraphPinType(PinCategory='object', PinSubCategoryObject=Material)
ue.blueprint_add_member_variable(bp, 'TestMat', pin, None, '/Engine/MapTemplates/Materials/BasicAsset03.BasicAsset03')

pin = EdGraphPinType(PinCategory='class', PinSubCategoryObject=Texture2D)
ue.blueprint_add_member_variable(bp, 'TestTextureClass', pin)

pin = EdGraphPinType(PinCategory='struct',PinSubCategoryObject=Vector)
ue.blueprint_add_member_variable(bp, 'TestVector', pin, None, '17,22,30')

pin = EdGraphPinType(PinCategory='struct',PinSubCategoryObject=Rotator,ContainerType=EPinContainerType.Array)
ue.blueprint_add_member_variable(bp, 'TestRotator', pin, None, '((Pitch=0.000000,Yaw=3.000000,Roll=0.000000),(Pitch=1.000000,Yaw=0.000000,Roll=0.000000))')

pin = EdGraphPinType(PinCategory='string',ContainerType=EPinContainerType.Map,PinValueType=EdGraphTerminalType(TerminalCategory='object',TerminalSubCategoryObject=SkeletalMesh))
ue.blueprint_add_member_variable(bp, 'TestMap', pin, None, '(("firstKey", SkeletalMesh\'"/Game/Skel001"\'),("secondKey", SkeletalMesh\'"/Game/Skel002"\'))')

ue.compile_blueprint(bp)

ue.open_editor_for_asset(bp)
