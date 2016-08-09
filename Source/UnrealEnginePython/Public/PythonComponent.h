#pragma once

#include "Components/ActorComponent.h"

#include "PythonComponent.generated.h"



UCLASS(ClassGroup = (Python), meta = (BlueprintSpawnableComponent))
class UPythonComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPythonComponent();
	~UPythonComponent();

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

	UFUNCTION(BlueprintCallable, Category = "Python")
	void CallPythonComponentMethod(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	bool CallPythonComponentMethodBool(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	FString CallPythonComponentMethodString(FString method_name, FString args);

private:
	PyObject *py_component_instance;
};

