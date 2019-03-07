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
#if ENGINE_MINOR_VERSION >= 20
#include "LevelSequenceEditor/Private/LevelSequenceEditorToolkit.h"
#else
#include "Private/LevelSequenceEditorToolkit.h"
#endif
#include "Tracks/MovieSceneCameraCutTrack.h"
#if ENGINE_MINOR_VERSION < 20
#include "Sections/IKeyframeSection.h"
#endif
#include "Sections/MovieSceneFloatSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneVectorSection.h"
#include "Runtime/MovieScene/Public/MovieSceneFolder.h"
#include "Runtime/MovieScene/Public/MovieSceneSpawnable.h"
#include "Runtime/MovieScene/Public/MovieScenePossessable.h"
#if ENGINE_MINOR_VERSION < 18
#include "Editor/UnrealEd/Private/FbxImporter.h"
#else
#include "Editor/UnrealEd/Public/FbxImporter.h"
#endif
#include "Editor/MovieSceneTools/Public/MatineeImportTools.h"
#endif

#include "GameFramework/Actor.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Engine/World.h"

#if ENGINE_MINOR_VERSION >= 20
#include "Wrappers/UEPyFFrameNumber.h"
#endif


#if ENGINE_MINOR_VERSION >= 20
static bool magic_get_frame_number(UMovieScene *MovieScene, PyObject *py_obj, FFrameNumber *dest)
{
	ue_PyFFrameNumber *py_frame_number = py_ue_is_fframe_number(py_obj);
	if (py_frame_number)
	{
		*dest = py_frame_number->frame_number;
		return true;
	}

	if (PyNumber_Check(py_obj))
	{
		PyObject *f_value = PyNumber_Float(py_obj);
		float value = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		*dest = MovieScene->GetTickResolution().AsFrameNumber(value);
		return true;
	}

	return false;

}

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 21
static void ImportTransformChannel(const FRichCurve& Source, FMovieSceneFloatChannel* Dest, FFrameRate DestFrameRate, bool bNegateTangents)
{
	TMovieSceneChannelData<FMovieSceneFloatValue> ChannelData = Dest->GetData();
	ChannelData.Reset();
	double DecimalRate = DestFrameRate.AsDecimal();
	for (int32 KeyIndex = 0; KeyIndex < Source.Keys.Num(); ++KeyIndex)
	{
		float ArriveTangent = Source.Keys[KeyIndex].ArriveTangent;
		if (KeyIndex > 0)
		{
			ArriveTangent = ArriveTangent / ((Source.Keys[KeyIndex].Value - Source.Keys[KeyIndex - 1].Value) * DecimalRate);
		}

		float LeaveTangent = Source.Keys[KeyIndex].LeaveTangent;
		if (KeyIndex < Source.Keys.Num() - 1)
		{
			LeaveTangent = LeaveTangent / ((Source.Keys[KeyIndex + 1].Value - Source.Keys[KeyIndex].Value) * DecimalRate);
		}

		if (bNegateTangents)
		{
			ArriveTangent = -ArriveTangent;
			LeaveTangent = -LeaveTangent;
		}
		
		FFrameNumber KeyTime = (Source.Keys[KeyIndex].Value * DestFrameRate).RoundToFrame();
		if (ChannelData.FindKey(KeyTime) == INDEX_NONE)
		{
			FMovieSceneFloatValue NewKey(Source.Keys[KeyIndex].Value);

			NewKey.InterpMode = Source.Keys[KeyIndex].InterpMode;
			NewKey.TangentMode = Source.Keys[KeyIndex].TangentMode;
			NewKey.Tangent.ArriveTangent = ArriveTangent / DestFrameRate.AsDecimal();
			NewKey.Tangent.LeaveTangent = LeaveTangent / DestFrameRate.AsDecimal();
			NewKey.Tangent.TangentWeightMode = RCTWM_WeightedNone;
			NewKey.Tangent.ArriveTangentWeight = 0.0f;
			NewKey.Tangent.LeaveTangentWeight = 0.0f;
			ChannelData.AddKey(KeyTime, NewKey);
		}
	}

	Dest->AutoSetTangents();
}
#else
static void ImportTransformChannel(const FInterpCurveFloat& Source, FMovieSceneFloatChannel* Dest, FFrameRate DestFrameRate, bool bNegateTangents)
{
	TMovieSceneChannelData<FMovieSceneFloatValue> ChannelData = Dest->GetData();
	ChannelData.Reset();
	double DecimalRate = DestFrameRate.AsDecimal();
	for (int32 KeyIndex = 0; KeyIndex < Source.Points.Num(); ++KeyIndex)
	{
		float ArriveTangent = Source.Points[KeyIndex].ArriveTangent;
		if (KeyIndex > 0)
		{
			ArriveTangent = ArriveTangent / ((Source.Points[KeyIndex].InVal - Source.Points[KeyIndex - 1].InVal) * DecimalRate);
		}

		float LeaveTangent = Source.Points[KeyIndex].LeaveTangent;
		if (KeyIndex < Source.Points.Num() - 1)
		{
			LeaveTangent = LeaveTangent / ((Source.Points[KeyIndex + 1].InVal - Source.Points[KeyIndex].InVal) * DecimalRate);
		}

		if (bNegateTangents)
		{
			ArriveTangent = -ArriveTangent;
			LeaveTangent = -LeaveTangent;
		}

		FFrameNumber KeyTime = (Source.Points[KeyIndex].InVal * DestFrameRate).RoundToFrame();
#if ENGINE_MINOR_VERSION > 20
		FMatineeImportTools::SetOrAddKey(ChannelData, KeyTime, Source.Points[KeyIndex].OutVal, ArriveTangent, LeaveTangent, Source.Points[KeyIndex].InterpMode, DestFrameRate);
#else
		FMatineeImportTools::SetOrAddKey(ChannelData, KeyTime, Source.Points[KeyIndex].OutVal, ArriveTangent, LeaveTangent, Source.Points[KeyIndex].InterpMode);
#endif
	}

	Dest->AutoSetTangents();
}
#endif

