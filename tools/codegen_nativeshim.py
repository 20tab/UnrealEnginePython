import os, sys
from pprint import pprint
import cogapp
from pathlib import Path



plugin_source_path = os.path.join(os.path.dirname(__file__), "../Source")

cog_files = [
    r'UnrealEnginePython\Private\Wrappers\UEPyESlateEnums.cpp',
]

cog_files = [str(Path(plugin_source_path, inCogFile)) for inCogFile in cog_files]

    

##============##
## Run Script ##
##============##


if __name__ == '__main__':
    for cog_file in cog_files:
        if not os.access(cog_file, os.W_OK):
            print(f'ERROR: File is read only: {cog_file}')
        cogapp.Cog().main(['codegen_nativeshim.py','-r', f'-w', '-I ' + os.path.dirname(__file__), cog_file])
