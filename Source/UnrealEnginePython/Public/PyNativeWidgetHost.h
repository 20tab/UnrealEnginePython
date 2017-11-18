// Copyright Kite & Lightning

#pragma once

#include "CoreMinimal.h"
#include "Components/NativeWidgetHost.h"
#include "PyNativeWidgetHost.generated.h"

/**
 * 
 */
UCLASS()
class UNREALENGINEPYTHON_API UPyNativeWidgetHost : public UNativeWidgetHost
{
    GENERATED_BODY()


    UPyNativeWidgetHost(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
        virtual const FText GetPaletteCategory() override;
#endif
	
};