static bool ImportFBXTransform(FString NodeName, UMovieScene3DTransformSection* TransformSection, UnFbx::FFbxCurvesAPI& CurveAPI)
{


	// Look for transforms explicitly
	FTransform DefaultTransform;

#if ENGINE_MINOR_VERSION > 21
	FRichCurve Translation[3];
	FRichCurve EulerRotation[3];
	FRichCurve Scale[3];
#else
	FInterpCurveFloat Translation[3];
	FInterpCurveFloat EulerRotation[3];
	FInterpCurveFloat Scale[3];
#endif
	CurveAPI.GetConvertedTransformCurveData(NodeName, Translation[0], Translation[1], Translation[2], EulerRotation[0], EulerRotation[1], EulerRotation[2], Scale[0], Scale[1], Scale[2], DefaultTransform);


	TransformSection->Modify();

	FFrameRate FrameRate = TransformSection->GetTypedOuter<UMovieScene>()->GetTickResolution();


	FVector Location = DefaultTransform.GetLocation(), Rotation = DefaultTransform.GetRotation().Euler(), Scale3D = DefaultTransform.GetScale3D();

	TArrayView<FMovieSceneFloatChannel*> Channels = TransformSection->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();

	Channels[0]->SetDefault(Location.X);
	Channels[1]->SetDefault(Location.Y);
	Channels[2]->SetDefault(Location.Z);

	Channels[3]->SetDefault(Rotation.X);
	Channels[4]->SetDefault(Rotation.Y);
	Channels[5]->SetDefault(Rotation.Z);

	Channels[6]->SetDefault(Scale3D.X);
	Channels[7]->SetDefault(Scale3D.Y);
	Channels[8]->SetDefault(Scale3D.Z);

	ImportTransformChannel(Translation[0], Channels[0], FrameRate, false);
	ImportTransformChannel(Translation[1], Channels[1], FrameRate, true);
	ImportTransformChannel(Translation[2], Channels[2], FrameRate, false);

	ImportTransformChannel(EulerRotation[0], Channels[3], FrameRate, false);
	ImportTransformChannel(EulerRotation[1], Channels[4], FrameRate, true);
	ImportTransformChannel(EulerRotation[2], Channels[5], FrameRate, true);

	ImportTransformChannel(Scale[0], Channels[6], FrameRate, false);
	ImportTransformChannel(Scale[1], Channels[7], FrameRate, false);
	ImportTransformChannel(Scale[2], Channels[8], FrameRate, false);

	return true;
}
#endif

