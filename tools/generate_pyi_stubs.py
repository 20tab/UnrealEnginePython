import re
import os
import sys
import unreal_engine as ue


# TODO: there are invalid names like 'IsInAir?' and 'EUserInterfaceActionType.None'
VALID_NAME_PATTERN = re.compile('^[_a-zA-Z][_a-zA-Z0-9]*$')
FILTERED_NAMES = {
    '__new__', '__doc__',
    '__str__', '__repr__', '__name__',
    '__loader__', '__spec__', '__package__',
    '__loader__', '__hash__', '__weakref__'
}

# This list is kinda reliable for descriptors implemented in C, it's ugly to get their types
# https://docs.python.org/3/library/inspect.html#fetching-attributes-statically
DESCRIPTORS = {'getset_descriptor', 'member_descriptor'}


def is_valid_name(name):
    return VALID_NAME_PATTERN.match(name) is not None and name not in FILTERED_NAMES


def filter_names(names):
    return (name for name in names if is_valid_name(name))


def filter_attributes(attributes):
    return {name: value for name, value in attributes.items() if is_valid_name(name)}


def get_type_name(obj):
    type_ = type(obj)
    try:
        return type_.__qualname__
    except AttributeError:
        return type_.__name__


def is_class(t):
    return isinstance(t, type)


def is_callable(obj):
    return hasattr(obj, '__call__')


def write_function(file, name, indent):
    file.write(indent)
    # TODO: ideally methods functions should be handled differently, like have self
    # but we don't know arguments and if it's staticmethod or classmethod anyway
    file.write("def {}(*args, **kwargs) -> 'typing.Any': ...\n".format(name))


def write_variable(file, name, value, indent):
    type_name = get_type_name(value)
    file.write(indent)

    if isinstance(value, property) or value is None or type_name in DESCRIPTORS:
        variable_type = 'typing.Any'
    else:
        variable_type = '{}'.format(type_name)

    file.write(
        "{}: '{}'\n".format(name, variable_type)
    )


def write_class(file, name, value, indent):
    file.write(indent)
    file.write("class {}:\n".format(name))
    attributes = filter_attributes(vars(value))

    if not attributes:
        file.write(indent)
        file.write("    pass\n")
    else:
        for attribute_name, attribute_value in attributes.items():
            write_object(file, attribute_name, attribute_value, indent + '    ')

    file.write('\n')
    file.write('\n')


def write_object(file, name, value, indent):
    # TODO: https://github.com/20tab/UnrealEnginePython/issues/394 ESlateEnums contains invalid attributes
    if not is_valid_name(name):
        return

    if is_class(value):
        write_class(file, name, value, indent)
    elif is_callable(value):
        write_function(file, name, indent)
    else:
        write_variable(file, name, value, indent)


def write_ue_classes(file, classes):
    for class_ in classes:
        file.write("class {}:\n".format(class_.get_name()))

        attributes_count = 0

        for property_ in filter_names(class_.properties()):
            file.write("    ")
            file.write("{}: 'typing.Any'\n".format(property_))
            attributes_count += 1

        for function_ in filter_names(class_.functions()):
            file.write("    ")
            file.write("def {}(*args, **kwargs) -> 'typing.Any': pass\n".format(function_))
            attributes_count += 1

        if attributes_count == 0:
            file.write("    ")
            file.write("pass\n")

        file.write("\n")


def generate_pyi_stubs(directory, include_reflection=False):
    """
    Generates pyi file. Note include_reflection has not been implemented yet.
    """
    # include_reflection is still WIP and bit unusable, because:
    # * the result file is very large and PyCharm by default ignores such files
    # * it contains some invalid identifiers ('IsInAir?')
    if include_reflection:
        raise NotImplementedError

    package_dir = os.path.join(directory, 'unreal_engine')
    os.mkdir(package_dir)

    init_path = os.path.join(package_dir, '__init__.pyi')
    with open(init_path, mode='w') as file:
        file.write('import typing')
        file.write('\n\n')

        ue_public = vars(ue)
        for name, value in filter_attributes(ue_public).items():
            write_object(file, name, value, '')
            file.write('\n')

    if include_reflection:
        classes_path = os.path.join(package_dir, 'classes.pyi')
        with open(classes_path, mode='w') as file:
            write_ue_classes(file, ue.all_classes())


if __name__ == '__main__':
    generate_pyi_stubs(sys.argv[1])
