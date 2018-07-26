// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SPythonLog.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScrollBorder.h"
#include "GameFramework/GameMode.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameState.h"
#include "Runtime/Slate/Public/Widgets/Input/SSearchBox.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/Slate/Public/Framework/Text/SlateTextLayout.h"
#include "Editor/EditorStyle/Public/Classes/EditorStyleSettings.h"
#include "SlateBasics.h"
#include "EditorStyle.h"

#define LOCTEXT_NAMESPACE "PythonConsole"


/** Custom console editable text box whose only purpose is to prevent some keys from being typed */
class SPythonConsoleEditableTextBox : public SEditableTextBox
{
public:
	SLATE_BEGIN_ARGS(SPythonConsoleEditableTextBox) {}

	/** Hint text that appears when there is no text in the text box */
	SLATE_ATTRIBUTE(FText, HintText)

		/** Called whenever the text is changed interactively by the user */
		SLATE_EVENT(FOnTextChanged, OnTextChanged)

		/** Called whenever the text is committed.  This happens when the user presses enter or the text box loses focus. */
		SLATE_EVENT(FOnTextCommitted, OnTextCommitted)

		SLATE_END_ARGS()


		void Construct(const FArguments& InArgs)
	{
		SetStyle(&FCoreStyle::Get().GetWidgetStyle< FEditableTextBoxStyle >("NormalEditableTextBox"));

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SPythonConsoleEditableTextBox::GetConsoleBorder)
			.BorderBackgroundColor(Style->BackgroundColor)
			.ForegroundColor(Style->ForegroundColor)
			.Padding(Style->Padding)
			[
				SAssignNew(EditableText, SPythonConsoleEditableText)
				.HintText(InArgs._HintText)
			.OnTextChanged(InArgs._OnTextChanged)
			.OnTextCommitted(InArgs._OnTextCommitted)
			]);
	}

	void SetPythonBox(SPythonConsoleInputBox *box)
	{
		SPythonConsoleEditableText *PythonEditableText = (SPythonConsoleEditableText *)EditableText.Get();
		box->HistoryPosition = 0;
		PythonEditableText->PythonConsoleInputBox = box;
	}

private:
	class SPythonConsoleEditableText : public SEditableText
	{
	public:

		SPythonConsoleInputBox *PythonConsoleInputBox;

		SLATE_BEGIN_ARGS(SPythonConsoleEditableText) {}
		/** The text that appears when there is nothing typed into the search box */
		SLATE_ATTRIBUTE(FText, HintText)
			/** Called whenever the text is changed interactively by the user */
			SLATE_EVENT(FOnTextChanged, OnTextChanged)

			/** Called whenever the text is committed.  This happens when the user presses enter or the text box loses focus. */
			SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
			SLATE_END_ARGS()

			void Construct(const FArguments& InArgs)
		{
			SEditableText::Construct
			(
				SEditableText::FArguments()
				.HintText(InArgs._HintText)
				.OnTextChanged(InArgs._OnTextChanged)
				.OnTextCommitted(InArgs._OnTextCommitted)
				.ClearKeyboardFocusOnCommit(false)
				.IsCaretMovedWhenGainFocus(false)
				.MinDesiredWidth(400.0f)
			);
		}

		virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
		{
			// Special case handling.  Intercept the tilde key.  It is not suitable for typing in the console
			if (InKeyEvent.GetKey() == EKeys::Tilde)
			{
				return FReply::Unhandled();
			}
			else if (InKeyEvent.GetKey() == EKeys::Up)
			{
				if (PythonConsoleInputBox->HistoryPosition > 0)
				{
					PythonConsoleInputBox->HistoryPosition--;
					this->SetText(FText::FromString(PythonConsoleInputBox->History[PythonConsoleInputBox->HistoryPosition]));
				}

				return FReply::Handled();
			}
			else if (InKeyEvent.GetKey() == EKeys::Down)
			{
				if (PythonConsoleInputBox->HistoryPosition < PythonConsoleInputBox->History.Num() - 1)
				{
					PythonConsoleInputBox->HistoryPosition++;
					this->SetText(FText::FromString(PythonConsoleInputBox->History[PythonConsoleInputBox->HistoryPosition]));
				}

				return FReply::Handled();
			}
			else
			{
				return SEditableText::OnKeyDown(MyGeometry, InKeyEvent);
			}
		}

		virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
		{
			// Special case handling.  Intercept the tilde key.  It is not suitable for typing in the console
			if (InCharacterEvent.GetCharacter() != 0x60)
			{
				return SEditableText::OnKeyChar(MyGeometry, InCharacterEvent);
			}
			else
			{
				return FReply::Unhandled();
			}
		}

	};

	/** @return Border image for the text box based on the hovered and focused state */
	const FSlateBrush* GetConsoleBorder() const
	{
		if (EditableText->HasKeyboardFocus())
		{
			return &Style->BackgroundImageFocused;
		}
		else
		{
			if (EditableText->IsHovered())
			{
				return &Style->BackgroundImageHovered;
			}
			else
			{
				return &Style->BackgroundImageNormal;
			}
		}
	}

};