#endif

#if WITH_EDITOR
PyObject *py_ue_sequencer_changed(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "|O:sequencer_changed", &py_bool))
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
#if ENGINE_MINOR_VERSION > 16
		sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::RefreshAllImmediately);
#else
		sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
#endif
#endif

	}

	Py_RETURN_NONE;
}
#endif

PyObject *py_ue_sequencer_possessable_tracks(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_possessable;
	if (!PyArg_ParseTuple(args, "O:sequencer_possessable_tracks", &py_possessable))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<ULevelSequence>())
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");

	FGuid f_guid;
	if (PyUnicodeOrString_Check(py_possessable))
	{
		const char *guid = UEPyUnicode_AsUTF8(py_possessable);
		if (!FGuid::Parse(FString(guid), f_guid))
		{
			return PyErr_Format(PyExc_Exception, "invalid GUID");
		}
	}
	else
	{
		FMovieScenePossessable *possessable = (FMovieScenePossessable *)do_ue_py_check_struct(py_possessable, FMovieScenePossessable::StaticStruct());
		if (possessable)
		{
			f_guid = possessable->GetGuid();
		}

	}

	if (!f_guid.IsValid())
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
	PyObject *ret = py_ue_new_owned_uscriptstruct(spawnable->StaticStruct(), (uint8 *)spawnable);
	Py_INCREF(ret);
	return ret;
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
		PyObject *py_possessable = py_ue_new_owned_uscriptstruct(possessable.StaticStruct(), (uint8 *)&possessable);
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

	PyObject *py_section = nullptr;
	if (!PyArg_ParseTuple(args, "|O:sequencer_track_add_section", &py_section))
		return nullptr;


	UMovieSceneTrack *track = ue_py_check_type<UMovieSceneTrack>(self);
	if (!track)
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneTrack");

	UMovieSceneSection *new_section = nullptr;

	if (!py_section)
	{
		new_section = track->CreateNewSection();
	}
	else
	{
		new_section = ue_py_check_type<UMovieSceneSection>(py_section);
		if (!new_section)
		{
			UClass *u_class = ue_py_check_type<UClass>(py_section);
			if (u_class && u_class->IsChildOf<UMovieSceneSection>())
			{
				new_section = NewObject<UMovieSceneSection>(track, u_class, NAME_None);
			}
		}
	}

	if (!new_section)
		return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneSection");

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

PyObject *py_ue_sequencer_set_playback_range(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

#if ENGINE_MINOR_VERSION < 20
	float start_time;
	float end_time;
	if (!PyArg_ParseTuple(args, "ff:sequencer_set_playback_range", &start_time, &end_time))
	{
		return nullptr;
	}

	scene->SetPlaybackRange(start_time, end_time);
#else
	PyObject *py_start;
	PyObject *py_end;
	if (!PyArg_ParseTuple(args, "OO:sequencer_set_playback_range", &py_start, &py_end))
	{
		return nullptr;
	}

	FFrameNumber FrameStart;
	FFrameNumber FrameEnd;

	if (!magic_get_frame_number(scene, py_start, &FrameStart))
		return PyErr_Format(PyExc_Exception, "range must use float or FrameNumber");

	if (!magic_get_frame_number(scene, py_end, &FrameEnd))
		return PyErr_Format(PyExc_Exception, "range must use float or FrameNumber");

	scene->SetPlaybackRange(TRange<FFrameNumber>::Inclusive(FrameStart, FrameEnd));

#endif

	Py_RETURN_NONE;
}

