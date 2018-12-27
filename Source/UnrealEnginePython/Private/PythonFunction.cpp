
#include "PythonFunction.h"
#include "UEPyModule.h"


void UPythonFunction::SetPyCallable(PyObject *callable)
{
	py_callable = callable;
	Py_INCREF(py_callable);
}


#if ENGINE_MINOR_VERSION > 18
void UPythonFunction::CallPythonCallable(UObject *Context, FFrame& Stack, RESULT_DECL)
#else
void UPythonFunction::CallPythonCallable(FFrame& Stack, RESULT_DECL)
#endif
{

	FScopePythonGIL gil;

#if ENGINE_MINOR_VERSION <= 18
	UObject *Context = Stack.Object;
#endif

	UPythonFunction *function = static_cast<UPythonFunction *>(Stack.CurrentNativeFunction);

	bool on_error = false;
	bool is_static = function->HasAnyFunctionFlags(FUNC_Static);
    FOutParmRec *OutParms = nullptr;

	// count the number of arguments
	Py_ssize_t argn = (Context && !is_static) ? 1 : 0;
	TFieldIterator<UProperty> IArgs(function);
	for (; IArgs && ((IArgs->PropertyFlags & (CPF_Parm | CPF_OutParm)) == CPF_Parm); ++IArgs) {
		argn++;
	}
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Initializing %d parameters"), argn);
#endif
	PyObject *py_args = PyTuple_New(argn);
	argn = 0;

	if (Context && !is_static) {
		PyObject *py_obj = (PyObject *)ue_get_python_uobject(Context);
		if (!py_obj) {
			unreal_engine_py_log_error();
			on_error = true;
		}
		else {
			Py_INCREF(py_obj);
			PyTuple_SetItem(py_args, argn++, py_obj);
		}
	}

	uint8 *frame = Stack.Locals;

	if (*Stack.Code == EX_EndFunctionParms)
    {   // native call
		for (UProperty *prop = (UProperty *)function->Children; prop; prop = (UProperty *)prop->Next) {
			if (prop->PropertyFlags & CPF_OutParm)
				continue;
			if (!on_error) {
				PyObject *arg = ue_py_convert_property(prop, (uint8 *)Stack.Locals, 0);
				if (!arg) {
					unreal_engine_py_log_error();
					on_error = true;
				}
				else {
					PyTuple_SetItem(py_args, argn++, arg);
				}
			}
		}
	}
	else
    {   // blueprint call
        // largely copied from ScriptCore.cpp::CallFunction - for BP calls, we need to set up some of the FOutParmRec stuff ourselves
		frame = (uint8 *)FMemory_Alloca(function->PropertiesSize);
		FMemory::Memzero(frame, function->PropertiesSize);
		for (UProperty *prop = (UProperty *)function->Children; *Stack.Code != EX_EndFunctionParms; prop = (UProperty *)prop->Next) 
        {
			Stack.Step(Stack.Object, prop->ContainerPtrToValuePtr<uint8>(frame));
			if (prop->PropertyFlags & CPF_OutParm)
            {
                FOutParmRec *rec = (FOutParmRec*)FMemory_Alloca(sizeof(FOutParmRec));
                rec->Property = prop;
                rec->PropAddr = Stack.MostRecentPropertyAddress;
                rec->NextOutParm = OutParms;
                OutParms = rec;
				continue;
            }
			if (!on_error) {
				PyObject *arg = ue_py_convert_property(prop, frame, 0);
				if (!arg) {
					unreal_engine_py_log_error();
					on_error = true;
				}
				else {
					PyTuple_SetItem(py_args, argn++, arg);
				}
			}
		}
	}

	Stack.Code++;

	if (on_error || !function->py_callable) {
		Py_DECREF(py_args);
		return;
	}

	PyObject *ret = PyObject_CallObject(function->py_callable, py_args);
	Py_DECREF(py_args);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}

    // get return value and/or any out params - for convenience, if a single item is returned, wrap it in a tuple so that we can process
    // multi-out params and single out params with one block of code
    bool wrapped_ret = false;
    if (!PyTuple_Check(ret))
    {
		PyObject *wrapped = PyTuple_New(1);
        PyTuple_SetItem(wrapped, 0, ret);
        ret = wrapped;
    }

    int nret = PyTuple_Size(ret);
    int tuple_index = 0;
    for (TFieldIterator<UProperty> It(function); It && (It->PropertyFlags & CPF_Parm); ++It)
    {
        if (!(It->PropertyFlags & CPF_OutParm))
            continue;
        if (tuple_index >= nret)
        {
            UE_LOG(LogPython, Error, TEXT("Python function %s didn't return enough values"), *function->GetFName().ToString());
            break;
        }

        UProperty *prop = *It;
        PyObject *py_obj = PyTuple_GetItem(ret, tuple_index);
        if (prop->PropertyFlags & CPF_ReturnParm)
        {   // handle the return value specially by having it write directly to the stack
            if (!ue_py_convert_pyobject(py_obj, prop, (uint8*)RESULT_PARAM - prop->GetOffset_ForUFunction(), 0))
            {
                UE_LOG(LogPython, Error, TEXT("Invalid return value type for function %s"), *function->GetFName().ToString());
            }
        }
        else
        {   // Find the given FOutParmRec for this property - look in the stack first
            uint8 *out_frame = nullptr;
            for (FOutParmRec *rec = Stack.OutParms; rec != nullptr; rec = rec->NextOutParm)
            {
                if (rec->Property == prop)
                {
                    out_frame = rec->PropAddr - prop->GetOffset_ForUFunction();
                    break;
                }
            }
            if (!out_frame)
            {   // look in our local out parms next
                for (FOutParmRec *rec = OutParms; rec != nullptr; rec = rec->NextOutParm)
                {
                    if (rec->Property == prop)
                    {
                        out_frame = rec->PropAddr - prop->GetOffset_ForUFunction();
                        break;
                    }
                }
            }
            if (!out_frame)
            {   // default to our current frame
                out_frame = frame;
            }

            if (!ue_py_convert_pyobject(py_obj, prop, out_frame, 0))
            {
                UE_LOG(LogPython, Error, TEXT("Failed to convert output property for function %s"), *function->GetFName().ToString());
            }
        }
        tuple_index++;
    }
	Py_DECREF(ret);
}

UPythonFunction::~UPythonFunction()
{
	FScopePythonGIL gil;
	Py_XDECREF(py_callable);
	FUnrealEnginePythonHouseKeeper::Get()->UnregisterPyUObject(this);
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("PythonFunction callable %p XDECREF'ed"), this);
#endif
}
