// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorPrivatePCH.h"
#include "SPythonLog.h"

#define LOCTEXT_NAMESPACE "PythonConsole"


TSharedRef< FPythonLogTextLayoutMarshaller > FPythonLogTextLayoutMarshaller::Create(TArray< TSharedPtr<FLogMessage> > InMessages)
{
	return MakeShareable(new FPythonLogTextLayoutMarshaller(MoveTemp(InMessages)));
}

FPythonLogTextLayoutMarshaller::~FPythonLogTextLayoutMarshaller()
{
}

void FPythonLogTextLayoutMarshaller::SetText(const FString& SourceString, FTextLayout& TargetTextLayout)
{
	TextLayout = &TargetTextLayout;
	AppendMessagesToTextLayout(Messages);
}

void FPythonLogTextLayoutMarshaller::GetText(FString& TargetString, const FTextLayout& SourceTextLayout)
{
	SourceTextLayout.GetAsText(TargetString);
}

bool FPythonLogTextLayoutMarshaller::AppendMessage(const TCHAR* InText, const ELogVerbosity::Type InVerbosity, const FName& InCategory)
{
	TArray< TSharedPtr<FLogMessage> > NewMessages;
	if (SPythonLog::CreateLogMessages(InText, InVerbosity, InCategory, NewMessages))
	{
		const bool bWasEmpty = Messages.Num() == 0;
		Messages.Append(NewMessages);

		if (TextLayout)
		{
			// If we were previously empty, then we'd have inserted a dummy empty line into the document
			// We need to remove this line now as it would cause the message indices to get out-of-sync with the line numbers, which would break auto-scrolling
			if (bWasEmpty)
			{
				TextLayout->ClearLines();
			}

			// If we've already been given a text layout, then append these new messages rather than force a refresh of the entire document
			AppendMessagesToTextLayout(NewMessages);
		}
		else
		{
			MakeDirty();
		}

		return true;
	}

	return false;
}

void FPythonLogTextLayoutMarshaller::AppendMessageToTextLayout(const TSharedPtr<FLogMessage>& InMessage)
{

	const FTextBlockStyle& MessageTextStyle = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>(InMessage->Style);

	TSharedRef<FString> LineText = InMessage->Message;

	TArray<TSharedRef<IRun>> Runs;
	Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, MessageTextStyle));

	TextLayout->AddLine(FSlateTextLayout::FNewLineData(MoveTemp(LineText), MoveTemp(Runs)));
}

void FPythonLogTextLayoutMarshaller::AppendMessagesToTextLayout(const TArray<TSharedPtr<FLogMessage>>& InMessages)
{
	TArray<FTextLayout::FNewLineData> LinesToAdd;
	LinesToAdd.Reserve(InMessages.Num());

	for (const auto& CurrentMessage : InMessages)
	{


		const FTextBlockStyle& MessageTextStyle = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>(CurrentMessage->Style);

		TSharedRef<FString> LineText = CurrentMessage->Message;

		TArray<TSharedRef<IRun>> Runs;
		Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, MessageTextStyle));

		LinesToAdd.Emplace(MoveTemp(LineText), MoveTemp(Runs));
	}

	TextLayout->AddLines(LinesToAdd);
}

void FPythonLogTextLayoutMarshaller::ClearMessages()
{
	Messages.Empty();
	MakeDirty();

}

int32 FPythonLogTextLayoutMarshaller::GetNumMessages() const
{
	return Messages.Num();
}

FPythonLogTextLayoutMarshaller::FPythonLogTextLayoutMarshaller(TArray< TSharedPtr<FLogMessage> > InMessages)
	: Messages(MoveTemp(InMessages))
	, TextLayout(nullptr)
{
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPythonLog::Construct(const FArguments& InArgs)
{
	MessagesTextMarshaller = FPythonLogTextLayoutMarshaller::Create(MoveTemp(InArgs._Messages));

	MessagesTextBox = SNew(SMultiLineEditableTextBox)
		.Style(FEditorStyle::Get(), "Log.TextBox")
		.TextStyle(FEditorStyle::Get(), "Log.Normal")
		.ForegroundColor(FLinearColor::Gray)
		.Marshaller(MessagesTextMarshaller)
		.IsReadOnly(true)
		.AlwaysShowScrollbars(true)
		.OnVScrollBarUserScrolled(this, &SPythonLog::OnUserScrolled)
		.ContextMenuExtender(this, &SPythonLog::ExtendTextBoxMenu);

	ChildSlot
	[
			SNew(SVerticalBox)

			// Console output and filters
			+ SVerticalBox::Slot()
			[
				SNew(SBorder)
				.Padding(3)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SVerticalBox)

					// Output log area
					+ SVerticalBox::Slot()
					.FillHeight(1)
					[
						MessagesTextBox.ToSharedRef()
					]
				]
			]

	];

	GLog->AddOutputDevice(this);

	bIsUserScrolled = false;
	RequestForceScroll();

	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	FString StdoutHook = TEXT("import sys\r\nimport unreal_engine as ue\r\nclass UnrealEngineOutput:\r\n\tdef __init__(self, logger):\r\n\t\tself.logger = logger\r\n\tdef write(self, buf) :\r\n\t\tself.logger(buf)\r\n\tdef flush(self) :\r\n\t\treturn\r\nsys.stdout = UnrealEngineOutput(ue.log)\r\nsys.stderr = UnrealEngineOutput(ue.log_error)");
	PythonModule.RunString(TCHAR_TO_UTF8(*StdoutHook));
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SPythonLog::~SPythonLog()
{
	if (GLog != NULL)
	{
		GLog->RemoveOutputDevice(this);
	}
}

