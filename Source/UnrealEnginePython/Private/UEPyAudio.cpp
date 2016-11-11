#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_play_sound_at_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *sound;
	PyObject *py_location;
	float volume = 1;
	float pitch = 1;
	float start = 0;

	if (!PyArg_ParseTuple(args, "OO|fff:play_sound_at_location", &sound, &py_location, &volume, &pitch, &start)) {
		return NULL;
	}


	USoundBase *sound_object = nullptr;
	if (ue_is_pyuobject(sound)) {
		ue_PyUObject *py_sound = (ue_PyUObject *)sound;
		if (py_sound->ue_object->IsA<USoundBase>()) {
			sound_object = (USoundBase *)py_sound->ue_object;
		}
	}
	else if (PyUnicodeOrString_Check(sound)) {
		sound_object = FindObject<USoundBase>(ANY_PACKAGE, UTF8_TO_TCHAR(PyUnicode_AsUTF8(sound)));
	}

	if (!sound_object)
		return PyErr_Format(PyExc_Exception, "invalid sound object");

	ue_PyFVector *location = py_ue_is_fvector(py_location);
	if (!location)
		return PyErr_Format(PyExc_TypeError, "sound location must be a FVector");

	UGameplayStatics::PlaySoundAtLocation(self->ue_object, sound_object, location->vec, volume, pitch, start);

	Py_INCREF(Py_None);
	return Py_None;
}