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

	virtual void AddReferencedObjects(FReferenceCollector& InCollector) override;
	static FUnrealEnginePythonHouseKeeper *Get();
	int32 RunGC();
	bool IsValidPyUObject(ue_PyUObject *PyUObject);
	void TrackUObject(UObject *Object);
	void UntrackUObject(UObject *Object);
	void RegisterPyUObject(UObject *Object, ue_PyUObject *InPyUObject);
	void UnregisterPyUObject(UObject *Object);
	ue_PyUObject *GetPyUObject(UObject *Object);
	UPythonDelegate *FindDelegate(UObject *Owner, PyObject *PyCallable);
	UPythonDelegate *NewDelegate(UObject *Owner, PyObject *PyCallable, UFunction *Signature);
	TSharedRef<FPythonSlateDelegate> NewSlateDelegate(TSharedRef<SWidget> Owner, PyObject *PyCallable);
	TSharedRef<FPythonSlateDelegate> NewDeferredSlateDelegate(PyObject *PyCallable);
	TSharedRef<FPythonSmartDelegate> NewPythonSmartDelegate(PyObject *PyCallable);
	void TrackDeferredSlateDelegate(TSharedRef<FPythonSlateDelegate> Delegate, TSharedRef<SWidget> Owner);
	TSharedRef<FPythonSlateDelegate> NewStaticSlateDelegate(PyObject *PyCallable);

private:
	void RunGCDelegate();
	uint32 PyUObjectsGC();
	int32 DelegatesGC();

	TMap<UObject *, FPythonUOjectTracker> UObjectPyMapping;
	TArray<FPythonDelegateTracker> PyDelegatesTracker;

	TArray<FPythonSWidgetDelegateTracker> PySlateDelegatesTracker;
	TArray<TSharedRef<FPythonSlateDelegate>> PyStaticSlateDelegatesTracker;

	TArray<TSharedRef<FPythonSmartDelegate>> PyStaticSmartDelegatesTracker;

	TArray<UObject *> PythonTrackedObjects;
};