bool SPythonLog::CreateLogMessages(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, TArray< TSharedPtr<FLogMessage> >& OutMessages)
{
	if (Verbosity == ELogVerbosity::SetColor)
	{
		// Skip Color Events
		return false;
	}
	else
	{
		FName Style;
		if (Category == NAME_Cmd)
		{
			Style = FName(TEXT("Log.Command"));
		}
		else if (Verbosity == ELogVerbosity::Error)
		{
			Style = FName(TEXT("Log.Error"));
		}
		else if (Verbosity == ELogVerbosity::Warning)
		{
			Style = FName(TEXT("Log.Warning"));
		}
		else
		{
			Style = FName(TEXT("Log.Normal"));
		}

		// Determine how to format timestamps
		static ELogTimes::Type LogTimestampMode = ELogTimes::None;
		if (UObjectInitialized() && !GExitPurge)
		{
			// Logging can happen very late during shutdown, even after the UObject system has been torn down, hence the init check above
			LogTimestampMode = GetDefault<UEditorStyleSettings>()->LogTimestampMode;
		}

		const int32 OldNumMessages = OutMessages.Num();

		// handle multiline strings by breaking them apart by line
		TArray<FTextRange> LineRanges;
		FString CurrentLogDump = V;
		FTextRange::CalculateLineRangesFromString(CurrentLogDump, LineRanges);

		bool bIsFirstLineInMessage = true;
		for (const FTextRange& LineRange : LineRanges)
		{
			if (!LineRange.IsEmpty())
			{
				FString Line = CurrentLogDump.Mid(LineRange.BeginIndex, LineRange.Len());
				Line = Line.ConvertTabsToSpaces(4);

				OutMessages.Add(MakeShareable(new FLogMessage(MakeShareable(new FString(Line)), Style)));

				bIsFirstLineInMessage = false;
			}
		}

		return OldNumMessages != OutMessages.Num();
	}
}

void SPythonLog::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), Category.ToString())
	if (MessagesTextMarshaller->AppendMessage(V, Verbosity, Category))
	{
		// Don't scroll to the bottom automatically when the user is scrolling the view or has scrolled it away from the bottom.
		if (!bIsUserScrolled)
		{
			MessagesTextBox->ScrollTo(FTextLocation(MessagesTextMarshaller->GetNumMessages() - 1));
		}
	}
}

void SPythonLog::ExtendTextBoxMenu(FMenuBuilder& Builder)
{
	FUIAction ClearPythonLogAction(
		FExecuteAction::CreateRaw(this, &SPythonLog::OnClearLog),
		FCanExecuteAction::CreateSP(this, &SPythonLog::CanClearLog)
		);

	Builder.AddMenuEntry(
		NSLOCTEXT("PythonConsole", "ClearLogLabel", "Clear Log"),
		NSLOCTEXT("PythonConsole", "ClearLogTooltip", "Clears all log messages"),
		FSlateIcon(),
		ClearPythonLogAction
		);
}

void SPythonLog::OnClearLog()
{
	// Make sure the cursor is back at the start of the log before we clear it
	MessagesTextBox->GoTo(FTextLocation(0));

	MessagesTextMarshaller->ClearMessages();
	MessagesTextBox->Refresh();
	bIsUserScrolled = false;
}

void SPythonLog::OnUserScrolled(float ScrollOffset)
{
	bIsUserScrolled = !FMath::IsNearlyEqual(ScrollOffset, 1.0f);
}

bool SPythonLog::CanClearLog() const
{
	return MessagesTextMarshaller->GetNumMessages() > 0;
}

void SPythonLog::OnConsoleCommandExecuted()
{
	RequestForceScroll();
}

void SPythonLog::RequestForceScroll()
{
	if (MessagesTextMarshaller->GetNumMessages() > 0)
	{
		MessagesTextBox->ScrollTo(FTextLocation(MessagesTextMarshaller->GetNumMessages() - 1));
		bIsUserScrolled = false;
	}
}



#undef LOCTEXT_NAMESPACE
