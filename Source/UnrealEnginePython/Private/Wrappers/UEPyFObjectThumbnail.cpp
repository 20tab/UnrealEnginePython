#include "UEPyFObjectThumbnail.h"


static PyObject *py_ue_fobject_thumbnail_get_image_width(ue_PyFObjectThumbnail *self, PyObject * args)
{
	return PyLong_FromLong(self->object_thumbnail.GetImageWidth());
}

static PyObject *py_ue_fobject_thumbnail_get_image_height(ue_PyFObjectThumbnail *self, PyObject * args)
{
	return PyLong_FromLong(self->object_thumbnail.GetImageHeight());
}

static PyObject *py_ue_fobject_thumbnail_get_compressed_data_size(ue_PyFObjectThumbnail *self, PyObject * args)
{
	return PyLong_FromLong(self->object_thumbnail.GetCompressedDataSize());
}

static PyObject *py_ue_fobject_thumbnail_get_uncompressed_image_data(ue_PyFObjectThumbnail *self, PyObject * args)
{
	const TArray<uint8> &data = self->object_thumbnail.GetUncompressedImageData();
	return PyByteArray_FromStringAndSize((char *)data.GetData(), data.Num());
}

static PyObject *py_ue_fobject_thumbnail_compress_image_data(ue_PyFObjectThumbnail *self, PyObject * args)
{
	self->object_thumbnail.CompressImageData();
	Py_RETURN_NONE;
}


static PyMethodDef ue_PyFObjectThumbnail_methods[] = {
	{ "get_image_width", (PyCFunction)py_ue_fobject_thumbnail_get_image_width, METH_VARARGS, "" },
	{ "get_image_height", (PyCFunction)py_ue_fobject_thumbnail_get_image_height, METH_VARARGS, "" },
	{ "get_compressed_data_size", (PyCFunction)py_ue_fobject_thumbnail_get_compressed_data_size, METH_VARARGS, "" },
	{ "get_uncompressed_image_data", (PyCFunction)py_ue_fobject_thumbnail_get_uncompressed_image_data, METH_VARARGS, "" },
	{ "compress_image_data", (PyCFunction)py_ue_fobject_thumbnail_get_uncompressed_image_data, METH_VARARGS, "" },
	{ nullptr }  /* Sentinel */
};

static PyObject *ue_PyFObjectThumbnail_str(ue_PyFObjectThumbnail *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FObjectThumbnail {'width': %d, 'height': %d, 'compressed_data_size': %d}>",
		self->object_thumbnail.GetImageWidth(),
		self->object_thumbnail.GetImageHeight(),
		self->object_thumbnail.GetCompressedDataSize());
}

static PyTypeObject ue_PyFObjectThumbnailType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FObjectThumbnail", /* tp_name */
	sizeof(ue_PyFObjectThumbnail), /* tp_basicsize */
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
	(reprfunc)ue_PyFObjectThumbnail_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FObjectThumbnail",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFObjectThumbnail_methods,             /* tp_methods */
	0,
};

static int py_ue_fobject_thumbnail_init(ue_PyFObjectThumbnail *self, PyObject * args)
{
	new(&self->object_thumbnail) FObjectThumbnail();
	return 0;
}

void ue_python_init_fobject_thumbnail(PyObject *ue_module)
{
	ue_PyFObjectThumbnailType.tp_new = PyType_GenericNew;
	ue_PyFObjectThumbnailType.tp_init = (initproc)py_ue_fobject_thumbnail_init;

	if (PyType_Ready(&ue_PyFObjectThumbnailType) < 0)
		return;

	Py_INCREF(&ue_PyFObjectThumbnailType);
	PyModule_AddObject(ue_module, "FObjectThumbnail", (PyObject *)&ue_PyFObjectThumbnailType);
}

ue_PyFObjectThumbnail *py_ue_is_fobject_thumbnail(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFObjectThumbnailType))
		return nullptr;
	return (ue_PyFObjectThumbnail *)obj;
}

PyObject *py_ue_new_fobject_thumbnail(FObjectThumbnail object_thumnail)
{
	ue_PyFObjectThumbnail *ret = (ue_PyFObjectThumbnail *)PyObject_New(ue_PyFObjectThumbnail, &ue_PyFObjectThumbnailType);
	new(&ret->object_thumbnail) FObjectThumbnail(object_thumnail);
	return (PyObject *)ret;
}