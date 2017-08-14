import os
import subprocess
import sys
import time

UE_VERSIONS = ['4.15', '4.16', '4.17']
PYTHON_VERSIONS = ["C:/Program Files/Python36", "C:/Program Files/Python35", "C:/Python27"]

def msbuild(project, python_version):
    base_environ = os.environ
    base_environ.update({'PYTHONHOME': python_version})
    vs = '"C:/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe"'
    process = subprocess.Popen('{0} {1} /t:Rebuild /p:Configuration="Development Editor" /p:Platform=Win64'.format(vs, project), env=base_environ)
    while process.poll() is None:
        time.sleep(0.5)
    if process.returncode != 0:
        sys.exit(process.returncode)

def commandlet(version, project):
    ue_editor = os.path.join('D:/', 'UE_{0}'.format(version), 'Engine/Binaries/Win64/UE4Editor-Cmd.exe')
    process = subprocess.Popen('{0} D:/{1}/{2}.uproject -run=PyCommandlet D:/{3}/Plugins/UnrealEnginePython/tools/release_check.py'.format(ue_editor, project, project, project))
    while process.poll() is None:
        time.sleep(0.5)
    # ignore return code, has too much different meanings for commandlets

def git(project):
    process = subprocess.Popen('git checkout master', cwd='D:/{0}/Plugins/UnrealEnginePython'.format(project))
    while process.poll() is None:
        time.sleep(0.5)
    if process.returncode != 0:
        sys.exit(process.returncode)
    process = subprocess.Popen('git pull', cwd='D:/{0}/Plugins/UnrealEnginePython'.format(project))
    while process.poll() is None:
        time.sleep(0.5)
    if process.returncode != 0:
        sys.exit(process.returncode)


main_start = time.time()
for ue_version in UE_VERSIONS:
    project = 'PyTest{0}'.format(ue_version.replace('.', ''))
    sln = os.path.join('D:/', project, '{0}.sln'.format(project))
    git(project)
    for python_version in PYTHON_VERSIONS:
        start = time.time()
        print('\n\n***** building {0} for {1} *****\n\n'.format(sln, python_version))
        sys.stdout.flush()
        msbuild(sln, python_version)
        commandlet(ue_version, project)
        end = time.time()
        print('\n\n***** built {0} for {1} in {2} seconds *****\n\n'.format(project, python_version, end-start))

main_end = time.time()
print('release ready after {0} seconds'.format(main_end-main_start))
