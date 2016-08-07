#pragma once


#include "GameFramework/Actor.h"

#include "PyActor.generated.h"



UCLASS(BlueprintType, Blueprintable, Config = Engine, ClassGroup = (Python))
class APyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	APyActor();
	~APyActor();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, meta = (Multiline = true), Category = "Python")
	FString PythonCode;

	UPROPERTY(EditAnywhere , Category = "Python")
	FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python")
	FString PythonClass;

private:
	PyObject *py_actor_instance;
};

