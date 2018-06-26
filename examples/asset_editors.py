import unreal_engine as ue

for _asset in ue.get_all_edited_assets():
    editor = ue.find_editor_for_asset(_asset)
    print('{0} - {1}'.format(editor.get_editor_name(), editor.get_last_activation_time()))
    editor.focus_window()