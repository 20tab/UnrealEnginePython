
#include "PythonSmartDelegate.h"

#include "UEPyModule.h"

#if WITH_EDITOR
#include "Slate/UEPySWindow.h"
#endif

FPythonSmartDelegate::FPythonSmartDelegate()
{
	py_callable = nullptr;
}

void FPythonSmartDelegate::SetPyCallable(PyObject *callable)
{
	// do not acquire the gil here as we set the callable in python call themselves
	py_callable = callable;
	Py_INCREF(py_callable);
}



bool FPythonSmartDelegate::Tick(float DeltaTime)
{
	FScopePythonGIL gil;
	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"f", DeltaTime);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return false;
	}
	if (PyObject_IsTrue(ret))
	{
		Py_DECREF(ret);
		return true;
	}
	Py_DECREF(ret);
	return false;
}

void FPythonSmartDelegate::Void()
{
	FScopePythonGIL gil;
	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

#if WITH_EDITOR
void FPythonSmartDelegate::PyFOnAssetPostImport(UFactory *factory, UObject *u_object)
{
	FScopePythonGIL gil;
	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"OO", ue_get_python_uobject((UObject *)factory), ue_get_python_uobject(u_object));
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}

void FPythonSmartDelegate::PyFOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
{
	FScopePythonGIL gil;
	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"NO", py_ue_new_swidget<ue_PySWindow>(InRootWindow.ToSharedRef(), &ue_PySWindowType), bIsNewProjectWindow ? Py_True : Py_False);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}
#endif


FPythonSmartDelegate::~FPythonSmartDelegate()
{
	FScopePythonGIL gil;
	Py_XDECREF(py_callable);
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("PythonSmartDelegate callable XDECREF'ed"));
#endif
}