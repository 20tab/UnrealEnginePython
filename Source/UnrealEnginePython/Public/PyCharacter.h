#pragma once

#include "GameFramework/Character.h"
#include "UnrealEnginePython.h"
#include "PyCharacter.generated.h"



UCLASS()
class UNREALENGINEPYTHON_API APyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	APyCharacter();
	~APyCharacter();

	// Called whenever the Actor is instantiated (before begin play)
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* input) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Python")
		FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python")
		FString PythonClass;

	UPROPERTY(EditAnywhere, Category = "Python")
		bool PythonTickForceDisabled;

	UPROPERTY(EditAnywhere, Category = "Python")
		bool PythonDisableAutoBinding;

	UFUNCTION(BlueprintCallable, Category = "Python")
		void CallPyCharacterMethod(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
		bool CallPyCharacterMethodBool(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
		FString CallPyCharacterMethodString(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
		float CallPyCharacterMethodFloat(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
		void SetPythonAttrInt(FString attr, int Integer);

	UFUNCTION(BlueprintCallable, Category = "Python")
		void SetPythonAttrFloat(FString attr, float Float);

	UFUNCTION(BlueprintCallable, Category = "Python")
		void SetPythonAttrString(FString attr, FString String);

	UFUNCTION(BlueprintCallable, Category = "Python")
		void SetPythonAttrBool(FString attr, bool Boolean);

	UFUNCTION(BlueprintCallable, Category = "Python")
		void SetPythonAttrVector(FString attr, FVector Vector);

	UFUNCTION(BlueprintCallable, Category = "Python")
		void SetPythonAttrRotator(FString attr, FRotator Rotator);

	UFUNCTION(BlueprintCallable, Category = "Python")
		void SetPythonAttrObject(FString attr, UObject *Object);

private:
	PyObject * py_character_instance;
	// mapped uobject, required for debug and advanced reflection
	ue_PyUObject *py_uobject;
};

