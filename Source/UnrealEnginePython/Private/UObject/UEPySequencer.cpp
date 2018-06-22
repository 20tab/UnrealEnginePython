#include "UEPySequencer.h"

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
#include "Private/SequencerSelection.h"
#if ENGINE_MINOR_VERSION < 18
#include "Editor/UnrealEd/Private/FbxImporter.h"
#else
#include "Editor/UnrealEd/Public/FbxImporter.h"
#endif
#include "Editor/MovieSceneTools/Public/MatineeImportTools.h"
#endif
#include "MovieSceneSection.h"
#include "Set.h"

namespace {
    const FName LevelSequenceEditorName("LevelSequenceEditor");
}

#include "GameFramework/Actor.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Engine/World.h"

#if WITH_EDITOR
PyObject *py_ue_sequencer_changed(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_bool = nullptr;
#if ENGINE_MINOR_VERSION < 13
    int changeNotificationType = (int)EMovieSceneDataChangeType::Unknown;
#else
    int changeNotificationType = 0;
#endif
	if (!PyArg_ParseTuple(args, "|Oi:sequencer_changed", &py_bool, &changeNotificationType))
	{
		return NULL;
	}

	bool force_open = false;

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	if (py_bool && PyObject_IsTrue(py_bool))
	{
		// try to open the editor for the asset
		FAssetEditorManager::Get().OpenEditorForAsset(seq);
	}

	IAssetEditorInstance *editor = FAssetEditorManager::Get().FindEditorForAsset(seq, true);
	if (editor)
	{
		FLevelSequenceEditorToolkit *toolkit = (FLevelSequenceEditorToolkit *)editor;
		ISequencer *sequencer = toolkit->GetSequencer().Get();
#if ENGINE_MINOR_VERSION < 13
		sequencer->NotifyMovieSceneDataChanged();
#else
		sequencer->NotifyMovieSceneDataChanged((EMovieSceneDataChangeType)changeNotificationType);
#endif

	}

	Py_INCREF(Py_None);
	return Py_None;
}
#endif

PyObject *py_ue_sequencer_possessable_tracks(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *guid;
	if (!PyArg_ParseTuple(args, "s:sequencer_possessable_tracks", &guid))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid))
	{
		return PyErr_Format(PyExc_Exception, "invalid GUID");
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_tracks = PyList_New(0);

	FMovieScenePossessable *possessable = scene->FindPossessable(f_guid);
	if (!possessable)
		return PyErr_Format(PyExc_Exception, "GUID not found");

	TArray<FMovieSceneBinding> bindings = scene->GetBindings();
	for (FMovieSceneBinding binding : bindings)
	{
		if (binding.GetObjectGuid() != f_guid)
			continue;
		for (UMovieSceneTrack *track : binding.GetTracks())
		{
			ue_PyUObject *ret = ue_get_python_uobject((UObject *)track);
			if (!ret)
			{
				Py_DECREF(py_tracks);
				return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
			}
			PyList_Append(py_tracks, (PyObject *)ret);
		}
		break;
	}

	return py_tracks;
}

PyObject *py_ue_sequencer_find_possessable(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *guid;
	PyObject *py_parent = nullptr;
	if (!PyArg_ParseTuple(args, "s|O:sequencer_find_possessable", &guid, &py_parent))
	{
		return nullptr;
	}

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a ULevelSequence");
	}

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid))
	{
		return PyErr_Format(PyExc_Exception, "invalid GUID");
	}

#if ENGINE_MINOR_VERSION < 15
	UObject *u_obj = seq->FindPossessableObject(f_guid, seq);
#else
	UObject *parent = nullptr;
	if (py_parent)
	{
		parent = ue_py_check_type<UObject>(py_parent);
		if (!parent)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
	}
	UObject *u_obj = nullptr;
	TArray<UObject *, TInlineAllocator<1>> u_objects;
	seq->LocateBoundObjects(f_guid, parent, u_objects);
	if (u_objects.Num() > 0)
	{
		u_obj = u_objects[0];
	}
