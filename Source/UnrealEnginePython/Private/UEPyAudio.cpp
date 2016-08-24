#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_play_sound_at_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *sound;
	float x, y, z;
	float volume = 1;
	float pitch = 1;
	float start = 0;

	if (!PyArg_ParseTuple(args, "Offf|fff:play_sound_at_location", &sound, &x, &y, &z, &volume, &pitch, &start)) {
		return NULL;
	}


	USoundBase *sound_object = nullptr;
	if (ue_is_pyuobject(sound)) {
		ue_PyUObject *py_sound = (ue_PyUObject *)sound;
		if (py_sound->ue_object->IsA<USoundBase>()) {
			sound_object = (USoundBase *)py_sound->ue_object;
		}
	}
	else if (PyUnicode_Check(sound)) {
		sound_object = FindObject<USoundBase>(ANY_PACKAGE, UTF8_TO_TCHAR(PyUnicode_AsUTF8(sound)));
	}

	if (!sound_object)
		return PyErr_Format(PyExc_Exception, "invalid sound object");

	UGameplayStatics::PlaySoundAtLocation(self->ue_object, sound_object, FVector(x, y, z), volume, pitch, start);

	Py_INCREF(Py_None);
	return Py_None;
}