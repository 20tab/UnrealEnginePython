import unreal_engine as ue
import json
import os

data = {
    'scope': 'source.python',
    'completions': []
}

for item in ('unreal_engine', 'unreal_engine.classes', 'unreal_engine.structs', 'unreal_engine.enums'):
    data['completions'].append({'trigger': item, 'contents': item})

for item in dir(ue):
    value = 'unreal_engine.{0}'.format(item)
    data['completions'].append({'trigger': value, 'contents': value})

for item in dir(ue.UObject):
    value = 'unreal_engine.UObject.{0}'.format(item)
    data['completions'].append({'trigger': value, 'contents': value})

for _class in ue.all_classes():
    data['completions'].append({'trigger': _class.get_name(), 'contents': _class.get_name()})
    for function in _class.functions():
        value = '{0}.{1}()'.format(_class.get_name(), function)
        data['completions'].append({'trigger': value, 'contents': value})
    for _property in _class.properties():
        value = '{0}.{1}'.format(_class.get_name(), _property)
        data['completions'].append({'trigger': value, 'contents': value})

j = json.dumps(data, indent=4)

with open(os.path.join(ue.get_content_dir(), 'UnrealEnginePython..sublime-completions'), 'w') as f:
    f.write(j)
