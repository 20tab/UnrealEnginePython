// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateCore.h"
#include "PythonProjectItem.h"

class SProjectViewItem : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnNameChanged, UPythonProjectItem*);

	SLATE_BEGIN_ARGS(SProjectViewItem) {}

	SLATE_ARGUMENT(UPythonProjectItem*, TreeItem)

	SLATE_ARGUMENT(FName, IconName)

	SLATE_ARGUMENT(FText, Text)

	SLATE_EVENT(FOnNameChanged, OnNameChanged)

	SLATE_EVENT(FIsSelected, IsSelected)


	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
private:

	UPythonProjectItem* TreeItem;

	TSharedPtr< SInlineEditableTextBlock > InlineRenameWidget;

	FOnNameChanged OnNameChanged;

	void HandleNameCommitted(const FText& NewText, ETextCommit::Type );
};