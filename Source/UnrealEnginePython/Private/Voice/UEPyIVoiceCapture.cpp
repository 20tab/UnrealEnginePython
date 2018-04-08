
#include "UEPyIVoiceCapture.h"


PyObject *py_ue_ivoice_capture_start(ue_PyIVoiceCapture *self, PyObject * args)
{

	self->voice_capture->Start();

	Py_RETURN_NONE;
}

PyObject *py_ue_ivoice_capture_stop(ue_PyIVoiceCapture *self, PyObject * args)
{

	self->voice_capture->Stop();

	Py_RETURN_NONE;
}

PyObject *py_ue_ivoice_capture_shutdown(ue_PyIVoiceCapture *self, PyObject * args)
{

	self->voice_capture->Shutdown();

	Py_RETURN_NONE;
}

PyObject *py_ue_ivoice_capture_init_method(ue_PyIVoiceCapture *self, PyObject * args)
{

	int sample_rate;
	int channels;
	char *name = (char *)"";
	if (!PyArg_ParseTuple(args, "ii|s", &sample_rate, &channels, &name))
	{
		return nullptr;
	}

#if ENGINE_MINOR_VERSION < 18
	if (self->voice_capture->Init(sample_rate, channels))
#else
	if (self->voice_capture->Init(UTF8_TO_TCHAR(name), sample_rate, channels))
#endif
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_ivoice_capture_is_capturing(ue_PyIVoiceCapture *self, PyObject * args)
{

	if (self->voice_capture->IsCapturing())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_ivoice_capture_get_capture_state(ue_PyIVoiceCapture *self, PyObject * args)
{
	uint32 available_data;
	EVoiceCaptureState::Type state = self->voice_capture->GetCaptureState(available_data);

	return Py_BuildValue("ii", (int)state, available_data);
}

PyObject *py_ue_ivoice_capture_get_voice_data(ue_PyIVoiceCapture *self, PyObject * args)
{
	int len;
	if (!PyArg_ParseTuple(args, "i", &len))
	{
		return nullptr;
	}

	uint32 available_data = 0;
	uint8 *data = (uint8*)FMemory_Alloca(len);

	EVoiceCaptureState::Type state = self->voice_capture->GetVoiceData(data, len, available_data);
	return Py_BuildValue("iO", (int)state, PyByteArray_FromStringAndSize((char *)data, (Py_ssize_t)FMath::Min(available_data, (uint32)len)));
}

static PyMethodDef ue_PyIVoiceCapture_methods[] = {
	{ "start", (PyCFunction)py_ue_ivoice_capture_start, METH_VARARGS, "" },
	{ "stop", (PyCFunction)py_ue_ivoice_capture_stop, METH_VARARGS, "" },
	{ "shutdown", (PyCFunction)py_ue_ivoice_capture_shutdown, METH_VARARGS, "" },
	{ "init", (PyCFunction)py_ue_ivoice_capture_init_method, METH_VARARGS, "" },
	{ "is_capturing", (PyCFunction)py_ue_ivoice_capture_is_capturing, METH_VARARGS, "" },
	{ "get_capture_state", (PyCFunction)py_ue_ivoice_capture_get_capture_state, METH_VARARGS, "" },
	{ "get_voice_data", (PyCFunction)py_ue_ivoice_capture_get_voice_data, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyIVoiceCaptureType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IVoiceCapture", /* tp_name */
	sizeof(ue_PyIVoiceCapture), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine VoiceCapture Interface",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIVoiceCapture_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_ivoice_capture_init(ue_PyIVoiceCapture *self, PyObject * args)
{

	TSharedPtr<IVoiceCapture> voice_capture_ptr = FVoiceModule::Get().CreateVoiceCapture();
	if (!voice_capture_ptr.IsValid())
	{
		PyErr_SetString(PyExc_Exception, "unable to create a new VoiceCapture");
		return -1;
	}

	new(&self->voice_capture) TSharedRef<IVoiceCapture>(voice_capture_ptr.ToSharedRef());
	return 0;
}

void ue_python_init_ivoice_capture(PyObject *ue_module)
{
	ue_PyIVoiceCaptureType.tp_new = PyType_GenericNew;
	ue_PyIVoiceCaptureType.tp_init = (initproc)py_ue_ivoice_capture_init;

	if (PyType_Ready(&ue_PyIVoiceCaptureType) < 0)
		return;

	Py_INCREF(&ue_PyIVoiceCaptureType);
	PyModule_AddObject(ue_module, "IVoiceCapture", (PyObject *)&ue_PyIVoiceCaptureType);
}