#endif
	if (!u_obj)
		return PyErr_Format(PyExc_Exception, "unable to find uobject with GUID \"%s\"", guid);

	Py_RETURN_UOBJECT(u_obj);
	}

PyObject *py_ue_sequencer_find_spawnable(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *guid;
	if (!PyArg_ParseTuple(args, "s:sequencer_find_spawnable", &guid))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid))
	{
		return PyErr_Format(PyExc_Exception, "invalid GUID");
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	FMovieSceneSpawnable *spawnable = seq->MovieScene->FindSpawnable(f_guid);
	PyObject *ret = py_ue_new_uscriptstruct(spawnable->StaticStruct(), (uint8 *)spawnable);
	Py_INCREF(ret);
	return ret;
}

PyObject *py_ue_sequencer_get_all_spawnables(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!PyArg_ParseTuple(args, ":get_all_spawnables"))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_spawnables = PyList_New(0);

	for (int i = 0; i < scene->GetSpawnableCount(); ++i)
	{
		FMovieSceneSpawnable & spawnable = scene->GetSpawnable(i);
		PyObject *ret = py_ue_new_uscriptstruct(spawnable.StaticStruct(), (uint8 *)&spawnable);
		if (!ret)
		{
			Py_DECREF(py_spawnables);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_spawnables, (PyObject *)ret);
	}

	return py_spawnables;
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_add_possessable(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_possessable", &py_obj))
	{
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
	if (!new_guid.IsValid())
	{
		return PyErr_Format(PyExc_Exception, "unable to possess object");
	}

	seq->BindPossessableObject(new_guid, *(py_ue_obj->ue_object), py_ue_obj->ue_object->GetWorld());

	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_guid.ToString()));
}

PyObject *py_ue_sequencer_add_actor(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_actor", &py_obj))
	{
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
	if (editor)
	{
		FLevelSequenceEditorToolkit *toolkit = (FLevelSequenceEditorToolkit *)editor;
		ISequencer *sequencer = toolkit->GetSequencer().Get();
		sequencer->AddActors(actors);
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "unable to access sequencer");
	}

	UObject& u_obj = *actors[0];

#if ENGINE_MINOR_VERSION < 15
	FGuid new_guid = seq->FindPossessableObjectId(u_obj);
#else
	FGuid new_guid = seq->FindPossessableObjectId(u_obj, u_obj.GetWorld());
#endif
	if (!new_guid.IsValid())
	{
		return PyErr_Format(PyExc_Exception, "unable to find guid");
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_guid.ToString()));
	}

PyObject *py_ue_sequencer_add_actor_component(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_actor_component", &py_obj))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ue_PyUObject *py_ue_obj = ue_is_pyuobject(py_obj);
	if (!py_ue_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a uobject");

	if (!py_ue_obj->ue_object->IsA<UActorComponent>())
		return PyErr_Format(PyExc_Exception, "argument is not an actorcomponent");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;

	UActorComponent* actorComponent = (UActorComponent *)py_ue_obj->ue_object;

	// try to open the editor for the asset
	FAssetEditorManager::Get().OpenEditorForAsset(seq);

	IAssetEditorInstance *editor = FAssetEditorManager::Get().FindEditorForAsset(seq, true);
	FGuid new_guid;
	if (editor)
	{
		FLevelSequenceEditorToolkit *toolkit = (FLevelSequenceEditorToolkit *)editor;
		ISequencer *sequencer = toolkit->GetSequencer().Get();
		new_guid = sequencer->GetHandleToObject(actorComponent);
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "unable to access sequencer");
	}

	if (!new_guid.IsValid())
	{
		return PyErr_Format(PyExc_Exception, "unable to find guid");
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_guid.ToString()));
}

