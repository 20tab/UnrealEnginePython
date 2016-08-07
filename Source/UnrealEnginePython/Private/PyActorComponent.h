#pragma once

#include "Components/ActorComponent.h"

#include "PyActorComponent.generated.h"



UCLASS(ClassGroup = (Python), meta = (BlueprintSpawnableComponent))
class UPyActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPyActorComponent();
	~UPyActorComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, meta = (Multiline = true), Category = "Python")
	FString PythonCode;

	UPROPERTY(EditAnywhere , Category = "Python")
	FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python")
	FString PythonClass;



private:
	PyObject *py_component_instance;
};

