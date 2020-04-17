import unreal_engine as ue

def open_submenu001(builder):
    builder.begin_section('submenu001', 'i am a tooltip')
    builder.add_menu_entry('sub_one', 'tooltip', lambda: ue.log('hello from submenu001'))
    builder.add_menu_entry('sub_one_2', 'tooltip 2', lambda: ue.log('hello again'))
    builder.end_section()

def open_sub_submenu(builder):
    builder.begin_section('sub_submenu003', 'i am a tooltip for the submenu')
    builder.add_menu_entry('sub_sub_three', 'tooltip', lambda: ue.log('hello from sub_submenu003'))
    builder.end_section()

def open_submenu002(builder):
    builder.begin_section('submenu002', 'i am a tooltip')
    builder.add_menu_entry('sub_two', 'tooltip', lambda: ue.log('hello from submenu002'))
    builder.add_sub_menu('sub sub menu', 'tooltip !', open_sub_submenu)
    builder.end_section()


def open_menu(builder):
    builder.begin_section('test1', 'test2')
    builder.add_menu_entry('one', 'two', lambda: ue.log('ciao 1'))
    builder.add_sub_menu('i am a submenu', 'tooltip for the submenu', open_submenu001)
    builder.add_menu_entry('three', 'four', lambda: ue.log('ciao 2'))
    builder.add_sub_menu('i am another submenu', 'tooltip for the second submenu', open_submenu002)
    builder.end_section()


ue.add_menu_bar_extension('SimpleMenuBarExtension', open_menu)