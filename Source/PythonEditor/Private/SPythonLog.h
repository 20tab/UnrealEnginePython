// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "BaseTextLayoutMarshaller.h"
#include "TextFilterExpressionEvaluator.h"



class FPythonLogTextLayoutMarshaller;

/**
* A single log message for the output log, holding a message and
* a style, for color and bolding of the message.
*/
struct FLogMessage
{
	TSharedRef<FString> Message;
	FName Style;

	FLogMessage(const TSharedRef<FString>& NewMessage, FName NewStyle = NAME_None)
		: Message(NewMessage)
		, Style(NewStyle)
	{
	}

};


/**
 * Widget which holds a list view of logs of the program output
 * as well as a combo box for entering in new commands
 */
class SPythonLog 
	: public SCompoundWidget, public FOutputDevice
{

public:

	SLATE_BEGIN_ARGS( SPythonLog )
		: _Messages()
		{}
		
		/** All messages captured before this log window has been created */
		SLATE_ARGUMENT( TArray< TSharedPtr<FLogMessage> >, Messages )

	SLATE_END_ARGS()

	/** Destructor for output log, so we can unregister from notifications */
	~SPythonLog();

	/**
	 * Construct this widget.  Called by the SNew() Slate macro.
	 *
	 * @param	InArgs	Declaration used by the SNew() macro to construct this widget
	 */
	void Construct( const FArguments& InArgs );

	/**
	 * Creates FLogMessage objects from FOutputDevice log callback
	 *
	 * @param	V Message text
	 * @param Verbosity Message verbosity
	 * @param Category Message category
	 * @param OutMessages Array to receive created FLogMessage messages
	 * @param Filters [Optional] Filters to apply to Messages
	 *
	 * @return true if any messages have been created, false otherwise
	 */
	static bool CreateLogMessages(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, TArray< TSharedPtr<FLogMessage> >& OutMessages);

protected:

	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category ) override;

protected:
	/**
	 * Extends the context menu used by the text box
	 */
	void ExtendTextBoxMenu(FMenuBuilder& Builder);

	/**
	 * Called when delete all is selected
	 */
	void OnClearLog();

	/**
	 * Called when the user scrolls the log window vertically
	 */
	void OnUserScrolled(float ScrollOffset);

	/**
	 * Called to determine whether delete all is currently a valid command
	 */
	bool CanClearLog() const;

	/** Called when a console command is entered for this output log */
	void OnConsoleCommandExecuted();

	/** Request we immediately force scroll to the bottom of the log */
	void RequestForceScroll();

	/** Converts the array of messages into something the text box understands */
	TSharedPtr< FPythonLogTextLayoutMarshaller > MessagesTextMarshaller;

	/** The editable text showing all log messages */
	TSharedPtr< SMultiLineEditableTextBox > MessagesTextBox;


	/** True if the user has scrolled the window upwards */
	bool bIsUserScrolled;

};

/** Output log text marshaller to convert an array of FLogMessages into styled lines to be consumed by an FTextLayout */
class FPythonLogTextLayoutMarshaller : public FBaseTextLayoutMarshaller
{
public:

	static TSharedRef< FPythonLogTextLayoutMarshaller > Create(TArray< TSharedPtr<FLogMessage> > InMessages);

	virtual ~FPythonLogTextLayoutMarshaller();

	// ITextLayoutMarshaller
	virtual void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override;
	virtual void GetText(FString& TargetString, const FTextLayout& SourceTextLayout) override;

	bool AppendMessage(const TCHAR* InText, const ELogVerbosity::Type InVerbosity, const FName& InCategory);
	void ClearMessages();

	int32 GetNumMessages() const;

protected:

	FPythonLogTextLayoutMarshaller(TArray< TSharedPtr<FLogMessage> > InMessages);

	void AppendMessageToTextLayout(const TSharedPtr<FLogMessage>& InMessage);
	void AppendMessagesToTextLayout(const TArray<TSharedPtr<FLogMessage>>& InMessages);

	/** All log messages to show in the text box */
	TArray< TSharedPtr<FLogMessage> > Messages;

	/** Holds cached numbers of messages to avoid unnecessary re-filtering */
	int32 CachedNumMessages;

	/** Flag indicating the messages count cache needs rebuilding */
	bool bNumMessagesCacheDirty;

	FTextLayout* TextLayout;
};