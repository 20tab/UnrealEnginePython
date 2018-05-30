#include "UEPyFSocket.h"


static PyObject *py_ue_fsocket_start_receiver(ue_PyFSocket *self, PyObject * args)
{

	if (self->udp_receiver)
	{
		return PyErr_Format(PyExc_Exception, "receiver already started");
	}

	self->udp_receiver = new FUdpSocketReceiver(self->sock, FTimespan::FromMilliseconds(100), *FString::Printf(TEXT("%s Thread"), *self->sock->GetDescription()));
	self->udp_receiver->OnDataReceived().BindRaw(self, &ue_PyFSocket::udp_recv);
	self->udp_receiver->Start();

	Py_INCREF(Py_None);
	return Py_None;
}

static void sock_close(ue_PyFSocket *self)
{
	if (self->sock)
	{
		self->sock->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(self->sock);
		self->sock = nullptr;
	}
}

static void sock_stop_receiver(ue_PyFSocket *self)
{
	if (self->udp_receiver)
	{
		self->udp_receiver->Stop();
		delete(self->udp_receiver);
		self->udp_receiver = nullptr;
	}
}

static PyObject *py_ue_fsocket_stop_receiver(ue_PyFSocket *self, PyObject * args)
{
	if (!self->udp_receiver)
	{
		return PyErr_Format(PyExc_Exception, "receiver not started");
	}

	sock_stop_receiver(self);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_fsocket_close(ue_PyFSocket *self, PyObject * args)
{

	if (self->udp_receiver)
	{
		return PyErr_Format(PyExc_Exception, "you have to stop its receiver before closing a socket");
	}

	sock_close(self);

	Py_INCREF(Py_None);
	return Py_None;
}



static PyMethodDef ue_PyFSocket_methods[] = {

	{ "start_receiver", (PyCFunction)py_ue_fsocket_start_receiver, METH_VARARGS, "" },
	{ "stop_receiver", (PyCFunction)py_ue_fsocket_stop_receiver, METH_VARARGS, "" },
	{ "close", (PyCFunction)py_ue_fsocket_close, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyFSocket_str(ue_PyFSocket *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FSocket '%s'>",
		TCHAR_TO_UTF8(*self->sock->GetDescription()));
}

static void ue_py_fsocket_dealloc(ue_PyFSocket *self)
{

	sock_stop_receiver(self);
	sock_close(self);

}

static PyTypeObject ue_PyFSocketType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FSocket", /* tp_name */
	sizeof(ue_PyFSocket), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_py_fsocket_dealloc,       /* tp_dealloc */
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
	(reprfunc)ue_PyFSocket_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FSocket",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFSocket_methods,             /* tp_methods */
	0,
	0,
};

static int ue_py_fsocket_init(ue_PyFSocket *self, PyObject *args, PyObject *kwargs)
{
	char *socket_desc;
	char *socket_addr;
	int port_number;
	int buffer_size = 1024;
	if (!PyArg_ParseTuple(args, "ssi|i", &socket_desc, &socket_addr, &port_number, &buffer_size))
		return -1;

	FIPv4Address addr;
	FIPv4Address::Parse(socket_addr, addr);
	FIPv4Endpoint endpoint(addr, port_number);

	self->sock = FUdpSocketBuilder(UTF8_TO_TCHAR(socket_desc)).AsNonBlocking().AsReusable().BoundToEndpoint(endpoint).WithReceiveBufferSize(buffer_size);

	return 0;
}

void ue_python_init_fsocket(PyObject *ue_module)
{
	ue_PyFSocketType.tp_new = PyType_GenericNew;

	ue_PyFSocketType.tp_init = (initproc)ue_py_fsocket_init;

	if (PyType_Ready(&ue_PyFSocketType) < 0)
		return;

	Py_INCREF(&ue_PyFSocketType);
	PyModule_AddObject(ue_module, "FSocket", (PyObject *)&ue_PyFSocketType);
}