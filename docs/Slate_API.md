
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

In the C++ api, each SWidget has its series of FArguments (https://api.unrealengine.com/INT/API/Runtime/Slate/Widgets/Input/SComboBox/FArguments/index.html).

They are the options of the SWidget, and in the python api you pass them as arguments to the constructor (note the pythonization of the names with lower-case-underscore-delimited conversion).

In this case the ClientSize FArguments (http://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SWindow/FArguments/ClientSize/index.html) became 'client_size' and Title (http://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SWindow/FArguments/Title/index.html) became 'title'. Note the shortcut from FVector2D to a simple 2-elements float tuple. (both are supported)


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

Boxes are one of the many Slate ways to organize/align multiple widgets in a containers.

SVerticalBox (https://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SVerticalBox/index.html) is a box allowing you to align widgets vertically in a container (each element of the box is named 'slot', a lot effectively contains the widget as well as various attibutes):

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox

window = SWindow(client_size=(512, 512), title='First Slate Window')

vertical_box = SVerticalBox()

vertical_box.add_slot(STextBlock(text='Hello i am an STextBlock [line 0]'))
vertical_box.add_slot(STextBlock(text='Hello i am an STextBlock [line 1]'))
vertical_box.add_slot(STextBlock(text='Hello i am an STextBlock [line 2]'))

window.set_content(vertical_box)
```

![SVerticalBox](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_SVerticalBox.png)

or in 'visual' style (always note the first opened bracked in the same line of SWindow to fake the parser):

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox

window = SWindow(client_size=(512, 512), title='First Slate Window')(
    SVerticalBox()
    (
        STextBlock(text='Hello i am an STextBlock [line 0]')
    )
    (
        STextBlock(text='Hello i am an STextBlock [line 1]')
    )
    (
        STextBlock(text='Hello i am an STextBlock [line 2]')
    )
)
```

## Slot attributes

We have seen that the mapping between a box and the various widgets is managed by 'slots'. Each slot describes the way a widget is managed in a box (mainly alignment and padding):

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment

window = SWindow(client_size=(512, 512), title='First Slate Window')

vertical_box = SVerticalBox()

vertical_box.add_slot(STextBlock(text='Hello i am an STextBlock [line 0]'), auto_height=True, padding=(10, 20, 10, 100))
vertical_box.add_slot(STextBlock(text='Hello i am an STextBlock [line 1]'), max_height=173, h_align=EHorizontalAlignment.HAlign_Center)
vertical_box.add_slot(STextBlock(text='Hello i am an STextBlock [line 2]'), h_align=EHorizontalAlignment.HAlign_Right, v_align=EVerticalAlignment.VAlign_Bottom, padding=(0, 0, 50, 50))

window.set_content(vertical_box)
```

![Slot](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_Slot.png)

The first slot will have its height automatically mapped to the height of the contained widget (the STextBlock) and a padding of 10 units on the top, 20 on the left, 10 on the right and 100 on the bottom. (more on padding, later).

The second slot starts 100 units below the first one (caused by the padding of the first slot). Its content will be aligned to the center and will have a maximum height of 173 unit (we use 'max' wording here as the SVerticalBox alignment could be forced to make it tinier). 

The third slot is aligned to right and vertically to the bottom with a rght/bottom padding of 50 units.

Useful links:

https://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SVerticalBox/FSlot/index.html

https://api.unrealengine.com/INT/API/Runtime/SlateCore/Types/EHorizontalAlignment/index.html

https://api.unrealengine.com/INT/API/Runtime/SlateCore/Types/EVerticalAlignment/index.html



It is pretty hard to see what is going on here, but adding an SBorder will clarify things...

## SBorder

SBorder is a special container what will add a border around its contained widget:

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox, SBorder
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment

window = SWindow(client_size=(512, 512), title='First Slate Window')

vertical_box = SVerticalBox()

vertical_box.add_slot(SBorder()(STextBlock(text='Hello i am an STextBlock [line 0]')), auto_height=True, padding=(10, 20, 10, 100))
vertical_box.add_slot(SBorder()(STextBlock(text='Hello i am an STextBlock [line 1]')), max_height=173, h_align=EHorizontalAlignment.HAlign_Center)
vertical_box.add_slot(SBorder()(STextBlock(text='Hello i am an STextBlock [line 2]')), h_align=EHorizontalAlignment.HAlign_Right, v_align=EVerticalAlignment.VAlign_Bottom, padding=(0, 0, 50, 20))

window.set_content(SBorder()(vertical_box))
```

![SBorder](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_SBorder.png)

Now slot attributes should be more clear. Note that SBorder has a set_content() method but here we used the 'visual' style (mixed with procedural one):

```python
SBorder()(STextBlock(text='Hello i am an STextBlock [line 0]'))
```

The list of FArguments for SBorder is avalable here: https://api.unrealengine.com/INT/API/Runtime/Slate/Widgets/Layout/SBorder/FArguments/

## Padding

The padding attributes of slots, defines their margin size.

Padding in the python api accepts various forms:

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox, SBorder
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment

window = SWindow(client_size=(512, 512), title='First Slate Window')

vertical_box = SVerticalBox()

vertical_box.add_slot(SBorder()(STextBlock(text='Hello i am an STextBlock [line 0]')), padding=100, v_align=EVerticalAlignment.VAlign_Fill)

window.set_content(SBorder()(vertical_box))
```

passing a single float will specify the same amount of units for top, left, bottom and right:

![Padding](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_Padding.png)

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox, SBorder
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment

window = SWindow(client_size=(512, 512), title='First Slate Window')

vertical_box = SVerticalBox()

vertical_box.add_slot(SBorder()(STextBlock(text='Hello i am an STextBlock [line 0]')), padding=(100, 100), v_align=EVerticalAlignment.VAlign_Fill)

window.set_content(SBorder()(vertical_box))
```

a 2-items float tuple will specify top and left and will force bottom and right to 0:

![Padding 2](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_Padding2.png)

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox, SBorder
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment

window = SWindow(client_size=(512, 512), title='First Slate Window')

vertical_box = SVerticalBox()

vertical_box.add_slot(SBorder()(STextBlock(text='Hello i am an STextBlock [line 0]')), padding=(100, 100, 30, 30), v_align=EVerticalAlignment.VAlign_Fill)

window.set_content(SBorder()(vertical_box))
```

We have alredy seen the 4-items float tuple for specifying top, left, bottom and right:

![Padding 3](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_Padding3.png)

In addition to float/float-tuples you can specify padding using the FMargin struct:

```python
from unreal_engine.structs import Margin
print(Margin.properties())
```
returns

```python
['Left', 'Top', 'Right', 'Bottom']
```

So:

```python
from unreal_engine import SWindow, STextBlock, SVerticalBox, SBorder
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment
from unreal_engine.structs import Margin

window = SWindow(client_size=(512, 512), title='First Slate Window')

vertical_box = SVerticalBox()

vertical_box.add_slot(SBorder()(STextBlock(text='Hello i am an STextBlock [line 0]')), padding=Margin(Left=100, Right=50), v_align=EVerticalAlignment.VAlign_Fill)

window.set_content(SBorder()(vertical_box))
```

![Padding 4](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_Padding4.png)

## SHorizontalBox

SHorizontalBox allows you to horizontally align widgets in a containers:

```python
from unreal_engine import SWindow, STextBlock, SHorizontalBox, SBorder
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment
from unreal_engine.structs import Margin

window = SWindow(client_size=(512, 512), title='First Slate Window')

horizontal_box = SHorizontalBox()

horizontal_box.add_slot(SBorder()(STextBlock(text='Left')), padding=Margin(Left=100, Right=50, Bottom=30), v_align=EVerticalAlignment.VAlign_Fill)
horizontal_box.add_slot(SBorder()(STextBlock(text='Right')), padding=Margin(Left=10, Right=20, Top=50), v_align=EVerticalAlignment.VAlign_Fill)

window.set_content(SBorder()(horizontal_box))
```

![SHorizontalBox](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_SHorizontalBox.png)

You can obviously combine vertical and horizontal boxes (this time using 'visual' style)

```python
from unreal_engine import SWindow, STextBlock, SHorizontalBox, SVerticalBox, SBorder, FLinearColor
from unreal_engine.enums import EVerticalAlignment, EHorizontalAlignment, ETextJustify
from unreal_engine.structs import Margin, SlateColor

SWindow(client_size=(512, 256), title='Slate Window')(
    SVerticalBox()
    (
        SHorizontalBox()
        (
            SBorder()(STextBlock(text='Left top', justification=ETextJustify.Center)), v_align=EVerticalAlignment.VAlign_Fill
        )
        (
            SBorder(border_background_color=SlateColor(SpecifiedColor=FLinearColor.Green))(STextBlock(text='Right top', highlight_text='Right'))
        )
    )
    (
        SHorizontalBox()
        (
            SBorder(border_background_color=SlateColor(SpecifiedColor=FLinearColor(1, 0, 0)), padding=20)
            (
                STextBlock(text='Left bottom')
            ), v_align=EVerticalAlignment.VAlign_Fill
        )
        (
            SBorder()(STextBlock(text='Middle bottom', justification=ETextJustify.Right)), v_align=EVerticalAlignment.VAlign_Center
        )
        (
            SBorder(border_background_color=SlateColor(SpecifiedColor=FLinearColor.Yellow))(STextBlock(text='Right bottom')), v_align=EVerticalAlignment.VAlign_Fill
        )
    )
)
```

![SHorizontalBox2](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_SHorizontalBox2.png)

More infos on SHorizontalBox: https://api.unrealengine.com/INT/API/Runtime/SlateCore/Widgets/SHorizontalBox/index.html

## SGridPanel

This widget allows you to align children in a virtual grid. For each slot you specify the column and row and eventually how much it 'spans' horizontally and verically:

```python
from unreal_engine import SWindow, STextBlock, SGridPanel, SBorder

margin = 40

SWindow(client_size=(512, 512), title='Slate Window')(
    SGridPanel()
    (
        SBorder(padding=margin)(STextBlock(text='cell0')), column=0, row=0
    )
    (
        SBorder(padding=margin)(STextBlock(text='cell1')), column=1, row=0
    )
    (
        SBorder(padding=margin)(STextBlock(text='cell2')), column=2, row=0
    )
    (
        SBorder(padding=margin)(STextBlock(text='cell3')), column=0, row=1
    )
    (
        SBorder(padding=margin)(STextBlock(text='cell4')), column=3, row=1, row_span=3
    )
    (
        SBorder(padding=margin)(STextBlock(text='cell5')), column=2, row=2
    )
    (
        SBorder(padding=margin)(STextBlock(text='cell6')), column=0, row=3, column_span=2
    )
)
```

![SGridPanel](https://github.com/20tab/UnrealEnginePython/raw/master/docs/screenshots/slate_SGridPanel.png)

## SScrollBox

## SButton

## SEditableTextBox

## SImage
