from __future__ import absolute_import, division, print_function, unicode_literals

import sys

# python 2 and 3 compatibility. makes all classes new-style in python2 and has
# no effect in python 3.
__metaclass__ = type

# Since this is meant as a wrapper around _unreal_engine import everthing
# into locals.
from _unreal_engine import *

class UnrealEngineLogger:

    def __init__(self, logger):
        self.logger = logger

    def write(self, buf):
        for line in buf.split("\n"):
            self.logger(buf)

    def flush(self):
        return

sys.stdout = UnrealEngineLogger(log)
sys.stderr = UnrealEngineLogger(log_error)

# The following should log to the Unreal Console due to our override of
# sys.stdout.
print("Unreal Python Module Startup Complete")
