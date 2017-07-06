#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/MovieScene/Public/MovieScene.h"
#include "Runtime/MovieScene/Public/MovieScenePossessable.h"
#include "Runtime/MovieScene/Public/MovieSceneBinding.h"
#include "Runtime/MovieScene/Public/MovieSceneTrack.h"
#include "Runtime/MovieScene/Public/MovieSceneNameableTrack.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"

#if WITH_EDITOR
#include "Editor/Sequencer/Public/ISequencer.h"
#include "Editor/Sequencer/Public/ISequencerModule.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"
#include "Private/LevelSequenceEditorToolkit.h"
#include "Tracks/MovieSceneCameraCutTrack.h"
#include "Sections/IKeyframeSection.h"
#include "Sections/MovieSceneFloatSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneVectorSection.h"
#include "Runtime/MovieScene/Public/MovieSceneFolder.h"
#include "Runtime/MovieScene/Public/MovieSceneSpawnable.h"
#endif

#if WITH_EDITOR
PyObject *py_ue_sequencer_changed(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "|O:sequencer_changed", &py_bool)) {
		return NULL;
	}

	bool force_open = false;

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	if (py_bool && PyObject_IsTrue(py_bool)) {
		// try to open the editor for the asset
		FAssetEditorManager::Get().OpenEditorForAsset(seq);
	}

	IAssetEditorInstance *editor = FAssetEditorManager::Get().FindEditorForAsset(seq, true);
	if (editor) {
		FLevelSequenceEditorToolkit *toolkit = (FLevelSequenceEditorToolkit *)editor;
		ISequencer *sequencer = toolkit->GetSequencer().Get();
#if ENGINE_MINOR_VERSION < 13
		sequencer->NotifyMovieSceneDataChanged();
#else
		sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
#endif

	}

	Py_INCREF(Py_None);
	return Py_None;
}
#endif

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

	PyObject *ret = (PyObject *)ue_get_python_wrapper(u_obj);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");

	Py_INCREF(ret);
	return ret;
}

PyObject *py_ue_sequencer_find_spawnable(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *guid;
	if (!PyArg_ParseTuple(args, "s:sequencer_find_spawnable", &guid)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid)) {
		return PyErr_Format(PyExc_Exception, "invalid GUID");
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	FMovieSceneSpawnable *spawnable = seq->MovieScene->FindSpawnable(f_guid);
	PyObject *ret = py_ue_new_uscriptstruct(spawnable->StaticStruct(), (uint8 *)spawnable);
	Py_INCREF(ret);
	return ret;
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_add_possessable(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_possessable", &py_obj)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ue_PyUObject *py_ue_obj = ue_is_pyuobject(py_obj);
	if (!py_ue_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a uobject");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	FString name = py_ue_obj->ue_object->GetName();
	AActor *actor = Cast<AActor>(py_ue_obj->ue_object);
	if (actor)
		name = actor->GetActorLabel();

	FGuid new_guid = seq->MovieScene->AddPossessable(name, py_ue_obj->ue_object->GetClass());
	if (!new_guid.IsValid()) {
		return PyErr_Format(PyExc_Exception, "unable to possess object");
	}

	seq->BindPossessableObject(new_guid, *(py_ue_obj->ue_object), py_ue_obj->ue_object->GetWorld());

	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_guid.ToString()));
}

PyObject *py_ue_sequencer_add_actor(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_actor", &py_obj)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ue_PyUObject *py_ue_obj = ue_is_pyuobject(py_obj);
	if (!py_ue_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a uobject");

	if (!py_ue_obj->ue_object->IsA<AActor>())
		return PyErr_Format(PyExc_Exception, "argument is not an actor");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	TArray<TWeakObjectPtr<AActor>> actors;
	actors.Add((AActor *)py_ue_obj->ue_object);

	// try to open the editor for the asset
	FAssetEditorManager::Get().OpenEditorForAsset(seq);

	IAssetEditorInstance *editor = FAssetEditorManager::Get().FindEditorForAsset(seq, true);
	if (editor) {
		FLevelSequenceEditorToolkit *toolkit = (FLevelSequenceEditorToolkit *)editor;
		ISequencer *sequencer = toolkit->GetSequencer().Get();
		sequencer->AddActors(actors);
	}
	else {
		return PyErr_Format(PyExc_Exception, "unable to access sequencer");
	}

	UObject& u_obj = *actors[0];

#if ENGINE_MINOR_VERSION < 15
	FGuid new_guid = seq->FindPossessableObjectId(u_obj);
#else
	FGuid new_guid = seq->FindPossessableObjectId(u_obj, u_obj.GetWorld());
#endif
	if (!new_guid.IsValid()) {
		return PyErr_Format(PyExc_Exception, "unable to find guid");
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_guid.ToString()));
}

