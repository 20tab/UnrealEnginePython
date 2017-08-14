import unreal_engine as ue
import sys

ue.log_warning('UnrealEnginePython build check: {0}/UE{1}.{2}'.format(sys.version, ue.ENGINE_MAJOR_VERSION, ue.ENGINE_MINOR_VERSION))
