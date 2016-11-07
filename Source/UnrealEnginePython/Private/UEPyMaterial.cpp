#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

PyObject *py_ue_set_scalar_parameter( ue_PyUObject *self , PyObject * args ) {

    ue_py_check( self );

    if ( !self->ue_object->IsA<UMaterialInstanceConstant>() ) {
        return PyErr_Format( PyExc_Exception, "uobject is not a UMaterialInstanceConstant" );
    }

    char *scalarName = nullptr;
    float scalarValue = 0;
    if ( !PyArg_ParseTuple( args, "sf:set_scalar_parameter", &scalarName, &scalarValue ) ) {
        return NULL;
    }

    UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;

    FName parameterName(UTF8_TO_TCHAR( scalarName ));
    material_instance->SetScalarParameterValueEditorOnly( parameterName, scalarValue );

    Py_INCREF( Py_None );
    return Py_None;

}

PyObject *py_ue_set_vector_parameter( ue_PyUObject *self, PyObject * args ) {
    ue_py_check( self );

    if ( !self->ue_object->IsA<UMaterialInstanceConstant>() ) {
        return PyErr_Format( PyExc_Exception, "uobject is not a UMaterialInstanceConstant" );
    }

    char *vectorName;
    PyObject *vectorValue = nullptr;
    if ( !PyArg_ParseTuple( args, "sO:set_vector_parameter", &vectorName, &vectorValue ) ) {
        return NULL;
    }

    FVector vectorParameter( 0, 0, 0 );

    if ( vectorValue ) {
        ue_PyFVector *py_vector = py_ue_is_fvector( vectorValue );
        if ( !py_vector )
            return PyErr_Format( PyExc_Exception, "argument must be an FVector" );
        vectorParameter = py_vector->vec;
    }

    UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;

    FName parameterName( UTF8_TO_TCHAR( vectorName ) );
    material_instance->SetVectorParameterValueEditorOnly( parameterName, FLinearColor( vectorParameter) );

    Py_INCREF( Py_None );
    return Py_None;


}
PyObject *py_ue_set_texture_parameter( ue_PyUObject *self, PyObject * args ) {
    ue_py_check( self );

    if ( !self->ue_object->IsA<UMaterialInstanceConstant>() ) {
        return PyErr_Format( PyExc_Exception, "uobject is not a UMaterialInstanceConstant" );
    }

    char *textureName;
    PyObject *textureObject = nullptr;
    if ( !PyArg_ParseTuple( args, "sO:set_texture_parameter", &textureName, &textureObject ) ) {
        return NULL;
    }

    if ( !ue_is_pyuobject( textureObject ) ) {
        return PyErr_Format( PyExc_Exception, "argument is not a UObject" );
    }

    ue_PyUObject *py_obj = (ue_PyUObject *)textureObject;
    if ( !py_obj->ue_object->IsA<UTexture>() )
        return PyErr_Format( PyExc_Exception, "uobject is not a UTexture" );

    UTexture *ue_texture = (UTexture *)py_obj->ue_object;

    UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;

    FName parameterName( UTF8_TO_TCHAR( textureName ) );
    material_instance->SetTextureParameterValueEditorOnly( parameterName, ue_texture );

    Py_INCREF( Py_None );
    return Py_None;
}

PyObject *py_ue_set_parent( ue_PyUObject *self, PyObject * args ) {

    ue_py_check( self );

    if ( !self->ue_object->IsA<UMaterialInstanceConstant>() ) {
        return PyErr_Format( PyExc_Exception, "uobject is not a UMaterialInstanceConstant" );
    }

    PyObject *py_material = nullptr;

    if ( !PyArg_ParseTuple( args, "O:set_parent", &py_material ) ) {
        return NULL;
    }

    if ( !ue_is_pyuobject( py_material ) ) {
        return PyErr_Format( PyExc_Exception, "argument is not a UObject" );
    }

    ue_PyUObject *py_obj = (ue_PyUObject *)py_material;
    if ( !py_obj->ue_object->IsA<UMaterialInterface>() )
        return PyErr_Format( PyExc_Exception, "uobject is not a UMaterialInterface" );


    UMaterialInterface *materialInterface = (UMaterialInterface *)py_obj->ue_object;
    UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
    material_instance->SetParentEditorOnly( materialInterface );
    material_instance->PostEditChange();

    Py_INCREF( Py_None );
    return Py_None;

}

#endif
