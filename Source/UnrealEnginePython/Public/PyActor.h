#pragma once


#include "GameFramework/Actor.h"

#include "PyActor.generated.h"



UCLASS(BlueprintType, Blueprintable)
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

	UFUNCTION(BlueprintCallable, Category = "Python")
	void CallPythonActorMethod(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	bool CallPythonActorMethodBool(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	FString CallPythonActorMethodString(FString method_name, FString args);

private:
	PyObject *py_actor_instance;

	UFUNCTION()
	void PyOnActorBeginOverlap(AActor *overlapped, AActor *other);

	UFUNCTION()
	void PyOnActorEndOverlap(AActor *overlapped, AActor *other);

	UFUNCTION()
	void PyOnActorHit(AActor *self_actor, AActor *other, FVector normal_impulse, const FHitResult &hit);

	UFUNCTION()
	void PyOnActorClicked(AActor *touched_actor, FKey button_pressed);
};

