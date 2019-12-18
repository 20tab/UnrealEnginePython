// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SPythonConsole.h"
#include "PythonConsoleModule.h"
#include "SPythonConsole.h"
#include "SPythonLog.h"

namespace PythonConsoleDefs
{
	// How many seconds to animate when console is summoned
	static const float IntroAnimationDuration = 0.25f;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPythonConsole::Construct( const FArguments& InArgs, const EPythonConsoleStyle::Type InStyle, FPythonConsoleModule* PythonConsoleModule, const FPythonConsoleDelegates* PythonConsoleDelegates )
{
	CurrentStyle = InStyle;

	TSharedPtr<SPythonConsoleInputBox> ConsoleInputBox;

	check( PythonConsoleModule != NULL );
	ChildSlot
	[ 
		SNew( SVerticalBox )
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew( SVerticalBox )
				.Visibility( this, &SPythonConsole::MakeVisibleIfLogIsShown )
					
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding( 10.0f )
				[
					SNew(SBox)
					.HeightOverride( 200.0f )
					[
						SNew( SBorder )
							.BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
							.ColorAndOpacity( this, &SPythonConsole::GetAnimatedColorAndOpacity )
							.BorderBackgroundColor( this, &SPythonConsole::GetAnimatedSlateColor )
							[
								SNew( SSpacer )
							]
					]
				]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding( 10.0f )
		[
			SNew(SBox)
			.HeightOverride( 26.0f )
			.HAlign( HAlign_Left )
			[
				SNew( SBorder )
				.Padding( FMargin(2) )
				.BorderImage( FEditorStyle::GetBrush( "PythonConsole.Background" ) )
				.ColorAndOpacity( this, &SPythonConsole::GetAnimatedColorAndOpacity )
				.BorderBackgroundColor( this, &SPythonConsole::GetAnimatedSlateColor )
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(3.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Console", "ConsoleLabel", "Console"))

					]
					+ SHorizontalBox::Slot()
					.Padding(5.0f, 2.0f)
					.VAlign(VAlign_Center)
					.MaxWidth(400.0f)
					[
						SAssignNew(ConsoleInputBox, SPythonConsoleInputBox)
						.OnConsoleCommandExecuted(PythonConsoleDelegates->OnConsoleCommandExecuted)
					]
				]
			]
		]
	];

	EditableTextBox = ConsoleInputBox->GetEditableTextBox();

	// Kick off intro animation
	AnimCurveSequence = FCurveSequence();
	AnimCurve = AnimCurveSequence.AddCurve( 0.0f, PythonConsoleDefs::IntroAnimationDuration, ECurveEaseFunction::QuadOut );
	FlashCurve = AnimCurveSequence.AddCurve( PythonConsoleDefs::IntroAnimationDuration, .15f, ECurveEaseFunction::QuadInOut );

	AnimCurveSequence.Play(this->AsShared());
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SPythonConsole::SPythonConsole()
	: CurrentStyle( EPythonConsoleStyle::Compact )
{
}


void SPythonConsole::SetFocusToEditableText()
{
	FSlateApplication::Get().SetKeyboardFocus( EditableTextBox.ToSharedRef(), EFocusCause::SetDirectly );
}

EVisibility SPythonConsole::MakeVisibleIfLogIsShown() const
{
	return CurrentStyle == EPythonConsoleStyle::WithLog ? EVisibility::Visible : EVisibility::Collapsed;
}


FLinearColor SPythonConsole::GetAnimatedColorAndOpacity() const
{
	return FLinearColor( 1.0f, 1.0f, 1.0f, AnimCurve.GetLerp() );
}


FSlateColor SPythonConsole::GetAnimatedSlateColor() const
{
	return FSlateColor( GetAnimatedColorAndOpacity() );
}

FSlateColor SPythonConsole::GetFlashColor() const
{
	float FlashAlpha = 1.0f - FlashCurve.GetLerp();

	if (FlashAlpha == 1.0f)
	{
		FlashAlpha = 0.0f;
	}

	return FLinearColor(1,1,1,FlashAlpha);
}