PyObject *py_ue_sequencer_make_new_spawnable(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_make_new_spawnable", &py_obj))
	{
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
	if (!editor)
	{
		return PyErr_Format(PyExc_Exception, "unable to access sequencer");
	}

	FLevelSequenceEditorToolkit *toolkit = (FLevelSequenceEditorToolkit *)editor;
	ISequencer *sequencer = toolkit->GetSequencer().Get();
	FGuid new_guid = sequencer->MakeNewSpawnable(*actor);

	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_guid.ToString()));
}
#endif

PyObject *py_ue_sequencer_master_tracks(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_tracks = PyList_New(0);

	TArray<UMovieSceneTrack *> tracks = scene->GetMasterTracks();

	for (UMovieSceneTrack *track : tracks)
	{
		ue_PyUObject *ret = ue_get_python_uobject((UObject *)track);
		if (!ret)
		{
			Py_DECREF(py_tracks);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_tracks, (PyObject *)ret);
	}

	return py_tracks;
}

PyObject *py_ue_sequencer_get_camera_cut_track(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	UMovieSceneTrack *track = scene->GetCameraCutTrack();
	if (!track)
	{
		return PyErr_Format(PyExc_Exception, "unable to find camera cut track");
	}

	Py_RETURN_UOBJECT(track);
}

PyObject *py_ue_sequencer_possessables(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_possessables = PyList_New(0);
	for (int32 i = 0; i < scene->GetPossessableCount(); i++)
	{
		FMovieScenePossessable possessable = scene->GetPossessable(i);
		PyObject *py_possessable = py_ue_new_uscriptstruct(possessable.StaticStruct(), (uint8 *)&possessable);
		PyList_Append(py_possessables, py_possessable);
	}

	return py_possessables;
}

PyObject *py_ue_sequencer_possessables_guid(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_possessables = PyList_New(0);
	for (int32 i = 0; i < scene->GetPossessableCount(); i++)
	{
		FMovieScenePossessable possessable = scene->GetPossessable(i);
		PyObject *py_possessable = PyUnicode_FromString(TCHAR_TO_UTF8(*possessable.GetGuid().ToString()));
		PyList_Append(py_possessables, py_possessable);
	}

	return py_possessables;
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_folders(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	PyObject *py_obj = nullptr;
	if (!PyArg_ParseTuple(args, "|O:sequencer_folders", &py_obj))
	{
		return NULL;
	}

	UMovieSceneFolder *parent_folder = nullptr;

	if (py_obj)
	{
		ue_PyUObject *ue_py_obj = ue_is_pyuobject(py_obj);
		if (!ue_py_obj)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		parent_folder = Cast<UMovieSceneFolder>(ue_py_obj->ue_object);
		if (!parent_folder)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneFolder");
		}
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_folders = PyList_New(0);

	TArray<UMovieSceneFolder *> folders;
	if (!parent_folder)
		folders = scene->GetRootFolders();
	else
	{
		folders = parent_folder->GetChildFolders();
	}

	for (UMovieSceneFolder *folder : folders)
	{
		ue_PyUObject *ret = ue_get_python_uobject((UObject *)folder);
		if (!ret)
		{
			Py_DECREF(py_folders);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_folders, (PyObject *)ret);
	}

	return py_folders;
}

PyObject *py_ue_sequencer_create_folder(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	PyObject *py_obj = nullptr;
	char *name;
	if (!PyArg_ParseTuple(args, "s|O:sequencer_create_folder", &name, &py_obj))
	{
		return NULL;
	}

	UMovieSceneFolder *parent_folder = nullptr;

	if (py_obj)
	{
		ue_PyUObject *ue_py_obj = ue_is_pyuobject(py_obj);
		if (!ue_py_obj)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		parent_folder = Cast<UMovieSceneFolder>(ue_py_obj->ue_object);
		if (!parent_folder)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneFolder");
		}
	}

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	UMovieSceneFolder *new_folder = NewObject<UMovieSceneFolder>(scene, NAME_None, RF_Transactional);
	new_folder->SetFolderName(FName(name));

	if (parent_folder)
	{
		parent_folder->Modify();
		parent_folder->AddChildFolder(new_folder);
	}
	else
	{
		scene->Modify();
		scene->GetRootFolders().Add(new_folder);
	}

	Py_RETURN_UOBJECT(new_folder);
}
#endif

