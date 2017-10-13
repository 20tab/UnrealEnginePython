import unreal_engine as ue

from unreal_engine.classes import Object, StrProperty, IntProperty
from unreal_engine import CLASS_CONFIG, CLASS_DEFAULT_CONFIG, CPF_CONFIG

def config(arg):
    config_name = None
    def wrapper(u_class):
        cflags = u_class.class_get_flags()
        u_class.class_set_flags(cflags|CLASS_CONFIG)
        if config_name:
            u_class.class_set_config_name(config_name)
        return u_class
    if isinstance(arg, str):
        config_name = arg
        return wrapper
    return wrapper(arg)
    
def default_config(u_class):
    cflags = u_class.class_get_flags()
    u_class.class_set_flags(cflags|CLASS_DEFAULT_CONFIG)
    return u_class

@config('DumbConfig')
@default_config
class FooSettings(Object):

    HelloWorld = StrProperty

    FooWorld = [IntProperty]

    def __init__(self):
        self.add_property_flags('HelloWorld', CPF_CONFIG)
        self.get_uproperty('HelloWorld').set_metadata('Category', 'CategoryTest001')
        self.HelloWorld = 'Hello World 001'

        self.add_property_flags('FooWorld', CPF_CONFIG)
        self.get_uproperty('FooWorld').set_metadata('Category', 'CategoryTest002')
        self.FooWorld = [17, 22, 30]


ue.register_settings('Project', 'FooBar', 'General', 'General DisplayName', 'General Description', ue.get_mutable_default(FooSettings))
