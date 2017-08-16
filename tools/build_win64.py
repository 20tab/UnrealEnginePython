import os
import subprocess
import sys
import time


def msbuild(project, python_version):
    base_environ = os.environ
    base_environ.update({'PYTHONHOME': python_version})
    vs = '"C:/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe"'
    process = subprocess.Popen('{0} {1} /t:Rebuild /p:Configuration="Development Editor" /p:Platform=Win64'.format(vs, project), env=base_environ)
    while process.poll() is None:
        time.sleep(0.5)
    if process.returncode != 0:
        sys.exit(process.returncode)

main_start = time.time()
sln = sys.argv[1]
python_version = sys.argv[2]

print('\n\n***** building {0} for {1} *****\n\n'.format(sln, python_version))
sys.stdout.flush()
msbuild(sln, python_version)

main_end = time.time()
print('build ready after {0} seconds'.format(main_end-main_start))