PyObject *py_ue_sequencer_make_new_spawnable(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_spawnable", &py_obj)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ue_PyUObject *py_ue_obj = ue_is_pyuobject(py_obj);
	if (!py_ue_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a uobject");

	if (!py_ue_obj->ue_object->IsA<AActor>())
		return PyErr_Format(PyExc_Exception, "argument is not an actor");

	AActor *actor = (AActor *)py_ue_obj->ue_object;

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	// try to open the editor for the asset
	FAssetEditorManager::Get().OpenEditorForAsset(seq);

	IAssetEditorInstance *editor = FAssetEditorManager::Get().FindEditorForAsset(seq, true);
	if (!editor) {
		return PyErr_Format(PyExc_Exception, "unable to access sequencer");
	}

	FLevelSequenceEditorToolkit *toolkit = (FLevelSequenceEditorToolkit *)editor;
	ISequencer *sequencer = toolkit->GetSequencer().Get();
	FGuid new_guid = sequencer->MakeNewSpawnable(*actor);

	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_guid.ToString()));
}
#endif

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
	for (int32 i = 0; i < scene->GetPossessableCount(); i++) {
		FMovieScenePossessable possessable = scene->GetPossessable(i);
		PyObject *py_possessable = py_ue_new_uscriptstruct(possessable.StaticStruct(), (uint8 *)&possessable);
		PyList_Append(py_possessables, py_possessable);
	}

	return py_possessables;
}

