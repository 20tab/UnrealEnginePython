import unreal_engine as ue
from unreal_engine.structs import KAggregateGeom


mesh = ue.get_selected_assets()[0]

mesh.BodySetup.AggGeom = KAggregateGeom()

mesh.static_mesh_generate_kdop26()
mesh.static_mesh_generate_kdop18()
mesh.static_mesh_generate_kdop10x()
mesh.static_mesh_generate_kdop10y()
mesh.static_mesh_generate_kdop10z()
