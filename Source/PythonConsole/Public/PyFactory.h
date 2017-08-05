#pragma once

#include "UnrealEd.h"
#include "PyFactory.generated.h"

UCLASS()
class UPyFactory : public UFactory
{
	GENERATED_BODY()

public:

	virtual UObject* FactoryCreateFile(UClass * Class, UObject *InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext *Warn, bool& bOutOperationCanceled) override {
		return PyFactoryCreateFile(Class, InParent, InName.ToString(), Filename);
	}

	virtual UObject* FactoryCreateNew(UClass * Class, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context, FFeedbackContext *Warn) override {
		return PyFactoryCreateNew(Class, InParent, InName.ToString());
	}

	virtual UClass* ResolveSupportedClass() override {
		if (SupportedClass)
			return SupportedClass;
		return PyResolveSupportedClass();
	}

	virtual FText GetDisplayName() const override {
		if (!PyDisplayName.IsEmpty()) {
			return FText::FromString(PyDisplayName);
		}
		return FText::FromString(GetClass()->GetName().Replace(UTF8_TO_TCHAR("Factory"), UTF8_TO_TCHAR("")));
	}

	virtual FText GetToolTip() const override {
		if (!PyToolTip.IsEmpty()) {
			return FText::FromString(PyToolTip);
		}
		return GetDisplayName();
	}

	UFUNCTION(BlueprintImplementableEvent)
	UObject* PyFactoryCreateFile(class UClass * Class, UObject *InParent, const FString & InName, const FString & Filename);

	UFUNCTION(BlueprintImplementableEvent)
	UObject* PyFactoryCreateNew(class UClass * Class, UObject *InParent, const FString & InName);

	UFUNCTION(BlueprintImplementableEvent)
	UClass* PyResolveSupportedClass();

	UPROPERTY(BlueprintReadWrite, Category = "PyFactory")
	FString PyDisplayName;

	UPROPERTY(BlueprintReadWrite, Category = "PyFactory")
	FString PyToolTip;

};