SPythonConsoleInputBox::SPythonConsoleInputBox()
	: bIgnoreUIUpdate(false)
{
	FScopePythonGIL gil;

}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPythonConsoleInputBox::Construct(const FArguments& InArgs)
{
	OnConsoleCommandExecuted = InArgs._OnConsoleCommandExecuted;

	ChildSlot
		[

			SAssignNew(InputText, SPythonConsoleEditableTextBox)
			.OnTextCommitted(this, &SPythonConsoleInputBox::OnTextCommitted)
		.HintText(NSLOCTEXT("PythonConsole", "TypeInConsoleHint", "Enter python command"))

		];

	SPythonConsoleEditableTextBox *TextBox = (SPythonConsoleEditableTextBox *)InputText.Get();
	TextBox->SetPythonBox(this);
	IsMultiline = false;
}
void SPythonConsoleInputBox::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (!GIntraFrameDebuggingGameThread && !IsEnabled())
	{
		SetEnabled(true);
	}
	else if (GIntraFrameDebuggingGameThread && IsEnabled())
	{
		SetEnabled(false);
	}
}


void SPythonConsoleInputBox::OnTextCommitted(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		if (!InText.IsEmpty())
		{

			// Copy the exec text string out so we can clear the widget's contents.  If the exec command spawns
			// a new window it can cause the text box to lose focus, which will result in this function being
			// re-entered.  We want to make sure the text string is empty on re-entry, so we'll clear it out
			const FString ExecString = InText.ToString();

			this->History.Add(ExecString);
			this->HistoryPosition = this->History.Num();

			if (IsMultiline)
			{
				UE_LOG(LogTemp, Log, TEXT("... %s"), *ExecString);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT(">>> %s"), *ExecString);
			}

			// Clear the console input area
			bIgnoreUIUpdate = true;
			InputText->SetText(FText::GetEmpty());
			bIgnoreUIUpdate = false;

			// Here run the python code
			//
			FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");

			if (IsMultiline)
			{
				if (ExecString.StartsWith(" "))
				{
					MultilineString += FString("\n") + ExecString;
				}
				else
				{
					IsMultiline = false;
					PythonModule.RunString(TCHAR_TO_UTF8(*MultilineString));
				}
			}
			else if (ExecString.EndsWith(":"))
			{
				IsMultiline = true;
				MultilineString = ExecString;
			}
			else
			{
				PythonModule.RunString(TCHAR_TO_UTF8(*ExecString));
			}

		}
		else if (IsMultiline)
		{
			IsMultiline = false;
			FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
			PythonModule.RunString(TCHAR_TO_UTF8(*MultilineString));
		}

	}

	OnConsoleCommandExecuted.ExecuteIfBound();
}


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
#if ENGINE_MINOR_VERSION < 18
	MessagesTextMarshaller = FPythonLogTextLayoutMarshaller::Create(MoveTemp(InArgs._Messages));
#else
	MessagesTextMarshaller = FPythonLogTextLayoutMarshaller::Create(InArgs._Messages);
#endif

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

	// The console input box
	+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(0.0f, 4.0f, 0.0f, 0.0f))
		[
			SNew(SPythonConsoleInputBox)
			.OnConsoleCommandExecuted(this, &SPythonLog::OnConsoleCommandExecuted)
		]];

	GLog->AddOutputDevice(this);

	bIsUserScrolled = false;
	RequestForceScroll();
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
