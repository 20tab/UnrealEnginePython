#include "UnrealEnginePythonPrivatePCH.h"
#include "Runtime/Engine/Classes/GameFramework/HUD.h"

PyObject *py_ue_hud_draw_2d_line(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int x1, y1, x2, y2;
	PyObject *py_fcolor;

	if (!PyArg_ParseTuple(args, "iiiiO:hud_draw_2d_line", &x1, &y1, &x2, &y2, &py_fcolor)) {
		return nullptr;
	}

	AHUD *hud = ue_py_check_type<AHUD>(self);
	if (!hud)
		return PyErr_Format(PyExc_Exception, "UObject is not a AHUD.");

	ue_PyFColor *fcolor = py_ue_is_fcolor(py_fcolor);
	if (!fcolor)
		return PyErr_Format(PyExc_Exception, "argument is not a FColor.");

	hud->Draw2DLine(x1, y1, x2, y2, fcolor->color);

	Py_RETURN_NONE;
}

PyObject *py_ue_hud_draw_line(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float x1, y1, x2, y2;
	PyObject *py_fcolor;
	float thickness;

	if (!PyArg_ParseTuple(args, "ffffOf:hud_draw_line", &x1, &y1, &x2, &y2, &py_fcolor, &thickness)) {
		return nullptr;
	}

	AHUD *hud = ue_py_check_type<AHUD>(self);
	if (!hud)
		return PyErr_Format(PyExc_Exception, "UObject is not a AHUD.");

	ue_PyFColor *fcolor = py_ue_is_fcolor(py_fcolor);
	if (!fcolor)
		return PyErr_Format(PyExc_Exception, "argument is not a FColor.");

	hud->DrawLine(x1, y1, x2, y2, fcolor->color, thickness);

	Py_RETURN_NONE;
}

PyObject *py_ue_hud_draw_rect(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float x, y, w, h;
	PyObject *py_fcolor;

	if (!PyArg_ParseTuple(args, "ffffO:hud_draw_line", &x, &y, &w, &h, &py_fcolor)) {
		return nullptr;
	}

	AHUD *hud = ue_py_check_type<AHUD>(self);
	if (!hud)
		return PyErr_Format(PyExc_Exception, "UObject is not a AHUD.");

	ue_PyFLinearColor *fcolor = py_ue_is_flinearcolor(py_fcolor);
	if (!fcolor)
		return PyErr_Format(PyExc_Exception, "argument is not a FColor.");

	hud->DrawRect(fcolor->color, x, y, w, h);

	Py_RETURN_NONE;
}


PyObject *py_ue_hud_draw_text(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *text;
	float x, y;
	PyObject *py_fcolor;
	float scale = 1;

	if (!PyArg_ParseTuple(args, "sffO|f:hud_draw_text", &text, &x, &y, &py_fcolor, &scale)) {
		return nullptr;
	}

	AHUD *hud = ue_py_check_type<AHUD>(self);
	if (!hud)
		return PyErr_Format(PyExc_Exception, "UObject is not a AHUD.");

	ue_PyFLinearColor *fcolor = py_ue_is_flinearcolor(py_fcolor);
	if (!fcolor)
		return PyErr_Format(PyExc_Exception, "argument is not a FColor.");

	hud->DrawText(FString(UTF8_TO_TCHAR(text)), fcolor->color, x, y, nullptr, scale);

	Py_RETURN_NONE;
}

PyObject *py_ue_hud_draw_texture(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_texture;
	float x, y, w, h;
	float u = 0, v = 0, uw = 1, vh = 1;
	PyObject *py_flinear_color = nullptr;
	int blend_mode = EBlendMode::BLEND_Translucent;
	float scale = 1;
	PyObject *py_scale_position = nullptr;
	float rotation = 0;
	float pivot_rot_x = 0, pivot_rot_y = 0;


	if (!PyArg_ParseTuple(args, "Offff|ffffOifOf(ff):hud_draw_texture", &py_texture, &x, &y, &w, &h, &u, &v, &uw, &vh, &py_flinear_color, &blend_mode, &scale, &py_scale_position, &rotation, &pivot_rot_x, &pivot_rot_y)) {
		return nullptr;
	}

	AHUD *hud = ue_py_check_type<AHUD>(self);
	if (!hud)
		return PyErr_Format(PyExc_Exception, "UObject is not a AHUD.");

	UTexture *texture = ue_py_check_type<UTexture>(py_texture);
	if (!texture)
		return PyErr_Format(PyExc_Exception, "argument is not a UTexture.");

	FLinearColor tint = FLinearColor::White;

	if (py_flinear_color) {
		ue_PyFLinearColor *flinear_color = py_ue_is_flinearcolor(py_flinear_color);
		if (!flinear_color)
			return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor.");
		tint = flinear_color->color;
	}

	bool scale_position = false;
	if (py_scale_position && PyObject_IsTrue(py_scale_position))
		scale_position = true;

	hud->DrawTexture(texture, x, y, w, h, u, v, uw, vh, tint, (EBlendMode)blend_mode, scale, scale_position, rotation, FVector2D(pivot_rot_x, pivot_rot_y));

	Py_RETURN_NONE;
}