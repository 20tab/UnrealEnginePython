#include "UEPyPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"

PyObject *py_ue_get_player_controller(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|i:get_player_controller", &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	Py_RETURN_UOBJECT(controller);
}

PyObject *py_ue_get_player_camera_manager(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|i:get_player_camera_manager", &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerCameraManager *camera = UGameplayStatics::GetPlayerCameraManager(world, controller_id);
	if (!camera)
		return PyErr_Format(PyExc_Exception, "unable to retrieve camera manager for controller %d", controller_id);

	Py_RETURN_UOBJECT(camera);
}

PyObject *py_ue_get_player_hud(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|i:get_player_hud", &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	Py_RETURN_UOBJECT(controller->GetHUD());
}

PyObject *py_ue_set_player_hud(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_hud;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "O|i:set_player_hud", &py_hud, &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	AHUD *hud = ue_py_check_type<AHUD>(py_hud);
	if (!hud)
		return PyErr_Format(PyExc_Exception, "argument is not a AHUD");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	controller->MyHUD = hud;

	Py_RETURN_NONE;
}

PyObject *py_ue_get_player_pawn(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|i:get_player_pawn", &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	// the controller could not have a pawn
	if (!controller->GetPawn())
		Py_RETURN_NONE;

	Py_RETURN_UOBJECT(controller->GetPawn());
}

PyObject *py_ue_create_player(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int controller_id;
	PyObject *spawn_pawn;
	if (!PyArg_ParseTuple(args, "i|O:create_player", &controller_id, &spawn_pawn))
	{
		return NULL;
	}

	bool b_spawn_pawn = true;

	if (spawn_pawn && !PyObject_IsTrue(spawn_pawn))
	{
		b_spawn_pawn = false;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::CreatePlayer(world, controller_id, b_spawn_pawn);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to create a new player from controller %d", controller_id);

	return PyLong_FromLong(controller->PlayerState->PlayerId);
}

PyObject *py_ue_get_num_players(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_get_num_spectators(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_restart_level(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|i:restart_level", &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	controller->RestartLevel();

	Py_RETURN_NONE;
}

