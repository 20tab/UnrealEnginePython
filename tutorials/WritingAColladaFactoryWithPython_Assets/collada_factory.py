from unreal_engine.classes import PyFactory, StaticMesh, Object, Class

import unreal_engine as ue

from collada import Collada

from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings
from unreal_engine import FRawMesh
import numpy

from unreal_engine import FVector, FRotator
from unreal_engine import SWindow, SVerticalBox, SHorizontalBox, SButton, SRotatorInputBox
from unreal_engine.enums import EHorizontalAlignment

from unreal_engine.classes import Material
from unreal_engine.structs import Rotator, StaticMaterial

class ColladaImportOptions(Object):

    DefaultRotation = Rotator
    DefaultMaterial = Material

class ColladaFactory(PyFactory):

    ImportOptions = ColladaImportOptions()

    def __init__(self):
        # inform the editor that this class is able to import assets
        self.bEditorImport = True
        # register the .dae extension as supported
        self.Formats = ['dae;Collada']
        # set the UClass this UFactory will generate
        self.SupportedClass = StaticMesh

    
    def open_collada_wizard(self):

        def cancel_import():
            self.wizard.request_destroy()

        def confirm_import():
            self.do_import = True
            self.wizard.request_destroy()

        self.wizard = SWindow(title='Collada Import Options', modal=True, sizing_rule=1)(
                          SVerticalBox()
                          (
                              ue.create_detail_view(self.ImportOptions),
                              auto_height=True,
                              padding = 10
                          )
                          (
                              SHorizontalBox()
                              (
                                  SButton(text='Cancel', on_clicked=cancel_import, h_align = EHorizontalAlignment.HAlign_Center)
                              )
                              (
                                  SButton(text='Import', on_clicked=confirm_import, h_align = EHorizontalAlignment.HAlign_Center)
                              ),
                              auto_height=True,
                              padding = 4,
                          ),
                          
                      )
        self.wizard.add_modal()


    # this functions starts with an uppercase letter, so it will be visible to the UE system
    # not required obviously, but it will be a good example
    def FixMeshData(self):
        # move from collada system (y on top) to ue4 one (z on top, forward decreases over viewer)
        for i in range(0, len(self.vertices), 3):
           xv, yv, zv = self.vertices[i], self.vertices[i+1], self.vertices[i+2]
           # invert forward
           vec = FVector(zv * -1, xv, yv) * self.ImportOptions.DefaultRotation
           self.vertices[i] = vec.x
           self.vertices[i+1] = vec.y
           self.vertices[i+2] = vec.z
           xn, yn, zn = self.normals[i], self.normals[i+1], self.normals[i+2]
           nor = FVector(zn * -1, xn, yn) * self.ImportOptions.DefaultRotation
           # invert forward
           self.normals[i] = nor.x
           self.normals[i+1] = nor.y
           self.normals[i+2] = nor.z
        
        # fix uvs from 0 on bottom to 0 on top
        for i, uv in enumerate(self.uvs):
            if i % 2 != 0:
                self.uvs[i] = 1 - uv

    def PyFactoryCreateFile(self, uclass: Class, parent: Object, name: str, filename: str) -> Object:
        # load the collada file
        dae = Collada(filename)
        ue.log_warning(dae)

        self.do_import = False
        self.open_collada_wizard()

        if not self.do_import:
            return None


        # create a new UStaticMesh with the specified name and parent
        static_mesh = StaticMesh(name, parent)

        # prepare a new model with the specified build settings
        source_model = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=False, bRecomputeTangents=True, bUseMikkTSpace=True, bBuildAdjacencyBuffer=True, bRemoveDegenerates=True))

        # extract vertices, uvs and normals from the da file (numpy.ravel will flatten the arrays to simple array of floats)
        triset = dae.geometries[0].primitives[0]
        self.vertices = numpy.ravel(triset.vertex[triset.vertex_index])
        # take the first uv channel (there could be multiple channels, like the one for lightmapping)
        self.uvs = numpy.ravel(triset.texcoordset[0][triset.texcoord_indexset[0]])
        self.normals = numpy.ravel(triset.normal[triset.normal_index])

        # fix mesh data
        self.FixMeshData()
        
        # create a new mesh, FRawMesh is an ptopmized wrapper exposed by the python plugin. read: no reflection involved
        mesh = FRawMesh()
        # assign vertices
        mesh.set_vertex_positions(self.vertices)
        # uvs are required
        mesh.set_wedge_tex_coords(self.uvs)
        # normals are optionals
        mesh.set_wedge_tangent_z(self.normals)
        
        # assign indices (not optimized, just return the list of triangles * 3...)
        mesh.set_wedge_indices(numpy.arange(0, len(triset) * 3))

        # assign the FRawMesh to the LOD0 (the model we created before)
        mesh.save_to_static_mesh_source_model(source_model)

        # assign LOD0 to the SataticMesh and build it
        static_mesh.SourceModels = [source_model]
        static_mesh.static_mesh_build()
        static_mesh.static_mesh_create_body_setup()

        static_mesh.StaticMaterials = [StaticMaterial(MaterialInterface=self.ImportOptions.DefaultMaterial, MaterialSlotName='Main')]

        return static_mesh
