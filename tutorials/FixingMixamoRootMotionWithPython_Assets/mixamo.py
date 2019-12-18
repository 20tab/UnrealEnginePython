import unreal_engine as ue
from unreal_engine.classes import SkeletalMesh, Skeleton, AnimSequence, AnimSequenceFactory
from unreal_engine import FTransform, FRawAnimSequenceTrack, FQuat
from unreal_engine import SWindow, SObjectPropertyEntryBox

class DialogException(Exception):
    def __init__(self, message):
        ue.message_dialog_open(0, message)

class RootMotionFixer:

   
    def add_root_to_skeleton(self, mesh, bone='root'):
        base_path = mesh.get_path_name()
        new_path = ue.create_modal_save_asset_dialog('Choose destination path', ue.get_path(base_path), ue.get_base_filename(base_path) + '_rooted')
        if not new_path:
            raise DialogException('Please specify a new path for the Skeletal Mesh copy')
        package_name = ue.object_path_to_package_name(new_path)
        object_name = ue.get_base_filename(new_path)
        # the last True allows for overwrites
        new_mesh = mesh.duplicate(package_name, object_name, True)

        # generate a new skeleton
        new_skel = self.build_new_skeleton(mesh.Skeleton, object_name + '_Skeleton', bone)
        # save the new skeleton in the same package directory of the new skeletal mesh
        new_skel.save_package(package_name)

        # assign the new skeleton to the new mesh
        new_mesh.skeletal_mesh_set_skeleton(new_skel)

        new_skel.save_package()

        self.fix_bones_influences(new_mesh, mesh.Skeleton)

        new_mesh.save_package()

    def build_new_skeleton(self, skeleton, name, root):
        new_skel = Skeleton(name)
        new_skel.skeleton_add_bone(root, -1, FTransform())
        for index in range(0, skeleton.skeleton_bones_get_num()):
            bone_name = skeleton.skeleton_get_bone_name(index)
            bone_parent = skeleton.skeleton_get_parent_index(index)
            bone_transform = skeleton.skeleton_get_ref_bone_pose(index)
            if bone_parent == -1:
                bone_parent_name = root
            else:
                bone_parent_name = skeleton.skeleton_get_bone_name(bone_parent)
            new_bone_parent = new_skel.skeleton_find_bone_index(bone_parent_name)
            new_skel.skeleton_add_bone(bone_name, new_bone_parent, bone_transform)
        return new_skel

    def get_updated_bone_index(self, old_skeleton, new_skeleton, old_bone_map, new_bone_map, index):

        # get the skeleton bone_id from the map
        true_bone_id = old_bone_map[index]

        # get the bone name
        bone_name = old_skeleton.skeleton_get_bone_name(true_bone_id)

        # get the new index
        new_bone_id = new_skeleton.skeleton_find_bone_index(bone_name)

        # check if a new mapping is available
        if new_bone_id in new_bone_map:
            return new_bone_map.index(new_bone_id)

        new_bone_map.append(new_bone_id)
        return len(new_bone_map)-1

    def fix_bones_influences(self, mesh, old_skeleton):
        active_bones = []
        for section in range(0, mesh.skeletal_mesh_sections_num()):
            vertices = mesh.skeletal_mesh_get_soft_vertices(0, section)
            ue.log_warning(len(vertices))
            new_vertices = []
            old_bone_map = mesh.skeletal_mesh_get_bone_map(0, section)
            new_bone_map = []

            for vertex in vertices:
                bone_ids = list(vertex.influence_bones)
                for index, bone_id in enumerate(bone_ids):
                    if vertex.influence_weights[index] > 0:
                        bone_ids[index] = self.get_updated_bone_index(old_skeleton, mesh.Skeleton, old_bone_map, new_bone_map, bone_id)
                        if new_bone_map[bone_ids[index]] not in active_bones:
                            active_bones.append(new_bone_map[bone_ids[index]])
                vertex.influence_bones = bone_ids
                new_vertices.append(vertex)

            # assign new vertices
            mesh.skeletal_mesh_set_soft_vertices(new_vertices, 0, section)
            # add the new bone mapping
            mesh.skeletal_mesh_set_bone_map(new_bone_map, 0, section)

        # specify which bones are active and required (ensure root is added to required bones)
        mesh.skeletal_mesh_set_active_bone_indices(active_bones)
        # mark all the bones as required (eventually you can be more selective)
        mesh.skeletal_mesh_set_required_bones(list(range(0, mesh.Skeleton.skeleton_bones_get_num())))

    def set_skeleton(self, asset_data):
        self.choosen_skeleton = asset_data.get_asset()
        self.window.request_destroy()

    def split_hips(self, animation, bone='Hips'):
        self.choosen_skeleton = None
        # first ask for which skeleton to use:
        self.window = SWindow(title='Choose your new Skeleton', modal=True, sizing_rule=1)(
                     SObjectPropertyEntryBox(allowed_class=Skeleton, on_object_changed=self.set_skeleton)
                 )
        self.window.add_modal()
        if not self.choosen_skeleton:
            raise DialogException('Please specify a Skeleton for retargeting')

        factory = AnimSequenceFactory()
        factory.TargetSkeleton = self.choosen_skeleton
        
        base_path = animation.get_path_name()
        package_name = ue.get_path(base_path)
        object_name = ue.get_base_filename(base_path)

        new_anim = factory.factory_create_new(package_name + '/' + object_name + '_rooted')

        new_anim.NumFrames = animation.NumFrames
        new_anim.SequenceLength = animation.SequenceLength

        # first step is generatin the 'root' track
        # we need to do it before anything else, as the 'root' track must be the 0 one
        for index, name in enumerate(animation.AnimationTrackNames):
            if name == bone:
                data = animation.get_raw_animation_track(index)
                # extract root motion
                root_motion = [(position - data.pos_keys[0]) for position in data.pos_keys]

                # create a new track (the root motion one)
                root_data = FRawAnimSequenceTrack()
                root_data.pos_keys = root_motion
                # ensure empty rotations !
                root_data.rot_keys = [FQuat()]
        
                # add  the track
                new_anim.add_new_raw_track('root', root_data)
                break
        else:
            raise DialogException('Unable to find bone {0}'.format(bone))
           
        # now append the original tracks, but removes the position keys
        # from the original root bone
        for index, name in enumerate(animation.AnimationTrackNames):
            data = animation.get_raw_animation_track(index)
            if name == bone:
                # remove root motion from original track
                data.pos_keys = [data.pos_keys[0]]
                new_anim.add_new_raw_track(name, data)
            else:
                new_anim.add_new_raw_track(name, data)

        new_anim.save_package()


root_motion_fixer = RootMotionFixer()

for uobject in ue.get_selected_assets():
    if uobject.is_a(SkeletalMesh):
        root_motion_fixer.add_root_to_skeleton(uobject)
    elif uobject.is_a(AnimSequence):
        root_motion_fixer.split_hips(uobject)
    else:
        raise DialogException('Only Skeletal Meshes are supported')
