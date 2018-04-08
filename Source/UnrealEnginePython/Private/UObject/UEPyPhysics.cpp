#include "UEPyPhysics.h"

#if ENGINE_MINOR_VERSION >= 18
#include "Runtime/Engine/Public/DestructibleInterface.h"
#else
#include "Components/DestructibleComponent.h"
#endif

#include "Components/PrimitiveComponent.h"


PyObject *py_ue_set_simulate_physics(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:set_simulate_physics", &is_true))
	{
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	if (!primitive)
	{
		return PyErr_Format(PyExc_Exception, "unable to set physics for the object");
	}

	primitive->SetSimulatePhysics(enabled);

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_ue_add_impulse(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_impulse = nullptr;
	char *bone_name = nullptr;
	PyObject *py_obj_b_vel_change = nullptr;
	if (!PyArg_ParseTuple(args, "O|sO:add_impulse", &py_obj_impulse, &bone_name, &py_obj_b_vel_change))
	{
		return nullptr;
	}

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	FVector impulse = FVector(0, 0, 0);
	if (py_obj_impulse)
	{
		ue_PyFVector *py_impulse = py_ue_is_fvector(py_obj_impulse);
		if (!py_impulse)
			return PyErr_Format(PyExc_Exception, "impulse must be a FVector");
		impulse = py_impulse->vec;
	}

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}

	bool b_vel_change = false;
	if (py_obj_b_vel_change && PyObject_IsTrue(py_obj_b_vel_change))
		b_vel_change = true;

	primitive->AddImpulse(impulse, f_bone_name, b_vel_change);

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_ue_add_angular_impulse(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_impulse = nullptr;
	char *bone_name = nullptr;
	PyObject *py_obj_b_vel_change = nullptr;
	if (!PyArg_ParseTuple(args, "O|sO:add_angular_impulse", &py_obj_impulse, &bone_name, &py_obj_b_vel_change))
	{
		return nullptr;
	}

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	FVector impulse = FVector(0, 0, 0);
	if (py_obj_impulse)
	{
		ue_PyFVector *py_impulse = py_ue_is_fvector(py_obj_impulse);
		if (!py_impulse)
			return PyErr_Format(PyExc_Exception, "impulse must be a FVector");
		impulse = py_impulse->vec;
	}

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}

	bool b_vel_change = false;
	if (py_obj_b_vel_change && PyObject_IsTrue(py_obj_b_vel_change))
		b_vel_change = true;

#if ENGINE_MINOR_VERSION >= 18
	primitive->AddAngularImpulseInRadians(impulse, f_bone_name, b_vel_change);
#else
	primitive->AddAngularImpulse(impulse, f_bone_name, b_vel_change);
#endif

	Py_RETURN_NONE;
}


PyObject *py_ue_add_force(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_force = nullptr;
	char *bone_name = nullptr;
	PyObject *py_obj_b_accel_change = nullptr;
	if (!PyArg_ParseTuple(args, "O|sO:add_force", &py_obj_force, &bone_name, &py_obj_b_accel_change))
	{
		return nullptr;
	}

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	FVector force = FVector(0, 0, 0);
	if (py_obj_force)
	{
		ue_PyFVector *py_force = py_ue_is_fvector(py_obj_force);
		if (!py_force)
			return PyErr_Format(PyExc_Exception, "force must be a FVector");
		force = py_force->vec;
	}

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}

	bool b_accel_change = false;
	if (py_obj_b_accel_change && PyObject_IsTrue(py_obj_b_accel_change))
		b_accel_change = true;

	primitive->AddForce(force, f_bone_name, b_accel_change);

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_ue_add_torque(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_torque = nullptr;
	char *bone_name = nullptr;
	PyObject *py_obj_b_accel_change = nullptr;
	if (!PyArg_ParseTuple(args, "O|sO:add_torque", &py_obj_torque, &bone_name, &py_obj_b_accel_change))
	{
		return nullptr;
	}

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	FVector torque = FVector(0, 0, 0);
	if (py_obj_torque)
	{
		ue_PyFVector *py_torque = py_ue_is_fvector(py_obj_torque);
		if (!py_torque)
			return PyErr_Format(PyExc_Exception, "torque must be a FVector");
		torque = py_torque->vec;
	}

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}

	bool b_accel_change = false;
	if (py_obj_b_accel_change && PyObject_IsTrue(py_obj_b_accel_change))
		b_accel_change = true;

#if ENGINE_MINOR_VERSION >= 18
	primitive->AddTorqueInRadians(torque, f_bone_name, b_accel_change);
#else
	primitive->AddTorque(torque, f_bone_name, b_accel_change);
#endif

	Py_RETURN_NONE;
}


