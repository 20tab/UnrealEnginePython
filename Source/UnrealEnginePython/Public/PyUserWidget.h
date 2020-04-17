#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UnrealEnginePython.h"
#include "Runtime/Launch/Resources/Version.h"
#include "PyUserWidget.generated.h"

#if ENGINE_MINOR_VERSION < 20
#define NativePaintArgs FPaintContext & InContext
#define NativePaintRetValue void
#else
#define NativePaintArgs const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled
#define NativePaintRetValue int32
#endif

UCLASS(BlueprintType, Blueprintable)
class UNREALENGINEPYTHON_API UPyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPyUserWidget(const FObjectInitializer& ObjectInitializer);
	~UPyUserWidget();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Called every frame
	virtual void NativeTick(const FGeometry & MyGeometry, float InDeltaTime) override;

	virtual NativePaintRetValue NativePaint(NativePaintArgs) const override;

	virtual bool NativeIsInteractable() const override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) override;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		FString PythonModule;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		FString PythonClass;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		bool PythonTickForceDisabled;

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		bool PythonPaintForceDisabled;

	UFUNCTION(BlueprintCallable, Category = "Python")
		void CallPythonUserWidgetMethod(FString method_name, FString args);

	UPROPERTY(EditAnywhere, Category = "Python", BlueprintReadWrite)
		TWeakObjectPtr<class UPyNativeWidgetHost> PyNativeWidgetHost;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	void SetSlateWidget(TSharedRef<SWidget> InContent);
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

private:
	PyObject * py_user_widget_instance;
	// mapped uobject, required for debug and advanced reflection
	ue_PyUObject *py_uobject;
};