PyObject *py_ue_sequencer_get_playback_range(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

#if ENGINE_MINOR_VERSION < 20
	TRange<float> range = scene->GetPlaybackRange();
	return Py_BuildValue("(ff)", range.GetLowerBoundValue(), range.GetUpperBoundValue());
#else
	TRange<FFrameNumber> range = scene->GetPlaybackRange();

	return Py_BuildValue("(OO)", py_ue_new_fframe_number(range.GetLowerBoundValue()), py_ue_new_fframe_number(range.GetUpperBoundValue()));

#endif
}

PyObject *py_ue_sequencer_set_working_range(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

	float start_time;
	float end_time;
	if (!PyArg_ParseTuple(args, "ff:sequencer_set_working_range", &start_time, &end_time))
	{
		return nullptr;
	}

	scene->SetWorkingRange(start_time, end_time);

	Py_RETURN_NONE;
}

PyObject *py_ue_sequencer_set_view_range(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

	float start_time;
	float end_time;
	if (!PyArg_ParseTuple(args, "ff:sequencer_set_view_range", &start_time, &end_time))
	{
		return nullptr;
	}

	scene->SetViewRange(start_time, end_time);

	Py_RETURN_NONE;
}

PyObject *py_ue_sequencer_set_section_range(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UMovieSceneSection *section = ue_py_check_type<UMovieSceneSection>(self);
	if (!section)
		return PyErr_Format(PyExc_Exception, "uobject is not a MovieSceneSection");

#if ENGINE_MINOR_VERSION < 20
	float start_time;
	float end_time;
	if (!PyArg_ParseTuple(args, "ff:sequencer_set_section_range", &start_time, &end_time))
	{
		return nullptr;
	}

#if ENGINE_MINOR_VERSION > 17
	section->SetRange(TRange<float>::Inclusive(start_time, end_time));
#else
	section->SetRange(TRange<float>(TRangeBound<float>::Inclusive(start_time), TRangeBound<float>::Inclusive(end_time)));
#endif


#else
	PyObject *py_start;
	PyObject *py_end;
	if (!PyArg_ParseTuple(args, "OO:sequencer_set_section_range", &py_start, &py_end))
	{
		return nullptr;
	}

	UMovieSceneTrack *Track = section->GetTypedOuter<UMovieSceneTrack>();
	if (!Track)
		return PyErr_Format(PyExc_Exception, "unable to retrieve track from section");
	UMovieScene *MovieScene = Track->GetTypedOuter<UMovieScene>();
	if (!MovieScene)
		return PyErr_Format(PyExc_Exception, "unable to retrieve scene from section");

	FFrameNumber FrameStart;
	FFrameNumber FrameEnd;

	if (!magic_get_frame_number(MovieScene, py_start, &FrameStart))
		return PyErr_Format(PyExc_Exception, "range must use float or FrameNumber");

	if (!magic_get_frame_number(MovieScene, py_end, &FrameEnd))
		return PyErr_Format(PyExc_Exception, "range must use float or FrameNumber");

	section->SetRange(TRange<FFrameNumber>::Inclusive(FrameStart, FrameEnd));
#endif

	Py_RETURN_NONE;
}

PyObject *py_ue_sequencer_get_selection_range(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ULevelSequence *seq = ue_py_check_type<ULevelSequence>(self);
	if (!seq)
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequence");
	UMovieScene	*scene = seq->GetMovieScene();

#if ENGINE_MINOR_VERSION < 20
	TRange<float> range = scene->GetSelectionRange();
	return Py_BuildValue("(ff)", range.GetLowerBoundValue(), range.GetUpperBoundValue());
#else
	TRange<FFrameNumber> range = scene->GetSelectionRange();

	return Py_BuildValue("(OO)", py_ue_new_fframe_number(range.GetLowerBoundValue()), py_ue_new_fframe_number(range.GetUpperBoundValue()));

#endif
}


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

	UMovieSceneSection *section = ue_py_check_type<UMovieSceneSection>(self);
	if (!section)
		return PyErr_Format(PyExc_Exception, "uobject is not a MovieSceneSection");

