#if ENGINE_MINOR_VERSION < 25
//UE4.25 refactored UProperties to no longer be UObjects
//For compatibility with older engines, added a typedef here rather than polluting code throughout
//Decision to put these definitions for older engine compatibility rather than newer engine compatiblity
//    was made with the view that FProperty is the current and future, and therefore should be the standard in code
typedef UProperty FProperty;
typedef UObjectProperty FObjectProperty;
typedef UStructProperty FStructProperty;
typedef UMulticastDelegateProperty FMulticastDelegateProperty;
typedef UArrayProperty FArrayProperty;
typedef UMapProperty FMapProperty;
typedef UDelegateProperty FDelegateProperty;
typedef UObjectProperty FObjectProperty;
typedef UClassProperty FClassProperty;
typedef UBoolProperty FBoolProperty;
typedef UIntProperty FIntProperty;

typedef UUInt32Property FUInt32Property;
typedef UInt64Property FInt64Property;
typedef UUInt64Property FUInt64Property;
typedef UFloatProperty FFloatProperty;
typedef UByteProperty FByteProperty;
typedef UEnumProperty FEnumProperty;
typedef UStrProperty FStrProperty;
typedef UTextProperty FTextProperty;
typedef UNameProperty FNameProperty;
typedef UObjectPropertyBase FObjectPropertyBase;
typedef UClassProperty FClassProperty;
typedef UStructProperty FStructProperty;
typedef UWeakObjectProperty FWeakObjectProperty;

typedef USoftObjectProperty FSoftObjectProperty;
typedef USoftClassProperty FSoftClassProperty;

typedef UNumericProperty FNumericProperty;
typedef UInterfaceProperty FInterfaceProperty;
#endif