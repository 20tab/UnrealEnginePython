import sys
import unittest
import unreal_engine as ue
import os.path

# ue.sandbox_exec(ue.find_plugin('UnrealEnginePython').get_base_dir() + '/run_tests.py')

# setup classic stdout/stderr
class UnrealEngineOutput:
    def __init__(self, logger):
        self.logger = logger
    def write(self, buf):
        self.logger(buf)
    def flush(self):
        return

sys.stdout = UnrealEngineOutput(ue.log)
sys.stderr = UnrealEngineOutput(ue.log_error)

uep_base = ue.find_plugin('UnrealEnginePython').get_base_dir()

loader = unittest.TestLoader()
tests = loader.discover(os.path.join(uep_base, 'tests'))

runner = unittest.runner.TextTestRunner()
runner.run(tests)