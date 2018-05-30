#pragma once


#include "GameFramework/HUD.h"
#include "UnrealEnginePython.h"
#include "PyHUD.generated.h"



UCLASS(BlueprintType, Blueprintable)
class UNREALENGINEPYTHON_API APyHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	APyHUD();
	~APyHUD();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere , Category = "Python")
	FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python")
	FString PythonClass;

	UPROPERTY(EditAnywhere, Category = "Python")
	bool PythonTickForceDisabled;

	UPROPERTY(EditAnywhere, Category = "Python")
	bool PythonDisableAutoBinding;

	UFUNCTION(BlueprintCallable, Category = "Python")
	void CallPythonHUDMethod(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	bool CallPythonHUDMethodBool(FString method_name, FString args);

	UFUNCTION(BlueprintCallable, Category = "Python")
	FString CallPythonHUDMethodString(FString method_name, FString args);

private:
	PyObject *py_hud_instance;
	// mapped uobject, required for debug and advanced reflection
	ue_PyUObject *py_uobject;
};

