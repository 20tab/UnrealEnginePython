
#include "PyNativeWidgetHost.h"





UPyNativeWidgetHost::UPyNativeWidgetHost(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

#if WITH_EDITOR
const FText UPyNativeWidgetHost::GetPaletteCategory()
{
    return NSLOCTEXT("Python", "Python", "Python");
}
#endif