PyObject *py_ue_sequencer_sections(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	PyObject *py_sections = PyList_New(0);

	TArray<UMovieSceneSection *> sections = scene->GetAllSections();

	for (UMovieSceneSection *section : sections)
	{

		ue_PyUObject *ret = ue_get_python_uobject((UObject *)section);
		if (!ret)
		{
			Py_DECREF(py_sections);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_sections, (PyObject *)ret);
	}

	return py_sections;
}

PyObject *py_ue_sequencer_track_sections(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<UMovieSceneTrack>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneTrack");

	UMovieSceneTrack *track = (UMovieSceneTrack *)self->ue_object;

	PyObject *py_sections = PyList_New(0);

	TArray<UMovieSceneSection *> sections = track->GetAllSections();

	for (UMovieSceneSection *section : sections)
	{
		ue_PyUObject *ret = ue_get_python_uobject((UObject *)section);
		if (!ret)
		{
			Py_DECREF(py_sections);
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		}
		PyList_Append(py_sections, (PyObject *)ret);
	}

	return py_sections;
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_track_add_section(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<UMovieSceneTrack>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneTrack");

	UMovieSceneTrack *track = (UMovieSceneTrack *)self->ue_object;

	UMovieSceneSection *new_section = track->CreateNewSection();

	track->AddSection(*new_section);

	Py_RETURN_UOBJECT(new_section);
}
#endif

PyObject *py_ue_sequencer_add_master_track(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:sequencer_add_master_track", &obj))
	{
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

	Py_RETURN_UOBJECT(track);
}

#if WITH_EDITOR
PyObject *py_ue_sequencer_section_add_key(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	float time;
	PyObject *py_value;
	int interpolation = 0;
	PyObject *py_unwind = nullptr;
	if (!PyArg_ParseTuple(args, "fO|iO:sequencer_section_add_key", &time, &py_value, &interpolation, &py_unwind))
	{
		return nullptr;
	}

	if (!self->ue_object->IsA<UMovieSceneSection>())
		return PyErr_Format(PyExc_Exception, "uobject is not a MovieSceneSection");

	UMovieSceneSection *section = (UMovieSceneSection *)self->ue_object;

	if (auto section_float = Cast<UMovieSceneFloatSection>(section))
	{
		if (PyNumber_Check(py_value))
		{
			PyObject *f_value = PyNumber_Float(py_value);
			float value = PyFloat_AsDouble(f_value);
			Py_DECREF(f_value);
			section_float->AddKey(time, value, (EMovieSceneKeyInterpolation)interpolation);
			Py_RETURN_NONE;
		}
	}

	else if (auto section_bool = Cast<UMovieSceneBoolSection>(section))
	{
		if (PyBool_Check(py_value))
		{
			bool value = false;
			if (PyObject_IsTrue(py_value))
				value = true;
			section_bool->AddKey(time, value, (EMovieSceneKeyInterpolation)interpolation);
			Py_RETURN_NONE;
		}
	}

	else if (auto section_transform = Cast<UMovieScene3DTransformSection>(section))
	{
		if (ue_PyFTransform *py_transform = py_ue_is_ftransform(py_value))
		{
			bool unwind = (py_unwind && PyObject_IsTrue(py_unwind));
			FTransform transform = py_ue_ftransform_get(py_transform);


			FTransformKey tx = FTransformKey(EKey3DTransformChannel::Translation, EAxis::X, transform.GetLocation().X, unwind);
			FTransformKey ty = FTransformKey(EKey3DTransformChannel::Translation, EAxis::Y, transform.GetLocation().Y, unwind);
			FTransformKey tz = FTransformKey(EKey3DTransformChannel::Translation, EAxis::Z, transform.GetLocation().Z, unwind);
			section_transform->AddKey(time, tx, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, ty, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, tz, (EMovieSceneKeyInterpolation)interpolation);


			FTransformKey rx = FTransformKey(EKey3DTransformChannel::Rotation, EAxis::X, transform.GetRotation().Euler().X, unwind);
			FTransformKey ry = FTransformKey(EKey3DTransformChannel::Rotation, EAxis::Y, transform.GetRotation().Euler().Y, unwind);
			FTransformKey rz = FTransformKey(EKey3DTransformChannel::Rotation, EAxis::Z, transform.GetRotation().Euler().Z, unwind);
			section_transform->AddKey(time, rx, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, ry, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, rz, (EMovieSceneKeyInterpolation)interpolation);

			FTransformKey sx = FTransformKey(EKey3DTransformChannel::Scale, EAxis::X, transform.GetScale3D().X, unwind);
			FTransformKey sy = FTransformKey(EKey3DTransformChannel::Scale, EAxis::Y, transform.GetScale3D().Y, unwind);
			FTransformKey sz = FTransformKey(EKey3DTransformChannel::Scale, EAxis::Z, transform.GetScale3D().Z, unwind);
			section_transform->AddKey(time, sx, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, sy, (EMovieSceneKeyInterpolation)interpolation);
			section_transform->AddKey(time, sz, (EMovieSceneKeyInterpolation)interpolation);

			Py_RETURN_NONE;
		}
	}

	else if (auto section_vector = Cast<UMovieSceneVectorSection>(section))
	{
		if (ue_PyFVector *py_vector = py_ue_is_fvector(py_value))
		{
			FVector vec = py_ue_fvector_get(py_vector);
			FVectorKey vx = FVectorKey(EKeyVectorChannel::X, vec.X);
			FVectorKey vy = FVectorKey(EKeyVectorChannel::Y, vec.Y);
			FVectorKey vz = FVectorKey(EKeyVectorChannel::Z, vec.Z);

			section_vector->AddKey(time, vx, (EMovieSceneKeyInterpolation)interpolation);
			section_vector->AddKey(time, vy, (EMovieSceneKeyInterpolation)interpolation);
			section_vector->AddKey(time, vz, (EMovieSceneKeyInterpolation)interpolation);

			Py_RETURN_NONE;
		}
	}

	return PyErr_Format(PyExc_Exception, "unsupported section type: %s", TCHAR_TO_UTF8(*section->GetClass()->GetName()));
}

PyObject *py_ue_sequencer_add_camera_cut_track(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	ULevelSequence *seq = (ULevelSequence *)self->ue_object;
	UMovieScene	*scene = seq->GetMovieScene();

	UMovieSceneTrack *track = scene->AddCameraCutTrack(UMovieSceneCameraCutTrack::StaticClass());

	Py_RETURN_UOBJECT(track);
}


PyObject *py_ue_sequencer_remove_possessable(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *guid;
	if (!PyArg_ParseTuple(args, "s:sequencer_remove_possessable", &guid))
	{
		return nullptr;
	}

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid))
	{
		return PyErr_Format(PyExc_Exception, "invalid guid");
	}

	if (scene->RemovePossessable(f_guid))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

PyObject *py_ue_sequencer_remove_spawnable(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *guid;
	if (!PyArg_ParseTuple(args, "s:sequencer_remove_spawnable", &guid))
	{
		return nullptr;
	}

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

	FGuid f_guid;
	if (!FGuid::Parse(FString(guid), f_guid))
	{
		return PyErr_Format(PyExc_Exception, "invalid guid");
	}

	if (scene->RemoveSpawnable(f_guid))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

PyObject *py_ue_sequencer_remove_camera_cut_track(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

	scene->RemoveCameraCutTrack();

	Py_RETURN_NONE;
}

PyObject *py_ue_sequencer_remove_master_track(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_track;
	if (!PyArg_ParseTuple(args, "O:sequencer_remove_master_track", &py_track))
	{
		return nullptr;
	}

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	UMovieSceneTrack *track = ue_py_check_type<UMovieSceneTrack>(py_track);
	if (!track)
		return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneTrack");

	UMovieScene	*scene = seq->GetMovieScene();

	if (scene->RemoveMasterTrack(*track))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

PyObject *py_ue_sequencer_remove_track(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_track;
	if (!PyArg_ParseTuple(args, "O:sequencer_remove_track", &py_track))
	{
		return nullptr;
	}

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	UMovieSceneTrack *track = ue_py_check_type<UMovieSceneTrack>(py_track);
	if (!track)
		return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneTrack");

	UMovieScene	*scene = seq->GetMovieScene();

	if (scene->RemoveTrack(*track))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

#if ENGINE_MINOR_VERSION >= 18
// Returns the selected sections
PyObject *py_ue_sequencer_get_selected_sections(ue_PyUObject *self, PyObject * args)
{
    ue_py_check(self);

    ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
    if (!seq)
        return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

    IAssetEditorInstance *editor = FAssetEditorManager::Get().FindEditorForAsset(seq, true);
    if (!editor || editor->GetEditorName() != LevelSequenceEditorName)
    {
        return PyErr_Format(PyExc_Exception, "unable to access level sequence editor");
    }

    FLevelSequenceEditorToolkit *toolkit = static_cast<FLevelSequenceEditorToolkit*>(editor);
    ISequencer *sequencer = toolkit->GetSequencer().Get();
    FSequencerSelection seqSelection = sequencer->GetSelection();
    TArray<UMovieSceneSection*> sectionList;
    sequencer->GetSelectedSections(sectionList);

    PyObject *py_sections = PyList_New(0);
    for (UMovieSceneSection* section : sectionList)
    {
        ue_PyUObject *ret = ue_get_python_uobject(section);
        if (!ret)
        {
            Py_DECREF(py_sections);
            return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
        }
        PyList_Append(py_sections, (PyObject *)ret);
    }

    return py_sections;
}
#endif

#endif

PyObject *py_ue_sequencer_add_track(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	char *guid;
	if (!PyArg_ParseTuple(args, "Os:sequencer_add_track", &obj, &guid))
	{
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
	if (!FGuid::Parse(FString(guid), f_guid))
	{
		return PyErr_Format(PyExc_Exception, "invalid guid");
	}
	UMovieSceneTrack *track = scene->AddTrack(u_class, f_guid);
	if (!track)
		return PyErr_Format(PyExc_Exception, "unable to create new track");

	Py_RETURN_UOBJECT(track);
}

#if WITH_EDITOR
// smart functions allowing the set/get of display names to various sequencer objects
PyObject *py_ue_sequencer_set_display_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:sequencer_set_display_name", &name))
	{
		return NULL;
	}

	if (self->ue_object->IsA<UMovieSceneNameableTrack>())
	{
		UMovieSceneNameableTrack *track = (UMovieSceneNameableTrack *)self->ue_object;
		track->SetDisplayName(FText::FromString(UTF8_TO_TCHAR(name)));
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "the uobject does not expose the SetDisplayName() method");
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_sequencer_get_display_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (self->ue_object->IsA<UMovieSceneNameableTrack>())
	{
		UMovieSceneNameableTrack *track = (UMovieSceneNameableTrack *)self->ue_object;
		FText name = track->GetDefaultDisplayName();
		return PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString()));
	}

	return PyErr_Format(PyExc_Exception, "the uobject does not expose the GetDefaultDisplayName() method");
}

PyObject *py_ue_sequencer_get_track_display_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (self->ue_object->IsA<UMovieSceneNameableTrack>())
	{
		UMovieSceneNameableTrack *track = (UMovieSceneNameableTrack *)self->ue_object;
		FText name = track->GetDisplayName();
		return PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString()));
	}

	return PyErr_Format(PyExc_Exception, "the uobject does not expose the GetDisplayName() method");
}

