#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_texture_get_data(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int mipmap = 0;

	if (!PyArg_ParseTuple(args, "|i:texture_get_data", &mipmap)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UTexture2D>())
		return PyErr_Format(PyExc_Exception, "object is not a Texture2D");

	UTexture2D *tex = (UTexture2D *)self->ue_object;

	if (mipmap >= tex->GetNumMips())
		return PyErr_Format(PyExc_Exception, "invalid mipmap id");

	const char *blob = (const char*)tex->PlatformData->Mips[mipmap].BulkData.Lock(LOCK_READ_ONLY);
	PyObject *bytes = PyByteArray_FromStringAndSize(blob, (Py_ssize_t)tex->PlatformData->Mips[mipmap].BulkData.GetBulkDataSize());
	tex->PlatformData->Mips[mipmap].BulkData.Unlock();
	return bytes;
}

PyObject *py_ue_texture_set_data(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *byte_array;
	int mipmap = 0;

	if (!PyArg_ParseTuple(args, "O|i:texture_get_data", &byte_array, &mipmap)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UTexture2D>())
		return PyErr_Format(PyExc_Exception, "object is not a Texture2D");

	UTexture2D *tex = (UTexture2D *)self->ue_object;

	if (!PyByteArray_Check(byte_array))
		return PyErr_Format(PyExc_Exception, "argument is not a bytearray");

	if (mipmap >= tex->GetNumMips())
		return PyErr_Format(PyExc_Exception, "invalid mipmap id");

	char *blob = (char*)tex->PlatformData->Mips[mipmap].BulkData.Lock(LOCK_READ_WRITE);
	int32 len = tex->PlatformData->Mips[mipmap].BulkData.GetBulkDataSize();
	Py_ssize_t byte_array_size = PyByteArray_Size(byte_array);
	// avoid making mess
	if (byte_array_size > len)
		byte_array_size = len;
	char *byte_array_blob = PyByteArray_AsString(byte_array);
	FMemory::Memcpy(blob, byte_array_blob, byte_array_size);
	tex->PlatformData->Mips[mipmap].BulkData.Unlock();

	tex->MarkPackageDirty();
#if WITH_EDITOR
	tex->PostEditChange();
#endif

	// ensure compatibility between 4.12 and 4.14
	//tex->UpdateResourceW();

	Py_INCREF(Py_None);
	return Py_None;
}


