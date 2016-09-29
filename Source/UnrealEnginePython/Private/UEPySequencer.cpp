#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/MovieScene/Public/MovieScene.h"
#include "Runtime/MovieScene/Public/MovieScenePossessable.h"
#include "Runtime/MovieScene/Public/MovieSceneBinding.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"

PyObject *py_ue_sequencer_possessable_tracks(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *guid;
	if (!PyArg_ParseTuple(args, "s:sequencer_possessable_tracks", &guid)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid)) {
		return PyErr_Format(PyExc_Exception, "invalid GUID");
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_tracks = PyList_New(0);

	FMovieScenePossessable *possessable = scene->FindPossessable(f_guid);
	if (!possessable)
		return PyErr_Format(PyExc_Exception, "GUID not found");

	TArray<FMovieSceneBinding> bindings = scene->GetBindings();
	for (FMovieSceneBinding binding : bindings) {
		if (binding.GetObjectGuid() != f_guid)
			continue;
		for (UMovieSceneTrack *track : binding.GetTracks()) {
			ue_PyUObject *ret = ue_get_python_wrapper((UObject *)track);
			if (!ret) {
				Py_DECREF(py_tracks);
				return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
			}
			PyList_Append(py_tracks, (PyObject *)ret);
		}
		break;
	}

	return py_tracks;
}

PyObject *py_ue_sequencer_find_possessable(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *guid;
	if (!PyArg_ParseTuple(args, "s:sequencer_find_possessable", &guid)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid)) {
		return PyErr_Format(PyExc_Exception, "invalid GUID");
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	UObject *u_obj = seq->FindPossessableObject(f_guid, seq);
	if (!u_obj)
		return PyErr_Format(PyExc_Exception, "unable to find uobject with GUID \"%s\"", guid);
	
	PyObject *ret = (PyObject *) ue_get_python_wrapper(u_obj);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");

	Py_INCREF(ret);
	return ret;
}

PyObject *py_ue_sequencer_master_tracks(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_tracks = PyList_New(0);

	TArray<UMovieSceneTrack *> tracks = scene->GetMasterTracks();

	for (UMovieSceneTrack *track : tracks) {
		ue_PyUObject *ret = ue_get_python_wrapper((UObject *)track);
		if (!ret) {
			Py_DECREF(py_tracks);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_tracks, (PyObject *)ret);
	}

	return py_tracks;
}

PyObject *py_ue_sequencer_possessables(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_possessables = PyList_New(0);
	for (int32 i = 0; i < scene->GetPossessableCount();i++) {
		FMovieScenePossessable possessable = scene->GetPossessable(i);
		PyObject *py_possessable = PyTuple_New(3);
		PyTuple_SetItem(py_possessable, 0, PyUnicode_FromString(TCHAR_TO_UTF8(*possessable.GetGuid().ToString())));
		PyTuple_SetItem(py_possessable, 1, PyUnicode_FromString(TCHAR_TO_UTF8(*possessable.GetName())));
		UClass *u_class = (UClass *)possessable.GetPossessedObjectClass();
		ue_PyUObject *ret = ue_get_python_wrapper(u_class);
		if (!ret) {
			// here we will leak memory :(
			Py_DECREF(py_possessables);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyTuple_SetItem(py_possessable, 2, (PyObject *)ret);
		PyList_Append(py_possessables, py_possessable);
	}

	return py_possessables;
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_folders(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_folders = PyList_New(0);

	TArray<UMovieSceneFolder *> folders = scene->GetRootFolders();

	for (UMovieSceneFolder *folder : folders) {
		ue_PyUObject *ret = ue_get_python_wrapper((UObject *)folder);
		if (!ret) {
			Py_DECREF(py_folders);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_folders, (PyObject *)ret);
	}

	return py_folders;
}
#endif

PyObject *py_ue_sequencer_sections(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_sections = PyList_New(0);

	TArray<UMovieSceneSection *> sections = scene->GetAllSections();

	for (UMovieSceneSection *section : sections) {
		ue_PyUObject *ret = ue_get_python_wrapper((UObject *)section);
		if (!ret) {
			Py_DECREF(py_sections);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_sections, (PyObject *)ret);
	}

	return py_sections;
}

