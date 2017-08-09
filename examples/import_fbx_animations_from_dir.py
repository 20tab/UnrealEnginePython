import unreal_engine as ue

from unreal_engine.classes import PyFbxFactory, Skeleton
from unreal_engine.enums import EFBXImportType

import os

base_directory = 'D:/Users/rober/Downloads/mixamo'

# get a reference to the skeleton
skeleton = ue.load_object(Skeleton, '/Game/run_Skeleton')

factory = PyFbxFactory()
factory.ImportUI.MeshTypeToImport = EFBXImportType.FBXIT_Animation
factory.ImportUI.Skeleton = skeleton

for item in os.listdir(base_directory):
    if item.endswith('.fbx'):
        animation = factory.factory_import_object(os.path.join(base_directory, item), '/Game/Mixamo002')