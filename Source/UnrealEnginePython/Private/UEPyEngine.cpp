#include "UnrealEnginePythonPrivatePCH.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

PyObject *py_unreal_engine_log(PyObject * self, PyObject * args) {
	char *message;
	if (!PyArg_ParseTuple(args, "s:log", &message)) {
		return NULL;
	}
	UE_LOG(LogPython, Log, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_log_warning(PyObject * self, PyObject * args) {
	char *message;
	if (!PyArg_ParseTuple(args, "s:log_warning", &message)) {
		return NULL;
	}
	UE_LOG(LogPython, Warning, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_log_error(PyObject * self, PyObject * args) {
	char *message;
	if (!PyArg_ParseTuple(args, "s:log_error", &message)) {
		return NULL;
	}
	UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_add_on_screen_debug_message(PyObject * self, PyObject * args) {
	int key;
	float time_to_display;
	char *message;
	if (!PyArg_ParseTuple(args, "ifs:add_on_screen_debug_message", &key, &time_to_display, &message)) {
		return NULL;
	}

	if (!GEngine)
		goto end;

	GEngine->AddOnScreenDebugMessage(key, time_to_display, FColor::Green, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

end:
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_print_string(PyObject * self, PyObject * args) {

	char *message;
	if (!PyArg_ParseTuple(args, "s:print_string", &message)) {
		return NULL;
	}

	if (!GEngine)
		goto end;

	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Cyan, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

end:
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_get_forward_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_forward_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetForwardVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}

PyObject *py_unreal_engine_get_right_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_right_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetRightVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}

PyObject *py_unreal_engine_get_up_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_up_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetUpVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}


PyObject *py_unreal_engine_find_class(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_class", &name)) {
		return NULL;
	}

	UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (u_class) {
		ue_PyUObject *ret = ue_get_python_wrapper(u_class);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_unreal_engine_color_to_linear(PyObject * self, PyObject * args) {
	uint8 r, g, b;
	uint8 a = 255;
	if (!PyArg_ParseTuple(args, "iii|i:color_to_linear", &r, &g, &b, &a)) {
		return NULL;
	}

	FLinearColor lcolor = FColor(r, g, b, a).ReinterpretAsLinear();
	return Py_BuildValue("fff", lcolor.R, lcolor.G, lcolor.B);
}

PyObject *py_unreal_engine_color_from_linear(PyObject * self, PyObject * args) {
	float r, g, b;
	float a = 1;
	if (!PyArg_ParseTuple(args, "fff|f:color_from_linear", &r, &g, &b, &a)) {
		return NULL;
	}

	FColor color = FLinearColor(r, g, b, a).ToFColor(true);
	return Py_BuildValue("iii", color.R, color.G, color.B);
}

PyObject *py_unreal_engine_find_object(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_object", &name)) {
		return NULL;
	}

	UObject *u_object = FindObject<UObject>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (u_object) {
		ue_PyUObject *ret = ue_get_python_wrapper(u_object);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_vector_add_vector(PyObject * self, PyObject *args) {

	float x = 0, y = 0, z = 0;

	Py_ssize_t items = PyTuple_Size(args);

	if ((items % 3) != 0) {
		return PyErr_Format(PyExc_TypeError, "this function requires a 3-multiple number of args");
	}

	for (int i = 0; i < (int)items; i += 3) {
		PyObject *x_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(x_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
		x += (float)PyFloat_AsDouble(x_py);
		PyObject *y_py = PyTuple_GetItem(args, i + 1);
		if (!PyFloat_Check(y_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (y)");
		y += (float)PyFloat_AsDouble(y_py);
		PyObject *z_py = PyTuple_GetItem(args, i + 2);
		if (!PyFloat_Check(z_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (z)");
		z += (float)PyFloat_AsDouble(z_py);
	}

	return Py_BuildValue("fff", x, y, z);
}

PyObject *py_unreal_engine_vector_add_float(PyObject * self, PyObject *args) {

	float x = 0, y = 0, z = 0;

	Py_ssize_t items = PyTuple_Size(args);

	if (items < 3) {
		return PyErr_Format(PyExc_TypeError, "this function requires a xyz vector");
	}

	PyObject *x_py = PyTuple_GetItem(args, 0);
	if (!PyFloat_Check(x_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	x = (float)PyFloat_AsDouble(x_py);


	PyObject *y_py = PyTuple_GetItem(args, 1);
	if (!PyFloat_Check(y_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	y = (float)PyFloat_AsDouble(y_py);

	PyObject *z_py = PyTuple_GetItem(args, 2);
	if (!PyFloat_Check(z_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	z = (float)PyFloat_AsDouble(z_py);

	for (int i = 3; i < (int)items; i++) {
		PyObject *delta_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(delta_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values");
		float delta = (float)PyFloat_AsDouble(delta_py);
		x += delta;
		y += delta;
		z += delta;
	}

	return Py_BuildValue("fff", x, y, z);
}

PyObject *py_unreal_engine_vector_mul_vector(PyObject * self, PyObject *args) {

	float x = 1, y = 1, z = 1;

	Py_ssize_t items = PyTuple_Size(args);

	if ((items % 3) != 0) {
		return PyErr_Format(PyExc_TypeError, "this function requires a 3-multiple number of args");
	}

	for (int i = 0; i < (int)items; i += 3) {
		PyObject *x_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(x_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
		x *= (float)PyFloat_AsDouble(x_py);
		PyObject *y_py = PyTuple_GetItem(args, i + 1);
		if (!PyFloat_Check(y_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (y)");
		y *= (float)PyFloat_AsDouble(y_py);
		PyObject *z_py = PyTuple_GetItem(args, i + 2);
		if (!PyFloat_Check(z_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (z)");
		z *= (float)PyFloat_AsDouble(z_py);
	}

	return Py_BuildValue("fff", x, y, z);
}

PyObject *py_unreal_engine_vector_mul_float(PyObject * self, PyObject *args) {

	float x = 0, y = 0, z = 0;

	Py_ssize_t items = PyTuple_Size(args);

	if (items < 3) {
		return PyErr_Format(PyExc_TypeError, "this function requires a xyz vector");
	}

	PyObject *x_py = PyTuple_GetItem(args, 0);
	if (!PyFloat_Check(x_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	x = (float)PyFloat_AsDouble(x_py);


	PyObject *y_py = PyTuple_GetItem(args, 1);
	if (!PyFloat_Check(y_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	y = (float)PyFloat_AsDouble(y_py);

	PyObject *z_py = PyTuple_GetItem(args, 2);
	if (!PyFloat_Check(z_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	z = (float)PyFloat_AsDouble(z_py);

	for (int i = 3; i < (int)items; i++) {
		PyObject *delta_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(delta_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values");
		float delta = (float)PyFloat_AsDouble(delta_py);
		x *= delta;
		y *= delta;
		z *= delta;
	}

	return Py_BuildValue("fff", x, y, z);
}
