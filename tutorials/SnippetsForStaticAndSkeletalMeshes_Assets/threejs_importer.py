import json
import unreal_engine as ue
from unreal_engine.classes import Skeleton, AnimSequence, SkeletalMesh, Material, MorphTarget, AnimSequence, AnimSequenceFactory
from unreal_engine import FTransform, FVector, FRotator, FQuat, FSoftSkinVertex, FMorphTargetDelta, FRawAnimSequenceTrack
from unreal_engine.structs import SkeletalMaterial, MeshUVChannelInfo, FloatCurve, RichCurve, RichCurveKey, SmartName, RawCurveTracks
from collections import OrderedDict
import numpy


class ThreeJSLoader:

    def __init__(self, filename, scale=1.0):
        # we need ordered json dictionaries
        with open(filename) as json_file:
            self.model = json.load(json_file, object_pairs_hook=OrderedDict)
        self.scale = scale
        # ask the user where to generate the new assets
        new_path = ue.create_modal_save_asset_dialog('Choose destination path')
        package_name = ue.object_path_to_package_name(new_path)
        object_name = ue.get_base_filename(new_path)
        self.skeleton = self.build_skeleton(package_name, object_name)
        # this is the list of soft skin vertices (they contains mesh data as well as bone influences)
        self.vertices = []
        # this contain mapping between the soft skin vertices and the json file vertex index (this is required for building morph targets)
        self.vertex_map = []
        self.mesh = self.build_mesh(package_name, object_name)
        self.curves = self.build_morph_targets()
        self.animation = self.build_animation(package_name, object_name)

    def build_skeleton(self, pkg_name, obj_name):
        pkg = ue.get_or_create_package('{0}_Skeleton'.format(pkg_name))
        skel = Skeleton('{0}_Skeleton'.format(obj_name), pkg)
        # add a root bone from which all of the others will descend 
        # (this trick will avoid generating an invalid skeleton [and a crash], as in UE4 only one root can exist)
        skel.skeleton_add_bone('root', -1, FTransform())
        # iterate bones in the json file, note that we move from opengl axis to UE4
        # (y on top, x right, z forward right-handed) to (y right, x forward left-handed, z on top)
        for bone in self.model['bones']:
            # assume no rotation
            quat = FQuat()
            # give priority to quaternions
            # remember to negate x and y axis, as we invert z on position
            if 'rotq' in bone:
                quat = FQuat(bone['rotq'][2], bone['rotq'][0] * -1,
                             bone['rotq'][1] * -1, bone['rotq'][3])
            elif 'rot' in bone:
                quat = FRotator(bone['rot'][2], bone['rot'][0] - 180
                                , bone['rot'][1] - 180).quaternion()
            pos = FVector(bone['pos'][2] * -1, bone['pos'][0],
                          bone['pos'][1]) * self.scale
            # always set parent+1 as we added the root bone before
            skel.skeleton_add_bone(
                bone['name'], bone['parent'] + 1, FTransform(pos, quat))
        skel.save_package()
        return skel

    def build_soft_vertex(self, index):
        # create a new soft skin vertex, holding tangents, normal, uvs, influences...
        v = FSoftSkinVertex()
        v_index = self.model['faces'][index] * 3
        # here we assume 2 bone influences, technically we should honour what the json influencesPerVertex field exposes
        b_index = self.model['faces'][index] * 2
        v.position = FVector(self.model['vertices'][v_index + 2] * -1, self.model['vertices']
                             [v_index], self.model['vertices'][v_index + 1]) * self.scale
        v.influence_weights = (
            self.model['skinWeights'][b_index], self.model['skinWeights'][b_index + 1])
        v.influence_bones = (
            self.model['skinIndices'][b_index] + 1, self.model['skinIndices'][b_index + 1] + 1)
        # return the json index too, as we will need it later for computing morph targets
        return (v, v_index)

    def get_normals(self, index):
        n_index = self.model['faces'][index] * 3
        return FVector(self.model['normals'][n_index + 2] * -1, self.model['normals'][n_index], self.model['normals'][n_index + 1])

    def build_mesh(self, pkg_name, obj_name):
        index = 0
        # this supports only format 3 (now deprecated, https://github.com/mrdoob/three.js/wiki/JSON-Model-format-3)
        while index < len(self.model['faces']):
            face = self.model['faces'][index]
            index += 1
            points = 3
            v0 = v1 = v2 = v3 = None
            if face & 1 == 0:
                # triangle
                v0, v0_index = self.build_soft_vertex(index)
                v1, v1_index = self.build_soft_vertex(index + 1)
                v2, v2_index = self.build_soft_vertex(index + 2)
            else:
                # quad
                v0, v0_index = self.build_soft_vertex(index)
                v1, v1_index = self.build_soft_vertex(index + 1)
                v2, v2_index = self.build_soft_vertex(index + 2)
                v3, v3_index = self.build_soft_vertex(index + 3)

            if v3:
                points = 4

            index += points

            if face & 2:
                index += 1

            if face & 4:
                index += 1

            if face & 8:
                index += points

            if face & 16:
                index += 1

            if face & 32:
                v0.tangent_z = self.get_normals(index)
                v1.tangent_z = self.get_normals(index + 1)
                v2.tangent_z = self.get_normals(index + 2)
                if v3:
                    v3.tangent_z = self.get_normals(index + 3)
                index += points

            if face & 64:
                index += 1

            if face & 128:
                index += points

            if points == 3:
                # we need to fix winding, from OpenGL (counterwise) to UE4 (clockwise)
                self.vertices.append(v2)
                self.vertex_map.append(v2_index)
                self.vertices.append(v0)
                self.vertex_map.append(v0_index)
                self.vertices.append(v1)
                self.vertex_map.append(v1_index)
            else:
                # we have a quad, generate two triangles
                # we need to fix winding, from OpenGL (counterwise) to UE4 (clockwise)
                self.vertices.append(v3)
                self.vertex_map.append(v3_index)
                self.vertices.append(v0)
                self.vertex_map.append(v0_index)
                self.vertices.append(v1)
                self.vertex_map.append(v1_index)

                self.vertices.append(v2)
                self.vertex_map.append(v2_index)
                self.vertices.append(v3)
                self.vertex_map.append(v3_index)
                self.vertices.append(v1)
                self.vertex_map.append(v1_index)

        pkg = ue.get_or_create_package(pkg_name)
        sm = SkeletalMesh(obj_name, pkg)
        sm.skeletal_mesh_set_skeleton(self.skeleton)
        # generate the LOD from the list of soft skin vertices
        sm.skeletal_mesh_build_lod(self.vertices)
        sm.save_package()
        return sm

    def build_morph_targets(self):
        # when we build the skeletal mesh LOD by passing soft skin vertices
        # UE4 will internally optimize the vertices to reduce duplicates
        # for this reason the vertex index we built is different from the one stored into UE4
        # the skeletal_mesh_to_import_vertex_map() returns the original mapping given the new one
        import_map = self.mesh.skeletal_mesh_to_import_vertex_map()
        # we will fill animation curves for later usage
        curves = []
        for morph_item in self.model['morphTargets']:
            # ensure the MorphTarget has the SkeletalMesh as outer
            morph = MorphTarget('', self.mesh)
            deltas = []
            for idx, import_index in enumerate(import_map):
                # get the original json vertex index
                vertex_index = self.vertex_map[import_index]
                # get the original soft skin vertex
                vdata = self.vertices[import_index]
                x = morph_item['vertices'][vertex_index + 2] * -1
                y = morph_item['vertices'][vertex_index]
                z = morph_item['vertices'][vertex_index + 1]
                delta = FMorphTargetDelta()
                delta.source_idx = idx
                # store the difference between original vertex position and the morph target one
                delta.position_delta = (
                    FVector(x, y, z) * self.scale) - vdata.position
                deltas.append(delta)
            # check for the return value, as sometimes morph targets
            # in json files do not generate any kind of modification
            # so unreal will skip it
            if morph.morph_target_populate_deltas(deltas):
                # register the morph target
                self.mesh.skeletal_mesh_register_morph_target(morph)
                # add curve, not required, we can use it later for skeletal-based animations
                curves.append(FloatCurve(Name=SmartName(DisplayName=morph.get_name()), FloatCurve=RichCurve(
                    Keys=[RichCurveKey(Time=0.0, Value=0.0), RichCurveKey(Time=1.0, Value=1.0)])))
        self.mesh.save_package()
        return curves

    def build_animation(self, pkg_name, obj_name):
        factory = AnimSequenceFactory()
        factory.TargetSkeleton = self.skeleton
        new_anim = factory.factory_create_new('{0}_Animation'.format(pkg_name))

        new_anim.NumFrames = self.model['animation']['length'] * \
            self.model['animation']['fps']
        new_anim.SequenceLength = self.model['animation']['length']
        # each bone maps to a track in UE4 animations
        for bone_index, track in enumerate(self.model['animation']['hierarchy']):
            # retrieve the bone/track name from the index (remember to add 1 as we have the additional root bone)
            bone_name = self.skeleton.skeleton_get_bone_name(bone_index + 1)

            positions = []
            rotations = []
            scales = []

            for key in track['keys']:
                t = key['time']
                if 'pos' in key:
                    positions.append(
                        (t, FVector(key['pos'][2] * -1, key['pos'][0], key['pos'][1]) * 100))
                if 'rotq' in key:
                    rotations.append((t, FQuat(
                        key['rotq'][2], key['rotq'][0] * -1, key['rotq'][1] * -1, key['rotq'][3])))
                elif 'rot' in key:
                    # is it a quaternion ?
                    if len(key['rot']) == 4:
                        rotations.append(
                            (t, FQuat(key['rot'][2], key['rot'][0] * -1, key['rot'][1] * -1, key['rot'][3])))
                    else:
                        rotations.append(
                            (t, FRotator(key['rot'][2], key['rot'][0] - 180, key['rot'][1] - 180).quaternion()))
            pos_keys = []
            rot_keys = []
            # generate the right number of frames
            for t in numpy.arange(0, self.model['animation']['length'], 1.0 / self.model['animation']['fps']):
                pos_keys.append(self.interpolate_vector(positions, t))
                rot_keys.append(self.interpolate_quaternion(
                    rotations, t).get_normalized())
            track_data = FRawAnimSequenceTrack()
            track_data.pos_keys = pos_keys
            track_data.rot_keys = rot_keys
            new_anim.add_new_raw_track(bone_name, track_data)

        # if we have curves, just add them to the animation
        if self.curves:
            new_anim.RawCurveData = RawCurveTracks(FloatCurves=self.curves)

        new_anim.save_package()

        return new_anim

    def interpolate_vector(self, timeline, t):
        keys = []
        x_values = []
        y_values = []
        z_values = []
        for key, value in timeline:
            keys.append(key)
            x_values.append(value[0])
            y_values.append(value[1])
            z_values.append(value[2])

        x = numpy.interp(t, keys, x_values)
        y = numpy.interp(t, keys, y_values)
        z = numpy.interp(t, keys, z_values)
        return FVector(x, y, z)

    def interpolate_quaternion(self, timeline, t):
        keys = []
        x_values = []
        y_values = []
        z_values = []
        w_values = []
        for key, value in timeline:
            keys.append(key)
            x_values.append(value[0])
            y_values.append(value[1])
            z_values.append(value[2])
            w_values.append(value[3])

        x = numpy.interp(t, keys, x_values)
        y = numpy.interp(t, keys, y_values)
        z = numpy.interp(t, keys, z_values)
        w = numpy.interp(t, keys, w_values)
        return FQuat(x, y, z, w)


filename = ue.open_file_dialog('Choose a three.js file')[0]

threejs = ThreeJSLoader(filename, 100)

ue.open_editor_for_asset(threejs.animation)
