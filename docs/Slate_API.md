
# The Slate API

Slate is the GUI toolkit in Unreal Engine 4. It allows you to create windows, buttons, sliders and all of the graphics elements you see in the editor. It is the base for the Blueprint Widgets too.

UnrealEnginePython allows you to build GUI from python using the Slate api.

The python wrapper development has been sponsored by Kite & Lightning (http://kiteandlightning.la/)

This document assumes a python3 environment. If you are using python2, just ensure to use unicode when you see strings.

## SWidget and ue_PySWidget

SWidget is the base C++ class for all the Slate widgets, it is wrapped in a python object (PyObject) named ue_PySWidget.

Each SWidget exposed to python has its ue_PySWidget representation. You can get the list of exposed SWidget from here: https://github.com/20tab/UnrealEnginePython/tree/master/Source/UnrealEnginePython/Private/Slate (the ones stating with 'UEPyS' prefix)

## SWindow

this is the first widget you generally use as it will be the container for the others. It represent a window:

```python
from unreal_engine import SWindow

window = SWindow(client_size=(512, 512), title='First Slate Window')
```

![SWindow](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_SWindow.png)

As you can see just by instancing Slate you will create and show them.

In the C++ api, each SWidget has its series of FArguments (https://api.unrealengine.com/INT/API/Runtime/Slate/Widgets/Input/SComboBox/FArguments/index.html). They are the options of the SWidget, and in the python api you pass them as arguments to the constructor (note the pythonization of the names with lower-case-underscore-delimited conversion). In this case the ClientSize FArguments (http://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SWindow/FArguments/ClientSize/index.html) became 'client_size' and Title (http://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SWindow/FArguments/Title/index.html) became 'title'. Note the shortcut from FVector2D to a simple 2-elements float tuple. (both are supported)


## STextBlock


## SButton
