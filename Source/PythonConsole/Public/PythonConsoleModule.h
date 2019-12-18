// Copyright 1998-2018 20Tab S.r.l. All Rights Reserved.

#pragma once

#include "Runtime/Slate/Public/SlateBasics.h"

/** Style of the debug console */
namespace EPythonConsoleStyle
{
	enum Type
	{
		/** Shows the debug console input line with tab completion only */
		Compact,

		/** Shows a scrollable log window with the input line on the bottom */
		WithLog,
	};
};

struct FPythonConsoleDelegates
{
	FSimpleDelegate OnFocusLost;
	FSimpleDelegate OnConsoleCommandExecuted;
};

class FPythonConsoleModule : public IModuleInterface
{
public:
	virtual void StartupModule();
	virtual void ShutdownModule();

	/** Generates a console input box widget.  Remember, this widget will become invalid if the
	    output log DLL is unloaded on the fly. */
	virtual TSharedRef< SWidget > MakeConsoleInputBox( TSharedPtr< SEditableTextBox >& OutExposedEditableTextBox ) const;


private:

	/** Weak pointer to a debug console that's currently open, if any */
	TWeakPtr< SWidget > PythonConsole;
};
