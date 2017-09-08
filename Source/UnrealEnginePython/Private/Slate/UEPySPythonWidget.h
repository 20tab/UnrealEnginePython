#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"
#include "UEPyFGeometry.h"
#include "UEPyFPaintContext.h"

extern PyTypeObject ue_PySPythonWidgetType;

class SPythonWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SPythonWidget)
	{ }
	SLATE_END_ARGS();

	void Construct(const FArguments& Args)
	{

	}

	virtual int32 OnPaint(const FPaintArgs & Args,
		const FGeometry & AllottedGeometry,
		const FSlateRect & MyClippingRect,
		FSlateWindowElementList & OutDrawElements,
		int32 LayerId,
		const FWidgetStyle & InWidgetStyle,
		bool bParentEnabled) const override
	{

		if (py_callable_paint)
		{
			FPaintContext context(AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

			FScopePythonGIL gil;

			PyObject *ret = PyObject_CallFunction(py_callable_paint, (char *)"O", py_ue_new_fpaint_context(context));
			if (!ret)
			{
				unreal_engine_py_log_error();
				return LayerId + 1;
			}

			Py_DECREF(ret);

		}

		return LayerId + 1;

	}

	virtual void Tick(const FGeometry & AllottedGeometry,
		const double InCurrentTime,
		const float InDeltaTime) override
	{

		if (!py_callable_tick)
			return;

		FScopePythonGIL gil;

		PyObject *ret = PyObject_CallFunction(py_callable_tick, (char *)"Off", py_ue_new_fgeometry(AllottedGeometry), InCurrentTime, InDeltaTime);
		if (!ret)
		{
			unreal_engine_py_log_error();
			return;
		}

		Py_DECREF(ret);
	}

	void SetPythonPaint(PyObject *py_callable)
	{
		py_callable_paint = py_callable;
	}

	void SetPythonTick(PyObject *py_callable)
	{
		py_callable_tick = py_callable;
	}

protected:
	PyObject *py_callable_tick;
	PyObject *py_callable_paint;

};

typedef struct
{
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySPythonWidget;

void ue_python_init_spython_widget(PyObject *);
