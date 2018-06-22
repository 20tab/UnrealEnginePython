#pragma once


#include "GameFramework/Pawn.h"
#include "UnrealEnginePython.h"
#include "PyPawn.generated.h"



UCLASS(BlueprintType, Blueprintable)
class UNREALENGINEPYTHON_API APyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	APyPawn();
	~APyPawn();

	// Called whenever the Actor is instantiated (before begin play)
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere , Category = "Python")
	FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python")
	FString PythonClass;

	UPROPERTY(EditAnywhere, Category = "Python")
	bool PythonTickForceDisabled;

	UPROPERTY(EditAnywhere, Category = "Python")
	bool PythonDisableAutoBinding;

	UFUNCTION(BlueprintCallable, Category = "Python")
	void CallPythonPawnMethod(FString method_name);

	UFUNCTION(BlueprintCallable, Category = "Python")
	bool CallPythonPawnMethodBool(FString method_name);

	UFUNCTION(BlueprintCallable, Category = "Python")
	FString CallPythonPawnMethodString(FString method_name);

private:
	PyObject *py_pawn_instance;
	// mapped uobject, required for debug and advanced reflection
	ue_PyUObject *py_uobject;
};

