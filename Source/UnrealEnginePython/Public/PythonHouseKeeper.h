#pragma once

#include "UnrealEnginePython.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/WeakObjectPtr.h"
#include "Widgets/SWidget.h"
#include "Slate/UEPySlateDelegate.h"
#include "Runtime/CoreUObject/Public/UObject/GCObject.h"
#include "PythonDelegate.h"
#include "PythonSmartDelegate.h"

class FUnrealEnginePythonHouseKeeper : public FGCObject
{

	struct FPythonUOjectTracker
	{
		FWeakObjectPtr Owner;
		ue_PyUObject *PyUObject;
		bool bPythonOwned;

		FPythonUOjectTracker(UObject *Object, ue_PyUObject *InPyUObject)
		{
			Owner = FWeakObjectPtr(Object);
			PyUObject = InPyUObject;
			bPythonOwned = false;
		}
	};

	struct FPythonDelegateTracker
	{
		FWeakObjectPtr Owner;
		UPythonDelegate *Delegate;

		FPythonDelegateTracker(UPythonDelegate *DelegateToTrack, UObject *DelegateOwner) : Owner(DelegateOwner), Delegate(DelegateToTrack)
		{
		}

		~FPythonDelegateTracker()
		{
		}
	};


	struct FPythonSWidgetDelegateTracker
	{
		TWeakPtr<SWidget> Owner;
		TSharedPtr<FPythonSlateDelegate> Delegate;

		FPythonSWidgetDelegateTracker(TSharedRef<FPythonSlateDelegate> DelegateToTrack, TSharedRef<SWidget> DelegateOwner) : Owner(DelegateOwner), Delegate(DelegateToTrack)
		{
		}

		~FPythonSWidgetDelegateTracker()
		{
		}
	};

public:

	virtual void AddReferencedObjects(FReferenceCollector& InCollector) override
	{
		InCollector.AddReferencedObjects(PythonTrackedObjects);
	}

	static FUnrealEnginePythonHouseKeeper *Get()
	{
		static FUnrealEnginePythonHouseKeeper *Singleton;
		if (!Singleton)
		{
			Singleton = new FUnrealEnginePythonHouseKeeper();
			// register a new delegate for the GC
#if ENGINE_MINOR_VERSION >= 18
			FCoreUObjectDelegates::GetPostGarbageCollect().AddRaw(Singleton, &FUnrealEnginePythonHouseKeeper::RunGCDelegate);
#else
			FCoreUObjectDelegates::PostGarbageCollect.AddRaw(Singleton, &FUnrealEnginePythonHouseKeeper::RunGCDelegate);
#endif
		}
		return Singleton;
	}

	void RunGCDelegate()
	{
		FScopePythonGIL gil;
		RunGC();
	}

	int32 RunGC()
	{
		int32 Garbaged = PyUObjectsGC();
		Garbaged += DelegatesGC();
		return Garbaged;
	}

	bool IsValidPyUObject(ue_PyUObject *PyUObject)
	{
		if (!PyUObject)
			return false;

		UObject *Object = PyUObject->ue_object;
		FPythonUOjectTracker *Tracker = UObjectPyMapping.Find(Object);
		if (!Tracker)
		{
			return false;
		}

		if (!Tracker->Owner.IsValid())
			return false;

		return true;

	}

	void TrackUObject(UObject *Object)
	{
		FPythonUOjectTracker *Tracker = UObjectPyMapping.Find(Object);
		if (!Tracker)
		{
			return;
		}
		if (Tracker->bPythonOwned)
			return;
		Tracker->bPythonOwned = true;
		// when a new ue_PyUObject spawns, it has a reference counting of two
		Py_DECREF(Tracker->PyUObject);
		Tracker->PyUObject->owned = 1;
		PythonTrackedObjects.Add(Object);
	}

	void UntrackUObject(UObject *Object)
	{
		PythonTrackedObjects.Remove(Object);
	}

	void RegisterPyUObject(UObject *Object, ue_PyUObject *InPyUObject)
	{
		UObjectPyMapping.Add(Object, FPythonUOjectTracker(Object, InPyUObject));
	}

	void UnregisterPyUObject(UObject *Object)
	{
		UObjectPyMapping.Remove(Object);
	}

	ue_PyUObject *GetPyUObject(UObject *Object)
	{
		FPythonUOjectTracker *Tracker = UObjectPyMapping.Find(Object);
		if (!Tracker)
		{
			return nullptr;
		}

		if (!Tracker->Owner.IsValid(true))
		{
#if defined(UEPY_MEMORY_DEBUG)
			UE_LOG(LogPython, Warning, TEXT("DEFREF'ing UObject at %p (refcnt: %d)"), Object, Tracker->PyUObject->ob_base.ob_refcnt);
#endif
			if (!Tracker->bPythonOwned)
				Py_DECREF((PyObject *)Tracker->PyUObject);
			UnregisterPyUObject(Object);
			return nullptr;
	}

		return Tracker->PyUObject;
}

