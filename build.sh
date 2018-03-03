#!/bin/bash
echo "Set the environment variable UE4 in the console or in the system variables"
echo "For example in my Linux machine, I use 'export UE4=/home/unreal/UnrealEngine/4.18'"

plugin_file=`pwd`/UnrealEnginePython.uplugin
package_folder=`pwd`/Plugins/UnrealEnginePython
"${UE4}"/Engine/Build/BatchFiles/RunUAT.sh BuildPlugin -plugin=${plugin_file} -package=${package_folder} -rocket -targetplatforms=Linux+Mac