#if ENGINE_MINOR_VERSION >= 20
	UMovieSceneTrack *Track = section->GetTypedOuter<UMovieSceneTrack>();
	if (!Track)
		return PyErr_Format(PyExc_Exception, "unable to retrieve track from section");
	UMovieScene *MovieScene = Track->GetTypedOuter<UMovieScene>();
	if (!MovieScene)
		return PyErr_Format(PyExc_Exception, "unable to retrieve scene from section");

	FFrameNumber FrameNumber = MovieScene->GetTickResolution().AsFrameNumber(time);
	EMovieSceneKeyInterpolation InterpolationMode = (EMovieSceneKeyInterpolation)interpolation;

	section->Modify();
#endif

	if (auto section_float = Cast<UMovieSceneFloatSection>(section))
	{
		if (PyNumber_Check(py_value))
		{
			PyObject *f_value = PyNumber_Float(py_value);
			float value = PyFloat_AsDouble(f_value);
			Py_DECREF(f_value);
#if ENGINE_MINOR_VERSION < 20
			section_float->AddKey(time, value, (EMovieSceneKeyInterpolation)interpolation);
#else
			FMovieSceneFloatChannel& Channel = (FMovieSceneFloatChannel&)section_float->GetChannel();
			int32 RetValue = -1;
			switch (InterpolationMode)
			{
			case(EMovieSceneKeyInterpolation::Auto):
				RetValue = Channel.AddCubicKey(FrameNumber, value, RCTM_Auto);
				break;
			case(EMovieSceneKeyInterpolation::User):
				RetValue = Channel.AddCubicKey(FrameNumber, value, RCTM_User);
			case(EMovieSceneKeyInterpolation::Break):
				RetValue = Channel.AddCubicKey(FrameNumber, value, RCTM_Break);
				break;
			case(EMovieSceneKeyInterpolation::Linear):
				RetValue = Channel.AddLinearKey(FrameNumber, value);
				break;
			case(EMovieSceneKeyInterpolation::Constant):
				RetValue = Channel.AddConstantKey(FrameNumber, value);
				break;
			default:
				return PyErr_Format(PyExc_Exception, "unsupported interpolation");
			}
			return PyLong_FromLong(RetValue);
#endif
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
#if ENGINE_MINOR_VERSION < 20
			section_bool->AddKey(time, value, (EMovieSceneKeyInterpolation)interpolation);
#else
			FMovieSceneBoolChannel& Channel = section_bool->GetChannel();
			int32 RetValue = Channel.GetData().AddKey(FrameNumber, value);
			return PyLong_FromLong(RetValue);
#endif
			Py_RETURN_NONE;
		}
	}

	else if (auto section_transform = Cast<UMovieScene3DTransformSection>(section))
	{
		if (ue_PyFTransform *py_transform = py_ue_is_ftransform(py_value))
		{
			bool unwind = (py_unwind && PyObject_IsTrue(py_unwind));
			FTransform transform = py_transform->transform;

#if ENGINE_MINOR_VERSION < 20
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
#else
			int RetValueTX, RetValueTY, RetValueTZ = -1;
			int RetValueRX, RetValueRY, RetValueRZ = -1;
			int RetValueSX, RetValueSY, RetValueSZ = -1;
			FMovieSceneFloatChannel *ChannelTX = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(0);
			FMovieSceneFloatChannel *ChannelTY = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(1);
			FMovieSceneFloatChannel *ChannelTZ = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(2);
			FMovieSceneFloatChannel *ChannelRX = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(3);
			FMovieSceneFloatChannel *ChannelRY = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(4);
			FMovieSceneFloatChannel *ChannelRZ = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(5);
			FMovieSceneFloatChannel *ChannelSX = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(6);
			FMovieSceneFloatChannel *ChannelSY = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(7);
			FMovieSceneFloatChannel *ChannelSZ = section_transform->GetChannelProxy().GetChannel<FMovieSceneFloatChannel>(8);

			switch (InterpolationMode)
			{
			case(EMovieSceneKeyInterpolation::Auto):
				RetValueTX = ChannelTX->AddCubicKey(FrameNumber, transform.GetTranslation().X, RCTM_Auto);
				RetValueTY = ChannelTY->AddCubicKey(FrameNumber, transform.GetTranslation().Y, RCTM_Auto);
				RetValueTZ = ChannelTZ->AddCubicKey(FrameNumber, transform.GetTranslation().Z, RCTM_Auto);
				RetValueRX = ChannelRX->AddCubicKey(FrameNumber, transform.GetRotation().Euler().X, RCTM_Auto);
				RetValueRY = ChannelRY->AddCubicKey(FrameNumber, transform.GetRotation().Euler().Y, RCTM_Auto);
				RetValueRZ = ChannelRZ->AddCubicKey(FrameNumber, transform.GetRotation().Euler().Z, RCTM_Auto);
				RetValueSX = ChannelSX->AddCubicKey(FrameNumber, transform.GetScale3D().X, RCTM_Auto);
				RetValueSY = ChannelSY->AddCubicKey(FrameNumber, transform.GetScale3D().Y, RCTM_Auto);
				RetValueSZ = ChannelSZ->AddCubicKey(FrameNumber, transform.GetScale3D().Z, RCTM_Auto);
				break;
			case(EMovieSceneKeyInterpolation::User):
				RetValueTX = ChannelTX->AddCubicKey(FrameNumber, transform.GetTranslation().X, RCTM_User);
				RetValueTY = ChannelTY->AddCubicKey(FrameNumber, transform.GetTranslation().Y, RCTM_User);
				RetValueTZ = ChannelTZ->AddCubicKey(FrameNumber, transform.GetTranslation().Z, RCTM_User);
				RetValueRX = ChannelRX->AddCubicKey(FrameNumber, transform.GetRotation().Euler().X, RCTM_User);
				RetValueRY = ChannelRY->AddCubicKey(FrameNumber, transform.GetRotation().Euler().Y, RCTM_User);
				RetValueRZ = ChannelRZ->AddCubicKey(FrameNumber, transform.GetRotation().Euler().Z, RCTM_User);
				RetValueSX = ChannelSX->AddCubicKey(FrameNumber, transform.GetScale3D().X, RCTM_User);
				RetValueSY = ChannelSY->AddCubicKey(FrameNumber, transform.GetScale3D().Y, RCTM_User);
				RetValueSZ = ChannelSZ->AddCubicKey(FrameNumber, transform.GetScale3D().Z, RCTM_User);
			case(EMovieSceneKeyInterpolation::Break):
				RetValueTX = ChannelTX->AddCubicKey(FrameNumber, transform.GetTranslation().X, RCTM_Break);
				RetValueTY = ChannelTY->AddCubicKey(FrameNumber, transform.GetTranslation().Y, RCTM_Break);
				RetValueTZ = ChannelTZ->AddCubicKey(FrameNumber, transform.GetTranslation().Z, RCTM_Break);
				RetValueRX = ChannelRX->AddCubicKey(FrameNumber, transform.GetRotation().Euler().X, RCTM_Break);
				RetValueRY = ChannelRY->AddCubicKey(FrameNumber, transform.GetRotation().Euler().Y, RCTM_Break);
				RetValueRZ = ChannelRZ->AddCubicKey(FrameNumber, transform.GetRotation().Euler().Z, RCTM_Break);
				RetValueSX = ChannelSX->AddCubicKey(FrameNumber, transform.GetScale3D().X, RCTM_Break);
				RetValueSY = ChannelSY->AddCubicKey(FrameNumber, transform.GetScale3D().Y, RCTM_Break);
				RetValueSZ = ChannelSZ->AddCubicKey(FrameNumber, transform.GetScale3D().Z, RCTM_Break);
				break;
			case(EMovieSceneKeyInterpolation::Linear):
				RetValueTX = ChannelTX->AddLinearKey(FrameNumber, transform.GetTranslation().X);
				RetValueTY = ChannelTY->AddLinearKey(FrameNumber, transform.GetTranslation().Y);
				RetValueTZ = ChannelTZ->AddLinearKey(FrameNumber, transform.GetTranslation().Z);
				RetValueRX = ChannelRX->AddLinearKey(FrameNumber, transform.GetRotation().Euler().X);
				RetValueRY = ChannelRY->AddLinearKey(FrameNumber, transform.GetRotation().Euler().Y);
				RetValueRZ = ChannelRZ->AddLinearKey(FrameNumber, transform.GetRotation().Euler().Z);
				RetValueSX = ChannelSX->AddLinearKey(FrameNumber, transform.GetScale3D().X);
				RetValueSY = ChannelSY->AddLinearKey(FrameNumber, transform.GetScale3D().Y);
				RetValueSZ = ChannelSZ->AddLinearKey(FrameNumber, transform.GetScale3D().Z);
				break;
			case(EMovieSceneKeyInterpolation::Constant):
				RetValueTX = ChannelTX->AddConstantKey(FrameNumber, transform.GetTranslation().X);
				RetValueTY = ChannelTY->AddConstantKey(FrameNumber, transform.GetTranslation().Y);
				RetValueTZ = ChannelTZ->AddConstantKey(FrameNumber, transform.GetTranslation().Z);
				RetValueRX = ChannelRX->AddConstantKey(FrameNumber, transform.GetRotation().Euler().X);
				RetValueRY = ChannelRY->AddConstantKey(FrameNumber, transform.GetRotation().Euler().Y);
				RetValueRZ = ChannelRZ->AddConstantKey(FrameNumber, transform.GetRotation().Euler().Z);
				RetValueSX = ChannelSX->AddConstantKey(FrameNumber, transform.GetScale3D().X);
				RetValueSY = ChannelSY->AddConstantKey(FrameNumber, transform.GetScale3D().Y);
				RetValueSZ = ChannelSZ->AddConstantKey(FrameNumber, transform.GetScale3D().Z);
				break;
			default:
				return PyErr_Format(PyExc_Exception, "unsupported interpolation");
			}
			return Py_BuildValue("((iii)(iii)(iii))", RetValueTX, RetValueTY, RetValueTZ, RetValueRX, RetValueRY, RetValueRZ, RetValueSX, RetValueSY, RetValueSZ);
#endif
		}
	}

	else if (auto section_vector = Cast<UMovieSceneVectorSection>(section))
	{
		if (ue_PyFVector *py_vector = py_ue_is_fvector(py_value))
		{
			FVector vec = py_vector->vec;
#if ENGINE_MINOR_VERSION < 20
			FVectorKey vx = FVectorKey(EKeyVectorChannel::X, vec.X);
			FVectorKey vy = FVectorKey(EKeyVectorChannel::Y, vec.Y);
			FVectorKey vz = FVectorKey(EKeyVectorChannel::Z, vec.Z);


			section_vector->AddKey(time, vx, (EMovieSceneKeyInterpolation)interpolation);
			section_vector->AddKey(time, vy, (EMovieSceneKeyInterpolation)interpolation);
			section_vector->AddKey(time, vz, (EMovieSceneKeyInterpolation)interpolation);
#else
			int RetValueVX, RetValueVY, RetValueVZ = -1;

			FMovieSceneFloatChannel& ChannelX = (FMovieSceneFloatChannel&)section_vector->GetChannel(0);
			FMovieSceneFloatChannel& ChannelY = (FMovieSceneFloatChannel&)section_vector->GetChannel(1);
			FMovieSceneFloatChannel& ChannelZ = (FMovieSceneFloatChannel&)section_vector->GetChannel(2);

			switch (InterpolationMode)
			{
			case(EMovieSceneKeyInterpolation::Auto):
				RetValueVX = ChannelX.AddCubicKey(FrameNumber, vec.X, RCTM_Auto);
				RetValueVY = ChannelY.AddCubicKey(FrameNumber, vec.Y, RCTM_Auto);
				RetValueVZ = ChannelZ.AddCubicKey(FrameNumber, vec.Z, RCTM_Auto);
				break;
			case(EMovieSceneKeyInterpolation::User):
				RetValueVX = ChannelX.AddCubicKey(FrameNumber, vec.X, RCTM_User);
				RetValueVY = ChannelY.AddCubicKey(FrameNumber, vec.Y, RCTM_User);
				RetValueVZ = ChannelZ.AddCubicKey(FrameNumber, vec.Z, RCTM_User);
			case(EMovieSceneKeyInterpolation::Break):
				RetValueVX = ChannelX.AddCubicKey(FrameNumber, vec.X, RCTM_Break);
				RetValueVY = ChannelY.AddCubicKey(FrameNumber, vec.Y, RCTM_Break);
				RetValueVZ = ChannelZ.AddCubicKey(FrameNumber, vec.Z, RCTM_Break);
				break;
			case(EMovieSceneKeyInterpolation::Linear):
				RetValueVX = ChannelX.AddLinearKey(FrameNumber, vec.X);
				RetValueVY = ChannelY.AddLinearKey(FrameNumber, vec.Y);
				RetValueVZ = ChannelZ.AddLinearKey(FrameNumber, vec.Z);
				break;
			case(EMovieSceneKeyInterpolation::Constant):
				RetValueVX = ChannelX.AddConstantKey(FrameNumber, vec.X);
				RetValueVY = ChannelY.AddConstantKey(FrameNumber, vec.Y);
				RetValueVZ = ChannelZ.AddConstantKey(FrameNumber, vec.Z);
				break;
			default:
				return PyErr_Format(PyExc_Exception, "unsupported interpolation");
			}
			return Py_BuildValue("(iii)", RetValueVX, RetValueVY, RetValueVZ);
#endif

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
#if ENGINE_MINOR_VERSION > 21
		FRichCurve Translation[3];
		FRichCurve EulerRotation[3];
		FRichCurve Scale[3];
#else
		FInterpCurveFloat Translation[3];
		FInterpCurveFloat EulerRotation[3];
		FInterpCurveFloat Scale[3];
#endif
		FTransform DefaultTransform;
#if ENGINE_MINOR_VERSION >= 18
		CurveAPI.GetConvertedTransformCurveData(NodeName, Translation[0], Translation[1], Translation[2], EulerRotation[0], EulerRotation[1], EulerRotation[2], Scale[0], Scale[1], Scale[2], DefaultTransform);
#if ENGINE_MINOR_VERSION < 20
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
#endif

#else
		CurveAPI.GetConvertedTransformCurveData(NodeName, Translation[0], Translation[1], Translation[2], EulerRotation[0], EulerRotation[1], EulerRotation[2], Scale[0], Scale[1], Scale[2]);

#endif
#if ENGINE_MINOR_VERSION < 20
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

#else
		ImportFBXTransform(nodename, section, CurveAPI);
#endif

#if ENGINE_MINOR_VERSION < 20
		section->SetStartTime(MinTime);
		section->SetEndTime(MaxTime);
#endif

		FbxImporter->ReleaseScene();
		ImportOptions->bConvertScene = bConverteScene;
		ImportOptions->bConvertSceneUnit = bConverteSceneUnit;
		ImportOptions->bForceFrontXAxis = bForceFrontXAxis;
		Py_RETURN_NONE;
	}

	FbxImporter->ReleaseScene();
	ImportOptions->bConvertScene = bConverteScene;
	ImportOptions->bConvertSceneUnit = bConverteSceneUnit;
	ImportOptions->bForceFrontXAxis = bForceFrontXAxis;
	return PyErr_Format(PyExc_Exception, "unable to find specified node in Fbx file");

}
#endif