PyObject *py_ue_set_physics_linear_velocity(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	PyObject *py_obj_new_vel = nullptr;
	PyObject *is_add_to_current = NULL;
	char *bone_name = nullptr;
	if (!PyArg_ParseTuple(args, "O|Os:set_physics_linear_velocity", &py_obj_new_vel, &is_add_to_current, &bone_name))
	{
		return nullptr;
	}

	FVector new_vel = FVector(0, 0, 0);
	if (py_obj_new_vel)
	{
		ue_PyFVector *py_new_vel = py_ue_is_fvector(py_obj_new_vel);
		if (!py_new_vel)
			return PyErr_Format(PyExc_Exception, "torque must be a FVector");
		new_vel = py_new_vel->vec;
	}

	bool add_to_current = false;
	if (is_add_to_current && PyObject_IsTrue(is_add_to_current))
		add_to_current = true;

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}

	primitive->SetPhysicsLinearVelocity(new_vel, add_to_current, f_bone_name);

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_ue_get_physics_linear_velocity(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	char *bone_name = nullptr;
	if (!PyArg_ParseTuple(args, "|s:get_physics_linear_velocity", &bone_name))
	{
		return nullptr;
	}

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}

	return py_ue_new_fvector(primitive->GetPhysicsLinearVelocity(f_bone_name));
}


PyObject *py_ue_set_physics_angular_velocity(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	PyObject *py_obj_new_ang_vel = nullptr;
	PyObject *is_add_to_current = NULL;
	char *bone_name = nullptr;
	if (!PyArg_ParseTuple(args, "O|Os:set_physics_angular_velocity", &py_obj_new_ang_vel, &is_add_to_current, &bone_name))
	{
		return nullptr;
	}

	FVector new_ang_vel = FVector(0, 0, 0);
	if (py_obj_new_ang_vel)
	{
		ue_PyFVector *py_new_ang_vel = py_ue_is_fvector(py_obj_new_ang_vel);
		if (!py_new_ang_vel)
			return PyErr_Format(PyExc_Exception, "torque must be a FVector");
		new_ang_vel = py_new_ang_vel->vec;
	}

	bool add_to_current = false;
	if (is_add_to_current && PyObject_IsTrue(is_add_to_current))
		add_to_current = true;

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}

#if ENGINE_MINOR_VERSION >= 18
	primitive->SetPhysicsAngularVelocityInDegrees(new_ang_vel, add_to_current, f_bone_name);
#else
	primitive->SetPhysicsAngularVelocity(new_ang_vel, add_to_current, f_bone_name);
#endif

	Py_RETURN_NONE;
}

PyObject *py_ue_get_physics_angular_velocity(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>())
	{
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	char *bone_name = nullptr;
	if (!PyArg_ParseTuple(args, "|s:get_physics_angular_velocity", &bone_name))
	{
		return nullptr;
	}

	FName f_bone_name = NAME_None;
	if (bone_name)
	{
		f_bone_name = FName(UTF8_TO_TCHAR(bone_name));
	}
#if ENGINE_MINOR_VERSION >= 18
	return py_ue_new_fvector(primitive->GetPhysicsAngularVelocityInDegrees(f_bone_name));
#else
	return py_ue_new_fvector(primitive->GetPhysicsAngularVelocity(f_bone_name));
#endif
}


PyObject *py_ue_destructible_apply_damage(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	float damage_amount = 0;
	float impulse_strength = 0;
	PyObject *py_obj_location = nullptr;
	PyObject *py_obj_impulse = nullptr;
	if (!PyArg_ParseTuple(args, "ffOO:destructible_apply_damage", &damage_amount, &impulse_strength, &py_obj_location, &py_obj_impulse))
	{
		return NULL;
	}

#if ENGINE_MINOR_VERSION < 18
	UDestructibleComponent *destructible = ue_py_check_type<UDestructibleComponent>(self);
#else
	IDestructibleInterface *destructible = ue_py_check_type<IDestructibleInterface>(self);
#endif

	if (!destructible)
	{
		AActor *actor = ue_py_check_type<AActor>(self);
		if (actor)
		{
#if ENGINE_MINOR_VERSION < 18
			destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
#else
			for (UActorComponent *component : actor->GetComponents())
			{
				if (Cast<IDestructibleInterface>(component))
				{
					destructible = (IDestructibleInterface *)component;
					break;
				}
			}
#endif
		}
		else
		{
			UActorComponent *component = ue_py_check_type<UActorComponent>(self);
			if (component)
			{
				actor = (AActor *)component->GetOuter();
#if ENGINE_MINOR_VERSION < 18
				destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
#else
				for (UActorComponent *checked_component : actor->GetComponents())
				{
					if (Cast<IDestructibleInterface>(checked_component))
					{
						destructible = (IDestructibleInterface *)checked_component;
						break;
					}
				}
#endif
			}
		}
	}


	FVector location = FVector(0, 0, 0);
	FVector impulse = FVector(0, 0, 0);

	if (py_obj_location)
	{
		ue_PyFVector *py_location = py_ue_is_fvector(py_obj_location);
		if (!py_location)
			return PyErr_Format(PyExc_Exception, "location must be a FVector");
		location = py_location->vec;
	}

	if (py_obj_impulse)
	{
		ue_PyFVector *py_impulse = py_ue_is_fvector(py_obj_impulse);
		if (!py_impulse)
			return PyErr_Format(PyExc_Exception, "impulse must be a FVector");
		impulse = py_impulse->vec;
	}

	if (destructible)
	{
		destructible->ApplyDamage(damage_amount, location, impulse, impulse_strength);
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "UObject is not a destructible");
	}

	Py_RETURN_NONE;
}
