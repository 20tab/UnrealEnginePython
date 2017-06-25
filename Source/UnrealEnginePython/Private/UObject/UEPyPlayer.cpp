#include "UnrealEnginePythonPrivatePCH.h"

PyObject *py_ue_get_player_controller(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|i:get_player_controller", &controller_id)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	ue_PyUObject *ret = ue_get_python_wrapper(controller);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_create_player(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int controller_id;
	PyObject *spawn_pawn;
	if (!PyArg_ParseTuple(args, "i|O:create_player", &controller_id, &spawn_pawn)) {
		return NULL;
	}

	bool b_spawn_pawn = true;

	if (spawn_pawn && !PyObject_IsTrue(spawn_pawn)) {
		b_spawn_pawn = false;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::CreatePlayer(world, controller_id, b_spawn_pawn);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to create a new player fro controller %d", controller_id);

	return PyLong_FromLong(controller->PlayerState->PlayerId);
}

PyObject *py_ue_get_num_players(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");
#if ENGINE_MINOR_VERSION < 14
	AGameMode *game_mode = world->GetAuthGameMode();
#else
	AGameModeBase *game_mode = world->GetAuthGameMode();
#endif
	if (!game_mode)
		return PyErr_Format(PyExc_Exception, "unable to retrieve GameMode from world");
#if ENGINE_MINOR_VERSION < 14
	return PyLong_FromLong(game_mode->NumPlayers);
#else
	return PyLong_FromLong(game_mode->GetNumPlayers());
#endif
}

PyObject *py_ue_get_num_spectators(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");
#if ENGINE_MINOR_VERSION < 14
	AGameMode *game_mode = world->GetAuthGameMode();
#else
	AGameModeBase *game_mode = world->GetAuthGameMode();
#endif
	if (!game_mode)
		return PyErr_Format(PyExc_Exception, "unable to retrieve GameMode from world");
#if ENGINE_MINOR_VERSION < 14
	return PyLong_FromLong(game_mode->NumSpectators);
#else
	return PyLong_FromLong(game_mode->GetNumSpectators());
#endif
}