PyObject *py_ue_sequencer_possessables_guid(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_possessables = PyList_New(0);
	for (int32 i = 0; i < scene->GetPossessableCount(); i++) {
		FMovieScenePossessable possessable = scene->GetPossessable(i);
		PyObject *py_possessable = PyUnicode_FromString(TCHAR_TO_UTF8(*possessable.GetGuid().ToString()));
		PyList_Append(py_possessables, py_possessable);
	}

	return py_possessables;
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_folders(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	PyObject *py_obj = nullptr;
	if (!PyArg_ParseTuple(args, "|O:sequencer_folders", &py_obj)) {
		return NULL;
	}

	UMovieSceneFolder *parent_folder = nullptr;

	if (py_obj) {
		ue_PyUObject *ue_py_obj = ue_is_pyuobject(py_obj);
		if (!ue_py_obj) {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		parent_folder = Cast<UMovieSceneFolder>(ue_py_obj->ue_object);
		if (!parent_folder) {
			return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneFolder");
		}
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_folders = PyList_New(0);

	TArray<UMovieSceneFolder *> folders;
	if (!parent_folder)
		folders = scene->GetRootFolders();
	else {
		folders = parent_folder->GetChildFolders();
	}

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

PyObject *py_ue_sequencer_create_folder(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	PyObject *py_obj = nullptr;
	char *name;
	if (!PyArg_ParseTuple(args, "s|O:sequencer_create_folder", &name, &py_obj)) {
		return NULL;
	}

	UMovieSceneFolder *parent_folder = nullptr;

	if (py_obj) {
		ue_PyUObject *ue_py_obj = ue_is_pyuobject(py_obj);
		if (!ue_py_obj) {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		parent_folder = Cast<UMovieSceneFolder>(ue_py_obj->ue_object);
		if (!parent_folder) {
			return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneFolder");
		}
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	UMovieSceneFolder *new_folder = NewObject<UMovieSceneFolder>(scene, NAME_None, RF_Transactional);
	new_folder->SetFolderName(FName(name));

	if (parent_folder) {
		parent_folder->Modify();
		parent_folder->AddChildFolder(new_folder);
	}
	else {
		scene->Modify();
		scene->GetRootFolders().Add(new_folder);
	}

	PyObject *ret = (PyObject *)ue_get_python_wrapper(new_folder);
	if (!ret) {
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	}
	return ret;
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

PyObject *py_ue_sequencer_track_sections(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UMovieSceneTrack>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneTrack");

	UMovieSceneTrack *track = (UMovieSceneTrack *)self->ue_object;

	PyObject *py_sections = PyList_New(0);

	TArray<UMovieSceneSection *> sections = track->GetAllSections();

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

#if WITH_EDITOR
PyObject *py_ue_sequencer_track_add_section(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UMovieSceneTrack>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneTrack");

	UMovieSceneTrack *track = (UMovieSceneTrack *)self->ue_object;

	UMovieSceneSection *new_section = track->CreateNewSection();

	track->AddSection(*new_section);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(new_section);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");

	Py_INCREF(ret);
	return ret;
}
#endif

PyObject *py_ue_sequencer_add_master_track(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_master_track", &obj)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	ue_PyUObject *py_obj = ue_is_pyuobject(obj);
	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a uobject");

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");

	UClass *u_class = (UClass *)py_obj->ue_object;

	if (!u_class->IsChildOf<UMovieSceneTrack>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneTrack class");

	UMovieSceneTrack *track = scene->AddMasterTrack(u_class);
	if (!track)
		return PyErr_Format(PyExc_Exception, "unable to create new master track");

	PyObject *ret = (PyObject *)ue_get_python_wrapper(track);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");

	Py_INCREF(ret);
	return ret;
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_section_add_key(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float time;
	PyObject *py_value;
	int interpolation = 0;
	if (!PyArg_ParseTuple(args, "fO|i:sequencer_section_add_key", &time, &py_value, &interpolation)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UMovieSceneSection>())
		return PyErr_Format(PyExc_Exception, "uobject is not a MovieSceneSection");

	UMovieSceneSection *section = (UMovieSceneSection *)self->ue_object;

	if (auto section_float = Cast<UMovieSceneFloatSection>(section)) {
		if (PyNumber_Check(py_value)) {
			PyObject *f_value = PyNumber_Float(py_value);
			float value = PyFloat_AsDouble(f_value);
			Py_DECREF(f_value);
			section_float->AddKey(time, value, (EMovieSceneKeyInterpolation)interpolation);
			Py_INCREF(Py_None);
			return Py_None;
		}
	}

	else if (auto section_bool = Cast<UMovieSceneBoolSection>(section)) {
		if (PyBool_Check(py_value)) {
			bool value = false;
			if (PyObject_IsTrue(py_value))
				value = true;
			section_bool->AddKey(time, value, (EMovieSceneKeyInterpolation)interpolation);
			Py_INCREF(Py_None);
			return Py_None;
		}
	}

	else if (auto section_transform = Cast<UMovieScene3DTransformSection>(section)) {
		if (ue_PyFTransform *py_transform = py_ue_is_ftransform(py_value)) {
			bool unwind = false;
			FTransform transform = py_transform->transform;


			FTransformKey tx = FTransformKey(EKey3DTransformChannel::Translation, EAxis::X, transform.GetLocation().X, unwind);
			FTransformKey ty = FTransformKey(EKey3DTransformChannel::Translation, EAxis::Y, transform.GetLocation().Y, unwind);
			FTransformKey tz = FTransformKey(EKey3DTransformChannel::Translation, EAxis::Z, transform.GetLocation().Z, unwind);
			section_transform->AddKey(time, tx, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, ty, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, tz, (EMovieSceneKeyInterpolation)interpolation);

			FTransformKey rx = FTransformKey(EKey3DTransformChannel::Rotation, EAxis::X, transform.GetRotation().X, unwind);
			FTransformKey ry = FTransformKey(EKey3DTransformChannel::Rotation, EAxis::Y, transform.GetRotation().Y, unwind);
			FTransformKey rz = FTransformKey(EKey3DTransformChannel::Rotation, EAxis::Z, transform.GetRotation().Z, unwind);
			section_transform->AddKey(time, rx, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, ry, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, rz, (EMovieSceneKeyInterpolation)interpolation);

			FTransformKey sx = FTransformKey(EKey3DTransformChannel::Scale, EAxis::X, transform.GetScale3D().X, unwind);
			FTransformKey sy = FTransformKey(EKey3DTransformChannel::Scale, EAxis::Y, transform.GetScale3D().Y, unwind);
			FTransformKey sz = FTransformKey(EKey3DTransformChannel::Scale, EAxis::Z, transform.GetScale3D().Z, unwind);
			section_transform->AddKey(time, sx, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, sy, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, sz, (EMovieSceneKeyInterpolation)interpolation);

			Py_INCREF(Py_None);
			return Py_None;
		}
	}

	else if (auto section_vector = Cast<UMovieSceneVectorSection>(section)) {
		if (ue_PyFVector *py_vector = py_ue_is_fvector(py_value)) {
			FVector vec = py_vector->vec;
			FVectorKey vx = FVectorKey(EKeyVectorChannel::X, vec.X);
			FVectorKey vy = FVectorKey(EKeyVectorChannel::Y, vec.Y);
			FVectorKey vz = FVectorKey(EKeyVectorChannel::Z, vec.Z);

			section_vector->AddKey(time, vx, (EMovieSceneKeyInterpolation)interpolation);
			section_vector->AddKey(time, vy, (EMovieSceneKeyInterpolation)interpolation);
			section_vector->AddKey(time, vz, (EMovieSceneKeyInterpolation)interpolation);

			Py_INCREF(Py_None);
			return Py_None;
		}
	}

	return PyErr_Format(PyExc_Exception, "unsupported section type: %s", TCHAR_TO_UTF8(*section->GetClass()->GetName()));
}

PyObject *py_ue_sequencer_add_camera_cut_track(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	UMovieSceneTrack *track = scene->AddCameraCutTrack(UMovieSceneCameraCutTrack::StaticClass());

	PyObject *ret = (PyObject *)ue_get_python_wrapper(track);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");

	Py_INCREF(ret);
	return ret;
}
#endif

PyObject *py_ue_sequencer_add_track(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	char *guid;
	if (!PyArg_ParseTuple(args, "Os:sequencer_add_track", &obj, &guid)) {
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	ue_PyUObject *py_obj = ue_is_pyuobject(obj);
	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a uobject");

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");

	UClass *u_class = (UClass *)py_obj->ue_object;

	if (!u_class->IsChildOf<UMovieSceneTrack>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneTrack class");

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid)) {
		return PyErr_Format(PyExc_Exception, "invalid guid");
	}
	UMovieSceneTrack *track = scene->AddTrack(u_class, f_guid);
	if (!track)
		return PyErr_Format(PyExc_Exception, "unable to create new track");

	PyObject *ret = (PyObject *)ue_get_python_wrapper(track);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");

	Py_INCREF(ret);
	return ret;
}

#if WITH_EDITOR
// smart functions allowing the set/get of display names to various sequencer objects
PyObject *py_ue_sequencer_set_display_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:sequencer_set_display_name", &name)) {
		return NULL;
	}

	if (self->ue_object->IsA<UMovieSceneNameableTrack>()) {
		UMovieSceneNameableTrack *track = (UMovieSceneNameableTrack *)self->ue_object;
		track->SetDisplayName(FText::FromString(UTF8_TO_TCHAR(name)));
	}
	else {
		return PyErr_Format(PyExc_Exception, "the uobject does not expose the SetDisplayName() method");
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_sequencer_get_display_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (self->ue_object->IsA<UMovieSceneNameableTrack>()) {
		UMovieSceneNameableTrack *track = (UMovieSceneNameableTrack *)self->ue_object;
		FText name = track->GetDefaultDisplayName();
		return PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString()));
	}

	return PyErr_Format(PyExc_Exception, "the uobject does not expose the GetDefaultDisplayName() method");
}
#endif

