#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "PyUserWidget.generated.h"



UCLASS(BlueprintType, Blueprintable)
class UPyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	~UPyUserWidget();

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Called every frame
	virtual void NativeTick(const FGeometry & MyGeometry, float InDeltaTime) override;

	//virtual void NativePaint(FPaintContext & InContext) const override;

	//virtual bool NativeIsInteractable() const override;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FString PythonClass;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool PythonTickForceDisabled;

private:
	PyObject *py_user_widget_instance;
	// mapped uobject, required for debug and advanced reflection
	ue_PyUObject *py_uobject;
};

