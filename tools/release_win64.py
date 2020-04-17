import os
import subprocess
import sys
import time
import shutil
import zipfile

UE_VERSIONS = ['4.20', '4.21', '4.22']
PYTHON_VERSIONS = ["C:/Program Files/Python37", "C:/Program Files/Python36", "C:/Python27"]
MSBUILD = 'C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/MSBuild/15.0/Bin/MSBuild.exe'
UE_PATH = 'C:/Program Files/Epic Games'
PROJECTS_PATH = 'C:/Users/rober/Documents/Unreal Projects'

RELEASE_DIR = sys.argv[1].rstrip('/')

def zipdir(path, zh, base):
    for root, dirs, files in os.walk(path):
        for file in files:
            filename = os.path.join(root, file)
            zh.write(filename, os.path.relpath(filename, base))

def msbuild(project, python_version):
    base_environ = os.environ
    base_environ.update({'PYTHONHOME': python_version})
    base_environ.update({'UEP_ENABLE_UNITY_BUILD': '1'})
    vs = '"{}"'.format(MSBUILD)
    process = subprocess.Popen('{0} "{1}" /m /t:Rebuild /p:Configuration="Development Editor" /p:Platform=Win64'.format(vs, project), env=base_environ)
    while process.poll() is None:
        time.sleep(0.5)
    if process.returncode != 0:
        sys.exit(process.returncode)

def commandlet(version, project):
    ue_editor = os.path.join(UE_PATH, 'UE_{0}'.format(version), 'Engine/Binaries/Win64/UE4Editor-Cmd.exe')
    process = subprocess.Popen('{0} {1}/{2}/{3}.uproject -run=PyCommandlet {1}/{4}/Plugins/UnrealEnginePython/tools/release_check.py'.format(ue_editor, PROJECTS_PATH, project, project, project))
    while process.poll() is None:
        time.sleep(0.5)
    # ignore return code, has too much different meanings for commandlets

def git(project):
    process = subprocess.Popen('git checkout master', cwd='{0}/{1}/Plugins/UnrealEnginePython'.format(PROJECTS_PATH, project))
    while process.poll() is None:
        time.sleep(0.5)
    if process.returncode != 0:
        sys.exit(process.returncode)
    process = subprocess.Popen('git pull', cwd='{0}/{1}/Plugins/UnrealEnginePython'.format(PROJECTS_PATH, project))
    while process.poll() is None:
        time.sleep(0.5)
    if process.returncode != 0:
        sys.exit(process.returncode)


main_start = time.time()
for ue_version in UE_VERSIONS:
    project = 'PyTest{0}'.format(ue_version.replace('.', ''))
    sln = os.path.join(PROJECTS_PATH, project, '{0}.sln'.format(project))
    git(project)
    for python_version in PYTHON_VERSIONS:
        python_sanitized = os.path.basename(python_version).lower()
        start = time.time()
        print('\n\n***** building {0} for {1} *****\n\n'.format(sln, python_version))
        sys.stdout.flush()
        msbuild(sln, python_version)
        commandlet(ue_version, project)
        end = time.time()
        for item in ('UE4Editor.modules', 'UE4Editor-UnrealEnginePython.dll', 'UE4Editor-PythonConsole.dll', 'UE4Editor-PythonEditor.dll', 'UE4Editor-PythonAutomation.dll'):
            shutil.copyfile('{0}/{1}/Plugins/UnrealEnginePython/Binaries/Win64/{2}'.format(PROJECTS_PATH, project, item), '{0}/UnrealEnginePython/Binaries/Win64/{1}'.format(RELEASE_DIR, item))
            if python_sanitized == 'python36':
                shutil.copyfile('{0}/{1}/Plugins/UnrealEnginePython/Binaries/Win64/{2}'.format(PROJECTS_PATH, project, item), '{0}/Embedded/UnrealEnginePython/Binaries/Win64/{1}'.format(RELEASE_DIR, item))
        filename = 'UnrealEnginePython_{0}_{1}_{2}_win64.zip'.format(os.path.basename(RELEASE_DIR), ue_version.replace('.','_'), python_sanitized)
        zh = zipfile.ZipFile(os.path.join(RELEASE_DIR, filename), 'w', zipfile.ZIP_DEFLATED)
        zipdir(os.path.join(RELEASE_DIR, 'UnrealEnginePython'), zh, RELEASE_DIR)
        zh.close()
        if python_sanitized == 'python36':
            filename = 'UnrealEnginePython_{0}_{1}_{2}_embedded_win64.zip'.format(os.path.basename(RELEASE_DIR), ue_version.replace('.','_'), python_sanitized)
            zh = zipfile.ZipFile(os.path.join(RELEASE_DIR, filename), 'w', zipfile.ZIP_DEFLATED)
            zipdir(os.path.join(RELEASE_DIR, 'Embedded/UnrealEnginePython'), zh, os.path.join(RELEASE_DIR, 'Embedded'))
            zh.close()
        print('\n\n***** built {0} for {1} in {2} seconds [{3}]*****\n\n'.format(project, python_version, end-start, filename))

main_end = time.time()
print('release ready after {0} seconds'.format(main_end-main_start))
