
#include "PythonHouseKeeper.h"

void FUnrealEnginePythonHouseKeeper::AddReferencedObjects(FReferenceCollector& InCollector)
{
    InCollector.AddReferencedObjects(PythonTrackedObjects);
}

FUnrealEnginePythonHouseKeeper *FUnrealEnginePythonHouseKeeper::Get()
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

void FUnrealEnginePythonHouseKeeper::RunGCDelegate()
{
    FScopePythonGIL gil;
    RunGC();
}

int32 FUnrealEnginePythonHouseKeeper::RunGC()
{
    int32 Garbaged = PyUObjectsGC();
    Garbaged += DelegatesGC();
    return Garbaged;
}

bool FUnrealEnginePythonHouseKeeper::IsValidPyUObject(ue_PyUObject *PyUObject)
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

void FUnrealEnginePythonHouseKeeper::TrackUObject(UObject *Object)
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

void FUnrealEnginePythonHouseKeeper::UntrackUObject(UObject *Object)
{
    PythonTrackedObjects.Remove(Object);
}

void FUnrealEnginePythonHouseKeeper::RegisterPyUObject(UObject *Object, ue_PyUObject *InPyUObject)
{
    UObjectPyMapping.Add(Object, FPythonUOjectTracker(Object, InPyUObject));
}

void FUnrealEnginePythonHouseKeeper::UnregisterPyUObject(UObject *Object)
{
    UObjectPyMapping.Remove(Object);
}

ue_PyUObject *FUnrealEnginePythonHouseKeeper::GetPyUObject(UObject *Object)
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

uint32 FUnrealEnginePythonHouseKeeper::PyUObjectsGC()
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


int32 FUnrealEnginePythonHouseKeeper::DelegatesGC()
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

UPythonDelegate *FUnrealEnginePythonHouseKeeper::FindDelegate(UObject *Owner, PyObject *PyCallable)
{
    for (int32 i = PyDelegatesTracker.Num() - 1; i >= 0; --i)
    {
        FPythonDelegateTracker &Tracker = PyDelegatesTracker[i];
        if (Tracker.Owner.Get() == Owner && Tracker.Delegate->UsesPyCallable(PyCallable))
            return Tracker.Delegate;
    }
    return nullptr;
}

UPythonDelegate *FUnrealEnginePythonHouseKeeper::NewDelegate(UObject *Owner, PyObject *PyCallable, UFunction *Signature)
{
    UPythonDelegate *Delegate = NewObject<UPythonDelegate>();

    Delegate->AddToRoot();
    Delegate->SetPyCallable(PyCallable);
    Delegate->SetSignature(Signature);

    FPythonDelegateTracker Tracker(Delegate, Owner);
    PyDelegatesTracker.Add(Tracker);

    return Delegate;
}

TSharedRef<FPythonSlateDelegate> FUnrealEnginePythonHouseKeeper::NewSlateDelegate(TSharedRef<SWidget> Owner, PyObject *PyCallable)
{
    TSharedRef<FPythonSlateDelegate> Delegate = MakeShareable(new FPythonSlateDelegate());
    Delegate->SetPyCallable(PyCallable);

    FPythonSWidgetDelegateTracker Tracker(Delegate, Owner);
    PySlateDelegatesTracker.Add(Tracker);

    return Delegate;
}

TSharedRef<FPythonSlateDelegate> FUnrealEnginePythonHouseKeeper::NewDeferredSlateDelegate(PyObject *PyCallable)
{
    TSharedRef<FPythonSlateDelegate> Delegate = MakeShareable(new FPythonSlateDelegate());
    Delegate->SetPyCallable(PyCallable);

    return Delegate;
}

TSharedRef<FPythonSmartDelegate> FUnrealEnginePythonHouseKeeper::NewPythonSmartDelegate(PyObject *PyCallable)
{
    TSharedRef<FPythonSmartDelegate> Delegate = MakeShareable(new FPythonSmartDelegate());
    Delegate->SetPyCallable(PyCallable);

    PyStaticSmartDelegatesTracker.Add(Delegate);

    return Delegate;
}

void FUnrealEnginePythonHouseKeeper::TrackDeferredSlateDelegate(TSharedRef<FPythonSlateDelegate> Delegate, TSharedRef<SWidget> Owner)
{
    FPythonSWidgetDelegateTracker Tracker(Delegate, Owner);
    PySlateDelegatesTracker.Add(Tracker);
}

TSharedRef<FPythonSlateDelegate> FUnrealEnginePythonHouseKeeper::NewStaticSlateDelegate(PyObject *PyCallable)
{
    TSharedRef<FPythonSlateDelegate> Delegate = MakeShareable(new FPythonSlateDelegate());
    Delegate->SetPyCallable(PyCallable);

    PyStaticSlateDelegatesTracker.Add(Delegate);

    return Delegate;
}

