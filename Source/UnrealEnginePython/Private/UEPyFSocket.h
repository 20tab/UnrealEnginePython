#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Networking.h"



typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FSocket *sock;
	FUdpSocketReceiver *udp_receiver;
	void udp_recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt) {
		UE_LOG(LogPython, Warning, TEXT("DATA !!!"));
	}
} ue_PyFSocket;

void ue_python_init_fsocket(PyObject *);