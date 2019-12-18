#pragma once

#include "UEPySCompoundWidget.h"

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"


extern PyTypeObject ue_PySPythonWidgetType;

class SPythonWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SPythonWidget)
	{
	}
	SLATE_END_ARGS();

	void Construct(const FArguments& Args)
	{
	}

	virtual bool SupportsKeyboardFocus() const override
	{
		return true;
	}

	virtual FReply OnKeyChar(const FGeometry & MyGeometry, const FCharacterEvent & InCharacterEvent) override
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"on_key_char"))
			return FReply::Unhandled();

		PyObject *py_callable_on_key_char = PyObject_GetAttrString(self, (char *)"on_key_char");
		if (!PyCallable_Check(py_callable_on_key_char))
		{
			UE_LOG(LogPython, Error, TEXT("on_key_char is not a callable"));
			return FReply::Unhandled();
		}

		PyObject *ret = PyObject_CallFunction(py_callable_on_key_char, (char *)"NN", py_ue_new_fgeometry(MyGeometry), py_ue_new_fcharacter_event(InCharacterEvent));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return FReply::Unhandled();
		}

		if (ret == Py_False)
		{
			Py_DECREF(ret);
			return FReply::Unhandled();
		}
		Py_DECREF(ret);
		return FReply::Handled();
	}

	virtual FReply OnKeyDown(const FGeometry & MyGeometry, const FKeyEvent & InKeyEvent) override
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"on_key_down"))
			return FReply::Unhandled();

		PyObject *py_callable_on_key_down = PyObject_GetAttrString(self, (char *)"on_key_down");
		if (!PyCallable_Check(py_callable_on_key_down))
		{
			UE_LOG(LogPython, Error, TEXT("on_key_down is not a callable"));
			return FReply::Unhandled();
		}

		PyObject *ret = PyObject_CallFunction(py_callable_on_key_down, (char *)"NN", py_ue_new_fgeometry(MyGeometry), py_ue_new_fkey_event(InKeyEvent));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return FReply::Unhandled();
		}

		if (ret == Py_False)
		{
			Py_DECREF(ret);
			return FReply::Unhandled();
		}
		Py_DECREF(ret);
		return FReply::Handled();
	}

	virtual FReply OnMouseMove(const FGeometry & MyGeometry, const FPointerEvent & MyEvent) override
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"on_mouse_move"))
			return FReply::Unhandled();

		PyObject *py_callable_on_mouse_move = PyObject_GetAttrString(self, (char *)"on_mouse_move");
		if (!PyCallable_Check(py_callable_on_mouse_move))
		{
			UE_LOG(LogPython, Error, TEXT("on_mouse_move is not a callable"));
			return FReply::Unhandled();
		}

		PyObject *ret = PyObject_CallFunction(py_callable_on_mouse_move, (char *)"NN", py_ue_new_fgeometry(MyGeometry), py_ue_new_fpointer_event(MyEvent));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return FReply::Unhandled();
		}

		if (ret == Py_False)
		{
			Py_DECREF(ret);
			return FReply::Unhandled();
		}
		Py_DECREF(ret);
		return FReply::Handled();
	}

	virtual FReply OnMouseWheel(const FGeometry & MyGeometry, const FPointerEvent & MyEvent) override
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"on_mouse_wheel"))
			return FReply::Unhandled();

		PyObject *py_callable_on_mouse_wheel = PyObject_GetAttrString(self, (char *)"on_mouse_wheel");
		if (!PyCallable_Check(py_callable_on_mouse_wheel))
		{
			UE_LOG(LogPython, Error, TEXT("on_mouse_wheel is not a callable"));
			return FReply::Unhandled();
		}

		PyObject *ret = PyObject_CallFunction(py_callable_on_mouse_wheel, (char *)"NN", py_ue_new_fgeometry(MyGeometry), py_ue_new_fpointer_event(MyEvent));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return FReply::Unhandled();
		}

		if (ret == Py_False)
		{
			Py_DECREF(ret);
			return FReply::Unhandled();
		}
		Py_DECREF(ret);
		return FReply::Handled();
	}

	virtual FReply OnMouseButtonDown(const FGeometry & MyGeometry, const FPointerEvent & MyEvent) override
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"on_mouse_button_down"))
			return FReply::Unhandled();

		PyObject *py_callable_on_mouse_button_down = PyObject_GetAttrString(self, (char *)"on_mouse_button_down");
		if (!PyCallable_Check(py_callable_on_mouse_button_down))
		{
			UE_LOG(LogPython, Error, TEXT("on_mouse_button_down is not a callable"));
			return FReply::Unhandled();
		}

		PyObject *ret = PyObject_CallFunction(py_callable_on_mouse_button_down, (char *)"NN", py_ue_new_fgeometry(MyGeometry), py_ue_new_fpointer_event(MyEvent));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return FReply::Unhandled();
		}

		if (ret == Py_False)
		{
			Py_DECREF(ret);
			return FReply::Unhandled();
		}
		Py_DECREF(ret);
		return FReply::Handled();
	}

	virtual FReply OnMouseButtonUp(const FGeometry & MyGeometry, const FPointerEvent & MyEvent) override
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"on_mouse_button_up"))
			return FReply::Unhandled();

		PyObject *py_callable_on_mouse_button_up = PyObject_GetAttrString(self, (char *)"on_mouse_button_up");
		if (!PyCallable_Check(py_callable_on_mouse_button_up))
		{
			UE_LOG(LogPython, Error, TEXT("on_mouse_button_up is not a callable"));
			return FReply::Unhandled();
		}

		PyObject *ret = PyObject_CallFunction(py_callable_on_mouse_button_up, (char *)"NN", py_ue_new_fgeometry(MyGeometry), py_ue_new_fpointer_event(MyEvent));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return FReply::Unhandled();
		}

		if (ret == Py_False)
		{
			Py_DECREF(ret);
			return FReply::Unhandled();
		}
		Py_DECREF(ret);
		return FReply::Handled();
	}


	virtual int32 OnPaint(const FPaintArgs & Args,
		const FGeometry & AllottedGeometry,
		const FSlateRect & MyClippingRect,
		FSlateWindowElementList & OutDrawElements,
		int32 LayerId,
		const FWidgetStyle & InWidgetStyle,
		bool bParentEnabled) const override
	{
		int32 MaxLayer = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"paint"))
			return MaxLayer;

		PyObject *py_callable_paint = PyObject_GetAttrString(self, (char *)"paint");
		if (!PyCallable_Check(py_callable_paint))
		{
			UE_LOG(LogPython, Error, TEXT("paint is not a callable"));
			return MaxLayer;
		}

		FPaintContext context(AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

		PyObject *ret = PyObject_CallFunction(py_callable_paint, (char *)"N", py_ue_new_fpaint_context(context));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return MaxLayer;
		}

		Py_DECREF(ret);

		return MaxLayer + 1;

	}

	virtual void Tick(const FGeometry & AllottedGeometry,
		const double InCurrentTime,
		const float InDeltaTime) override
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"tick"))
			return;

		PyObject *py_callable_tick = PyObject_GetAttrString(self, (char *)"tick");
		if (!PyCallable_Check(py_callable_tick))
		{
			UE_LOG(LogPython, Error, TEXT("tick is not a callable"));
			return;
		}

		PyObject *ret = PyObject_CallFunction(py_callable_tick, (char *)"Nff", py_ue_new_fgeometry(AllottedGeometry), InCurrentTime, InDeltaTime);
		if (!ret)
		{
			unreal_engine_py_log_error();
			return;
		}

		Py_DECREF(ret);
	}

	void SetPyObject(PyObject *py_obj)
	{
		Py_XDECREF(self);
		Py_INCREF(py_obj);
		self = py_obj;
	}

	void SetActive(bool bActive)
	{
		if (bActive && !ActiveTimerHandle.IsValid())
		{
			ActiveTimerHandle = RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SPythonWidget::EnsureTick));
		}
		else if (!bActive && ActiveTimerHandle.IsValid())
		{
			UnRegisterActiveTimer(ActiveTimerHandle.Pin().ToSharedRef());
		}
	}

	void SetContent(TSharedRef<SWidget> InContent)
	{
		ChildSlot
			[
				InContent
			];
	}

	void ClearContent()
	{
		ChildSlot.DetachWidget();
	}

protected:
	PyObject * self;

	TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;

	EActiveTimerReturnType EnsureTick(double InCurrentTime, float InDeltaTime)
	{
		return EActiveTimerReturnType::Continue;
	}
};

typedef struct
{
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySPythonWidget;

void ue_python_init_spython_widget(PyObject *);
