#pragma once


#include "GameFramework/Actor.h"
#include "UnrealEnginePython.h"
#include "PyActor.generated.h"



UCLASS(BlueprintType, Blueprintable)
class UNREALENGINEPYTHON_API APyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	APyActor();
	~APyActor();

	// Called whenever the Actor is instantiated (before begin play)
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FString PythonClass;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool PythonTickForceDisabled;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool PythonDisableAutoBinding;

	UFUNCTION(BlueprintCallable, Category = "Python")
	void CallPythonActorMethod(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	bool CallPythonActorMethodBool(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	FString CallPythonActorMethodString(FString method_name, FString args);

private:
	PyObject *py_actor_instance;
	// mapped uobject, required for debug and advanced reflection
	ue_PyUObject *py_uobject;
};

