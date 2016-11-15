// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#include "DedicatedServerPrivatePCH.h"

void DumpConsoleHelp();

#if WITH_SERVER_CODE && PLATFORM_MAC
	void FServerConsole::Tick()
	{
	}

	void FServerConsole::SendNullInput()
	{
	}

	void FServerConsole::ClearInputLine()
	{
	}

	void FServerConsole::RedrawInputLine()
	{
	}
#endif
