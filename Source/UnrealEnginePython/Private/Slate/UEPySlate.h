#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "UEPySWidget.h"
#include "UEPySCompoundWidget.h"
#include "UEPySWindow.h"
#include "UEPySBorder.h"
#include "UEPySButton.h"
#include "UEPySLeafWidget.h"
#include "UEPySTextBlock.h"
#include "UEPySEditableTextBox.h"

#include "UEPySlate.generated.h"


void ue_python_init_slate(PyObject *);

UCLASS()
class UPythonSlateDelegate : public UPythonDelegate
{
	GENERATED_BODY()

public:
	FReply OnMouseButtonDown(const FGeometry &geometry, const FPointerEvent &pointer_event);
	FReply OnClicked();
};

#endif