	uint32 PyUObjectsGC()
	{
		uint32 Garbaged = 0;
		TArray<UObject *> BrokenList;
		for (auto &UObjectPyItem : UObjectPyMapping)
		{
			UObject *Object = UObjectPyItem.Key;
			FPythonUOjectTracker &Tracker = UObjectPyItem.Value;
#if defined(UEPY_MEMORY_DEBUG)
			UE_LOG(LogPython, Warning, TEXT("Checking for UObject at %p"), Object);
#endif
			if (!Tracker.Owner.IsValid(true))
			{
#if defined(UEPY_MEMORY_DEBUG)
				UE_LOG(LogPython, Warning, TEXT("Removing UObject at %p (refcnt: %d)"), Object, Tracker.PyUObject->ob_base.ob_refcnt);
#endif
				BrokenList.Add(Object);
				Garbaged++;
		}
			else
			{
#if defined(UEPY_MEMORY_DEBUG)
				UE_LOG(LogPython, Error, TEXT("UObject at %p %s is in use"), Object, *Object->GetName());
#endif
	}
		}

		for (UObject *Object : BrokenList)
		{
			FPythonUOjectTracker &Tracker = UObjectPyMapping[Object];
			if (!Tracker.bPythonOwned)
				Py_DECREF((PyObject *)Tracker.PyUObject);
			UnregisterPyUObject(Object);
		}

		return Garbaged;

	}


	int32 DelegatesGC()
	{
		int32 Garbaged = 0;
#if defined(UEPY_MEMORY_DEBUG)
		UE_LOG(LogPython, Display, TEXT("Garbage collecting %d UObject delegates"), PyDelegatesTracker.Num());
#endif
		for (int32 i = PyDelegatesTracker.Num() - 1; i >= 0; --i)
		{
			FPythonDelegateTracker &Tracker = PyDelegatesTracker[i];
			if (!Tracker.Owner.IsValid(true))
			{
				Tracker.Delegate->RemoveFromRoot();
				PyDelegatesTracker.RemoveAt(i);
				Garbaged++;
			}

		}

#if defined(UEPY_MEMORY_DEBUG)
		UE_LOG(LogPython, Display, TEXT("Garbage collecting %d Slate delegates"), PySlateDelegatesTracker.Num());
#endif

		for (int32 i = PySlateDelegatesTracker.Num() - 1; i >= 0; --i)
		{
			FPythonSWidgetDelegateTracker &Tracker = PySlateDelegatesTracker[i];
			if (!Tracker.Owner.IsValid())
			{
				PySlateDelegatesTracker.RemoveAt(i);
				Garbaged++;
			}

		}
		return Garbaged;
		}

	UPythonDelegate *NewDelegate(UObject *Owner, PyObject *PyCallable, UFunction *Signature)
	{
		UPythonDelegate *Delegate = NewObject<UPythonDelegate>();

		Delegate->AddToRoot();
		Delegate->SetPyCallable(PyCallable);
		Delegate->SetSignature(Signature);

		FPythonDelegateTracker Tracker(Delegate, Owner);
		PyDelegatesTracker.Add(Tracker);

		return Delegate;
	}

	TSharedRef<FPythonSlateDelegate> NewSlateDelegate(TSharedRef<SWidget> Owner, PyObject *PyCallable)
	{
		TSharedRef<FPythonSlateDelegate> Delegate = MakeShareable(new FPythonSlateDelegate());
		Delegate->SetPyCallable(PyCallable);

		FPythonSWidgetDelegateTracker Tracker(Delegate, Owner);
		PySlateDelegatesTracker.Add(Tracker);

		return Delegate;
	}

	TSharedRef<FPythonSlateDelegate> NewDeferredSlateDelegate(PyObject *PyCallable)
	{
		TSharedRef<FPythonSlateDelegate> Delegate = MakeShareable(new FPythonSlateDelegate());
		Delegate->SetPyCallable(PyCallable);

		return Delegate;
	}

	TSharedRef<FPythonSmartDelegate> NewPythonSmartDelegate(PyObject *PyCallable)
	{
		TSharedRef<FPythonSmartDelegate> Delegate = MakeShareable(new FPythonSmartDelegate());
		Delegate->SetPyCallable(PyCallable);

		PyStaticSmartDelegatesTracker.Add(Delegate);

		return Delegate;
	}

	void TrackDeferredSlateDelegate(TSharedRef<FPythonSlateDelegate> Delegate, TSharedRef<SWidget> Owner)
	{
		FPythonSWidgetDelegateTracker Tracker(Delegate, Owner);
		PySlateDelegatesTracker.Add(Tracker);
	}

	TSharedRef<FPythonSlateDelegate> NewStaticSlateDelegate(PyObject *PyCallable)
	{
		TSharedRef<FPythonSlateDelegate> Delegate = MakeShareable(new FPythonSlateDelegate());
		Delegate->SetPyCallable(PyCallable);

		PyStaticSlateDelegatesTracker.Add(Delegate);

		return Delegate;
	}

private:
	TMap<UObject *, FPythonUOjectTracker> UObjectPyMapping;
	TArray<FPythonDelegateTracker> PyDelegatesTracker;

	TArray<FPythonSWidgetDelegateTracker> PySlateDelegatesTracker;
	TArray<TSharedRef<FPythonSlateDelegate>> PyStaticSlateDelegatesTracker;

	TArray<TSharedRef<FPythonSmartDelegate>> PyStaticSmartDelegatesTracker;

	TArray<UObject *> PythonTrackedObjects;
};
