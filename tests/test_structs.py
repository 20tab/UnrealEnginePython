import unittest
import unreal_engine as ue
from unreal_engine.structs import ColorMaterialInput, Key
from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings


class TestStructs(unittest.TestCase):

    def test_new_struct(self):
        material_input = ColorMaterialInput()
        self.assertTrue('MaskR' in material_input.fields())

    def test_new_struct_with_kwargs(self):
        material_input = ColorMaterialInput(Mask=1, MaskR=1, MaskG=1, MaskB=0, MaskA=1)
        self.assertEqual(material_input.Mask, 1)
        self.assertEqual(material_input.MaskR, 1)
        self.assertEqual(material_input.MaskG, 1)
        self.assertEqual(material_input.MaskB, 0)
        self.assertEqual(material_input.MaskA, 1)


    def test_struct_set(self):
        material_input = ColorMaterialInput()
        material_input.MaskG = 1
        self.assertEqual(material_input.MaskG, 1)

    def test_struct_clone(self):
        material_input = ColorMaterialInput(Mask=1, MaskR=0, MaskG=1, MaskB=0, MaskA=1)
        material_input2 = material_input.clone()
        self.assertEqual(material_input2.Mask, 1)
        self.assertEqual(material_input2.MaskR, 0)
        self.assertEqual(material_input2.MaskG, 1)
        self.assertEqual(material_input2.MaskB, 0)
        self.assertEqual(material_input2.MaskA, 1)

    def test_cmp(self):
        key1 = Key(KeyName='SpaceBar')
        key2 = Key(KeyName='SpaceBar')
        self.assertEqual(key1, key2)

    def test_ptr(self):
        source_model = StaticMeshSourceModel()
        source_model.BuildSettings.bRecomputeNormals=False
        source_model.BuildSettings.bRecomputeTangents=True
        source_model.BuildSettings.bUseMikkTSpace=True
        source_model.BuildSettings.bBuildAdjacencyBuffer=True
        source_model.BuildSettings.bRemoveDegenerates=True

        source_model2 = source_model.clone()
        self.assertEqual(source_model2.BuildSettings.bRecomputeNormals, False)
        self.assertEqual(source_model2.BuildSettings.bRecomputeTangents, True)
        self.assertEqual(source_model2.BuildSettings.bUseMikkTSpace, True)
        self.assertEqual(source_model2.BuildSettings.bBuildAdjacencyBuffer, True)
        self.assertEqual(source_model2.BuildSettings.bRemoveDegenerates, True)

   