PyObject *py_ue_sequencer_get_track_unique_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (self->ue_object->IsA<UMovieSceneNameableTrack>())
	{
		UMovieSceneNameableTrack *track = (UMovieSceneNameableTrack *)self->ue_object;
		FName name = track->GetTrackName();
		return PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString()));
	}

	return PyErr_Format(PyExc_Exception, "the uobject does not expose the GetDisplayName() method");
}

PyObject *py_ue_sequencer_import_fbx_transform(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	char *filename;
	char *nodename;
	PyObject *py_force_front_x_axis = nullptr;

	if (!PyArg_ParseTuple(args, "ss|o:sequencer_import_fbx_transform", &filename, &nodename, &py_force_front_x_axis))
		return nullptr;

	UMovieScene3DTransformSection *section = ue_py_check_type<UMovieScene3DTransformSection>(self);
	if (!section)
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieScene3DTransformSection");

	UnFbx::FFbxImporter* FbxImporter = UnFbx::FFbxImporter::GetInstance();

	UnFbx::FBXImportOptions* ImportOptions = FbxImporter->GetImportOptions();
	bool bConverteScene = ImportOptions->bConvertScene;
	bool bConverteSceneUnit = ImportOptions->bConvertSceneUnit;
	bool bForceFrontXAxis = ImportOptions->bForceFrontXAxis;

	ImportOptions->bConvertScene = true;
	ImportOptions->bConvertSceneUnit = true;
	ImportOptions->bForceFrontXAxis = py_force_front_x_axis && PyObject_IsTrue(py_force_front_x_axis);

	FString FbxFilename = FString(UTF8_TO_TCHAR(filename));
	FString FbxNodeName = FString(UTF8_TO_TCHAR(nodename));

	const FString Extension = FPaths::GetExtension(FbxFilename);
	if (!FbxImporter->ImportFromFile(FbxFilename, Extension, true))
	{
		FbxImporter->ReleaseScene();
		ImportOptions->bConvertScene = bConverteScene;
		ImportOptions->bConvertSceneUnit = bConverteSceneUnit;
		ImportOptions->bForceFrontXAxis = bForceFrontXAxis;
		return PyErr_Format(PyExc_Exception, "unable to import Fbx file");
	}

	UnFbx::FFbxCurvesAPI CurveAPI;
	FbxImporter->PopulateAnimatedCurveData(CurveAPI);

	TArray<FString> AllNodeNames;
#if ENGINE_MINOR_VERSION < 18
	CurveAPI.GetAnimatedNodeNameArray(AllNodeNames);
#else
	CurveAPI.GetAllNodeNameArray(AllNodeNames);
#endif

	for (FString NodeName : AllNodeNames)
	{
		if (NodeName != FbxNodeName)
			continue;

		// Look for transforms explicitly
		FInterpCurveFloat Translation[3];
		FInterpCurveFloat EulerRotation[3];
		FInterpCurveFloat Scale[3];
		FTransform DefaultTransform;
#if ENGINE_MINOR_VERSION >= 18
		CurveAPI.GetConvertedTransformCurveData(NodeName, Translation[0], Translation[1], Translation[2], EulerRotation[0], EulerRotation[1], EulerRotation[2], Scale[0], Scale[1], Scale[2], DefaultTransform);

		for (int32 ChannelIndex = 0; ChannelIndex < 3; ++ChannelIndex)
		{
			EAxis::Type ChannelAxis = EAxis::X;
			if (ChannelIndex == 1)
			{
				ChannelAxis = EAxis::Y;
			}
			else if (ChannelIndex == 2)
			{
				ChannelAxis = EAxis::Z;
			}
			section->GetTranslationCurve(ChannelAxis).SetDefaultValue(DefaultTransform.GetLocation()[ChannelIndex]);
			section->GetRotationCurve(ChannelAxis).SetDefaultValue(DefaultTransform.GetRotation().Euler()[ChannelIndex]);
			section->GetScaleCurve(ChannelAxis).SetDefaultValue(DefaultTransform.GetScale3D()[ChannelIndex]);
	}
#else
		CurveAPI.GetConvertedTransformCurveData(NodeName, Translation[0], Translation[1], Translation[2], EulerRotation[0], EulerRotation[1], EulerRotation[2], Scale[0], Scale[1], Scale[2]);

#endif

		float MinTime = FLT_MAX;
		float MaxTime = -FLT_MAX;

		const int NumCurves = 3; // Trans, Rot, Scale
		for (int32 CurveIndex = 0; CurveIndex < NumCurves; ++CurveIndex)
		{
			for (int32 ChannelIndex = 0; ChannelIndex < 3; ++ChannelIndex)
			{
				EAxis::Type ChannelAxis = EAxis::X;
				if (ChannelIndex == 1)
				{
					ChannelAxis = EAxis::Y;
				}
				else if (ChannelIndex == 2)
				{
					ChannelAxis = EAxis::Z;
				}

				FInterpCurveFloat* CurveFloat = nullptr;
				FRichCurve* ChannelCurve = nullptr;
				bool bNegative = false;

				if (CurveIndex == 0)
				{
					CurveFloat = &Translation[ChannelIndex];
					ChannelCurve = &section->GetTranslationCurve(ChannelAxis);
					if (ChannelIndex == 1)
					{
						bNegative = true;
					}
				}
				else if (CurveIndex == 1)
				{
					CurveFloat = &EulerRotation[ChannelIndex];
					ChannelCurve = &section->GetRotationCurve(ChannelAxis);
					if (ChannelIndex == 1 || ChannelIndex == 2)
					{
						bNegative = true;
					}
				}
				else if (CurveIndex == 2)
				{
					CurveFloat = &Scale[ChannelIndex];
					ChannelCurve = &section->GetScaleCurve(ChannelAxis);
				}

				if (ChannelCurve != nullptr && CurveFloat != nullptr)
				{
					ChannelCurve->Reset();

					for (int32 KeyIndex = 0; KeyIndex < CurveFloat->Points.Num(); ++KeyIndex)
					{
						MinTime = FMath::Min(MinTime, CurveFloat->Points[KeyIndex].InVal);
						MaxTime = FMath::Max(MaxTime, CurveFloat->Points[KeyIndex].InVal);

						float ArriveTangent = CurveFloat->Points[KeyIndex].ArriveTangent;
						if (KeyIndex > 0)
						{
							ArriveTangent = ArriveTangent / (CurveFloat->Points[KeyIndex].InVal - CurveFloat->Points[KeyIndex - 1].InVal);
						}

						float LeaveTangent = CurveFloat->Points[KeyIndex].LeaveTangent;
						if (KeyIndex < CurveFloat->Points.Num() - 1)
						{
							LeaveTangent = LeaveTangent / (CurveFloat->Points[KeyIndex + 1].InVal - CurveFloat->Points[KeyIndex].InVal);
						}

						if (bNegative)
						{
							ArriveTangent = -ArriveTangent;
							LeaveTangent = -LeaveTangent;
						}

						FMatineeImportTools::SetOrAddKey(*ChannelCurve, CurveFloat->Points[KeyIndex].InVal, CurveFloat->Points[KeyIndex].OutVal, ArriveTangent, LeaveTangent, CurveFloat->Points[KeyIndex].InterpMode);
					}

					ChannelCurve->RemoveRedundantKeys(KINDA_SMALL_NUMBER);
					ChannelCurve->AutoSetTangents();
				}
			}
		}

		section->SetStartTime(MinTime);
		section->SetEndTime(MaxTime);

		FbxImporter->ReleaseScene();
		ImportOptions->bConvertScene = bConverteScene;
		ImportOptions->bConvertSceneUnit = bConverteScene;
		ImportOptions->bForceFrontXAxis = bConverteScene;
		Py_RETURN_NONE;
}

	FbxImporter->ReleaseScene();
	ImportOptions->bConvertScene = bConverteScene;
	ImportOptions->bConvertSceneUnit = bConverteSceneUnit;
	ImportOptions->bForceFrontXAxis = bForceFrontXAxis;
	return PyErr_Format(PyExc_Exception, "unable to find specified node in Fbx file");
}
#endif

