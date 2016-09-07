#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/MovieScene/Public/MovieScene.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"

PyObject *py_ue_sequencer_tracks(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UMovieScene *scene = nullptr;

	if (self->ue_object->IsA<ULevelSequence>()) {
		ULevelSequence *seq = (ULevelSequence *)self->ue_object;
		scene = seq->GetMovieScene();
	}
	else if (self->ue_object->IsA<UMovieScene>()) {
		scene = (UMovieScene *)self->ue_object;
	}

	if (!scene)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence or a MovieScene");

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

#if WITH_EDITOR
PyObject *py_ue_sequencer_folders(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UMovieScene *scene = nullptr;

	if (self->ue_object->IsA<ULevelSequence>()) {
		ULevelSequence *seq = (ULevelSequence *)self->ue_object;
		scene = seq->GetMovieScene();
	}
	else if (self->ue_object->IsA<UMovieScene>()) {
		scene = (UMovieScene *)self->ue_object;
	}

	if (!scene)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence or a MovieScene");

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

	UMovieScene *scene = nullptr;

	if (self->ue_object->IsA<ULevelSequence>()) {
		ULevelSequence *seq = (ULevelSequence *)self->ue_object;
		scene = seq->GetMovieScene();
	}
	else if (self->ue_object->IsA<UMovieScene>()) {
		scene = (UMovieScene *)self->ue_object;
	}

	if (!scene)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence or a MovieScene");

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

