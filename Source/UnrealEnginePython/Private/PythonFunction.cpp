
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

	// is it a blueprint call ?
    UProperty *first_out_prop = nullptr;
	if (*Stack.Code == EX_EndFunctionParms) {
		for (UProperty *prop = (UProperty *)function->Children; prop; prop = (UProperty *)prop->Next) {
			if (prop->PropertyFlags & CPF_OutParm)
            {
                if (!first_out_prop)
                    first_out_prop = prop;
				continue;
            }
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
	else {
		//UE_LOG(LogPython, Warning, TEXT("BLUEPRINT CALL"));
		frame = (uint8 *)FMemory_Alloca(function->PropertiesSize);
		FMemory::Memzero(frame, function->PropertiesSize);
		for (UProperty *prop = (UProperty *)function->Children; *Stack.Code != EX_EndFunctionParms; prop = (UProperty *)prop->Next) {
			Stack.Step(Stack.Object, prop->ContainerPtrToValuePtr<uint8>(frame));
			if (prop->PropertyFlags & CPF_OutParm)
            {
                if (!first_out_prop)
                    first_out_prop = prop;
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

	// get return value and/or any out params
    if (PyTuple_Check(ret))
    {   // function has multiple output params or a return value and one or more output params
        int nret = PyTuple_Size(ret);
        int tuple_index = 0;
        for( TFieldIterator<UProperty> It(function); It && (It->PropertyFlags & CPF_Parm); ++It )
        {
            if( It->PropertyFlags & CPF_OutParm )
            {
                if (tuple_index >= nret)
                {
                    UE_LOG(LogPython, Error, TEXT("Python function %s didn't return enough values"), *function->GetFName().ToString());
                }
                else
                {
                    UProperty *prop = *It;
                    PyObject *py_obj = PyTuple_GetItem(ret, tuple_index);
                    uint8 *out_frame = frame;
                    for (FOutParmRec *rec = Stack.OutParms; rec != nullptr; rec = rec->NextOutParm)
                    {
                        if (rec->Property == prop)
                        {
                            out_frame = rec->PropAddr - prop->GetOffset_ForUFunction();
                            break;
                        }
                    }
                    if (ue_py_convert_pyobject(py_obj, prop, out_frame, 0))
                    {   
                        if (prop->PropertyFlags & CPF_ReturnParm)
                        {
                            // copy value to stack result value
                            //FMemory::Memcpy(RESULT_PARAM, frame + function->ReturnValueOffset, prop->ArrayDim * prop->ElementSize);
                        }
                    }
                    else {
                        UE_LOG(LogPython, Error, TEXT("Invalid return value type for function %s"), *function->GetFName().ToString());
                    }
                    tuple_index++;
                }
            }
        }

    }
    else
    {   // no output params, but maybe a return value
        if (first_out_prop)
        {
			uint8 *out_frame = frame;
            for (FOutParmRec *rec = Stack.OutParms; rec != nullptr; rec = rec->NextOutParm)
            {
                if (rec->Property == first_out_prop)
                {
                    out_frame = rec->PropAddr - first_out_prop->GetOffset_ForUFunction();
                    break;
                }
            }
            if (ue_py_convert_pyobject(ret, first_out_prop, out_frame, 0))
            {
                if (function->ReturnValueOffset != MAX_uint16)
                {
                    // copy value to stack result value
                    //FMemory::Memcpy(RESULT_PARAM, frame + function->ReturnValueOffset, first_out_prop->ArrayDim * first_out_prop->ElementSize);
                }
            }
            else {
                UE_LOG(LogPython, Error, TEXT("Invalid return value type for function %s"), *function->GetFName().ToString());
            }
        }
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
