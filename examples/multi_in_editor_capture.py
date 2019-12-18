import unreal_engine as ue
from unreal_engine.classes import AutomatedLevelSequenceCapture, World
from unreal_engine.structs import SoftObjectPath

level_sequence_mappings = {
    '/Game/SequenceForDefault001': '/Game/Default001',
    '/Game/SequenceForVR001': '/Game/VR001'
}

def setup_sequence(capture):
    ue.open_editor_for_asset(ue.load_object(World, level_sequence_mappings[capture.LevelSequenceAsset.AssetPathName]))

captures = []
for sequence_asset in level_sequence_mappings:
    capture = AutomatedLevelSequenceCapture()
    capture.LevelSequenceAsset = SoftObjectPath(AssetPathName=sequence_asset)
    captures.append(capture)

ue.in_editor_capture(captures, setup_sequence)
