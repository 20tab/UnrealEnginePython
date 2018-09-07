
# The Slate API

Slate is the GUI toolkit in Unreal Engine 4. It allows you to create windows, buttons, sliders and all of the graphics elements you see in the editor. It is the base for the Blueprint Widgets too.

UnrealEnginePython allows you to build GUI from python using the Slate api.

The python wrapper development has been sponsored by Kite & Lightning (http://kiteandlightning.la/)

This document assumes a python3 environment. If you are using python2, just ensure to use unicode when you see strings.

## SWidget and ue_PySWidget

SWidget is the base C++ class for all the Slate widgets, it is wrapped in a python object (PyObject) named ue_PySWidget.

Each SWidget exposed to python has its ue_PySWidget representation. You can get the list of exposed SWidget from here: https://github.com/20tab/UnrealEnginePython/tree/master/Source/UnrealEnginePython/Private/Slate (the ones stating with 'UEPyS' prefix)

## SWindow

this is the first widget you generally use as it will be the container for the others. It represents a window:

```python
from unreal_engine import SWindow

window = SWindow(client_size=(512, 512), title='First Slate Window')
```

![SWindow](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_SWindow.png)

As you can see just by instancing Slate you will create and show them.

In the C++ api, each SWidget has its series of FArguments (https://api.unrealengine.com/INT/API/Runtime/Slate/Widgets/Input/SComboBox/FArguments/index.html). They are the options of the SWidget, and in the python api you pass them as arguments to the constructor (note the pythonization of the names with lower-case-underscore-delimited conversion). In this case the ClientSize FArguments (http://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SWindow/FArguments/ClientSize/index.html) became 'client_size' and Title (http://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SWindow/FArguments/Title/index.html) became 'title'. Note the shortcut from FVector2D to a simple 2-elements float tuple. (both are supported)


## STextBlock

STextBlock (https://api.unrealengine.com/INT/API/Runtime/Slate/Widgets/Text/STextBlock/index.html) shows a simple text label:

```python
from unreal_engine import SWindow, STextBlock

window = SWindow(client_size=(512, 512), title='First Slate Window')

text = STextBlock(text='Hello i am an STextBlock')

window.set_content(text)
```

![STextBlock](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_STextBlock.png)

Again using FArguments (https://api.unrealengine.com/INT/API/Runtime/Slate/Widgets/Text/STextBlock/FArguments/index.html) you can (as an example) set the color of the text to red:

```python
from unreal_engine import SWindow, STextBlock, FLinearColor
from unreal_engine.structs import SlateColor

window = SWindow(client_size=(512, 512), title='First Slate Window')

red = SlateColor(SpecifiedColor=FLinearColor(1, 0, 0))

text = STextBlock(text='Hello i am an STextBlock', color_and_opacity=red)

window.set_content(text)
```

![STextBlock red](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_STextBlock_red.png)

## Dynamic binding for FArguments

As in C++ Slate, you can map 'delegates' (well, callables in python) to some of the FArguments. As an example you can dynamically change the 'text' attribute of an STextBlock to be constantly updated with the current time:

```python
from unreal_engine import SWindow, STextBlock, FLinearColor
from unreal_engine.structs import SlateColor
import time

window = SWindow(client_size=(512, 512), title='First Slate Window')

red = SlateColor(SpecifiedColor=FLinearColor(1, 0, 0))

text = STextBlock(text=lambda: str(time.time()), color_and_opacity=red)

window.set_content(text)
```

Obviously instead of the lambda you could use a plain function (any callable will works):

```python
from unreal_engine import SWindow, STextBlock, FLinearColor
from unreal_engine.structs import SlateColor
import time

def what_time_is_it():
    return str(time.time())

window = SWindow(client_size=(512, 512), title='First Slate Window')

red = SlateColor(SpecifiedColor=FLinearColor(1, 0, 0))

text = STextBlock(text=what_time_is_it, color_and_opacity=red)

window.set_content(text)
```

## Content assignment shortcut

In the previous examples we have seen how we added the STextBlock to the SWindow by using set_content().

This is a very 'procedural' way of dealing with GUIs. Technically Slate (as well as other toolkits) enforce the developer to a very 'visual' style when describing user interfaces. For this reason, if you do not like the 'procedural' approach you can abuse a specific feature of ue_PySWidget objects: when they are containers, they are 'callable', and calling them will internally call set_content() (or similar).

Based on this you can rewrite the first STextBlock example in this way:

```python
from unreal_engine import SWindow, STextBlock

window = SWindow(client_size=(512, 512), title='First Slate Window')(STextBlock(text='Hello i am an STextBlock'))
```
or 'visually' better (note the opened bracked in the same line of SWindow)

```python
from unreal_engine import SWindow, STextBlock

window = SWindow(client_size=(512, 512), title='First Slate Window')(
    STextBlock(text='Hello i am an STextBlock')
)
```

## SVerticalBox

## SHorizontalBox

## SButton
