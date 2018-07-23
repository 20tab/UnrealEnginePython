import unreal_engine as ue
from unreal_engine.classes import CurveFloatFactory, CurveVectorFactory
from unreal_engine.structs import RichCurve, RichCurveKey
import time

factory = CurveFloatFactory()

curve = factory.factory_create_new('/Game/CustomFloatCurve' + str(int(time.time())))

keys = []

for i in range(0, 100):
    keys.append(RichCurveKey(Time=i, Value=i))

curve.FloatCurve.Keys = keys

curve.post_edit_change()

ue.open_editor_for_asset(curve)

factory = CurveVectorFactory()

curve = factory.factory_create_new('/Game/CustomVectorCurve' + str(int(time.time())))

# one curve list for each axis
keys_x = []
keys_y = []
keys_z = []

for i in range(0, 100):
    keys_x.append(RichCurveKey(Time=i * 0.1, Value=i * 0.1))
    keys_y.append(RichCurveKey(Time=i * 0.1, Value=i * 0.1))
    keys_z.append(RichCurveKey(Time=i * 0.1, Value=i * 0.1))

# FloatCurves is a native array, use property index (0=x, 1=y, 2=z)
curve.set_property('FloatCurves', RichCurve(Keys=keys_x), 0)
curve.set_property('FloatCurves', RichCurve(Keys=keys_y), 1)
curve.set_property('FloatCurves', RichCurve(Keys=keys_z), 2)

curve.post_edit_change()

ue.open_editor_for_asset(curve)
