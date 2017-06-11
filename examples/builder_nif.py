from unreal_engine.classes import StaticMesh
from unreal_engine.structs import StaticMeshSourceModel, MeshBuildSettings

from unreal_engine import FRawMesh
import time

import unreal_engine as ue
import itertools

# this is a static mesh importer script using niftools (https://github.com/niftools/pyffi)
from pyffi.formats.nif import NifFormat

# create a new StaticMesh
sm = StaticMesh()

# prepare a new LOD for the StaticMesh
source_model = StaticMeshSourceModel(BuildSettings=MeshBuildSettings(bRecomputeNormals=True, bRecomputeTangents=True, bUseMikkTSpace=False, bBuildAdjacencyBuffer=True,bRemoveDegenerates=True))

# open a nif file
stream = open('C:/Users/Roberto/Downloads/test_dump_tex.nif', 'rb')
data = NifFormat.Data()
data.read(stream)
stream.close()

# a custom class for storing vertex data
class Vertex:

    def __init__(self, vertex, texcoord, normal):
        self.vertex = vertex
        self.texcoord = texcoord
        self.normal = normal

# faces is a list containing Vertex instances
faces = []

#load only the first child of the first root
node = data.roots[0].children[0]

# we assume three vertices for each face (with counter-clockwise order)
# managing uvs and normals is left to the reader :P
for face in node.data.triangles:
    v = node.data.vertices[face.v_1]
    faces.append(Vertex((v.x, v.y, v.z), (0,0), None))
    v = node.data.vertices[face.v_3]
    faces.append(Vertex((v.x, v.y, v.z), (0,0), None))
    v = node.data.vertices[face.v_2]
    faces.append(Vertex((v.x, v.y, v.z), (0,0), None))

# instntiate a new FRawMesh
mesh = FRawMesh()
mesh.set_vertex_positions(list(itertools.chain(*[v.vertex for v in faces])))
# uvs are required
mesh.set_wedge_tex_coords(list(itertools.chain(*[v.texcoord for v in faces])))
# normals are optionals
#mesh.set_wedge_tangent_x(list(itertools.chain(*[v.normal for v in faces])))

# assign indices
mesh.set_wedge_indices([n for n, face in enumerate(faces)])

# assign the FRawMesh to the LOD0
mesh.save_to_static_mesh_source_model(source_model)

# assign LOD0 to the SataticMesh and build it
sm.SourceModels = [source_model]
sm.static_mesh_build()
sm.static_mesh_create_body_setup()

# save the asset
sm.save_package('/Game/NifStaticMesh')
