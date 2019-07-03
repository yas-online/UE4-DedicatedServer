// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#include "DedicatedServerPrivatePCH.h"

DEFINE_LOG_CATEGORY( LogServerConsole );

// Fixme: We need to workaround a silly issue inside the engine where the help commands relies on Slate being present...
// See: https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Engine/Private/UnrealEngine.cpp#L3151
void DumpConsoleHelp()
{
	#if !UE_BUILD_SHIPPING
		UE_LOG( LogServerConsole, Display, TEXT( "\n" )
			TEXT( "Console Help:" ) TEXT( "\n" )
			TEXT( "=============" ) TEXT( "\n" )
			TEXT( "\n" )
			TEXT( "A console variable is a engine wide key value pair." ) TEXT( "\n" )
			TEXT( "The key is a string usually starting with the subsystem prefix followed by '.' e.g. r.BloomQuality." ) TEXT( "\n" )
			TEXT( "The value can be of different tpe (e.g. float, int, string)." ) TEXT( "\n" )
			TEXT( "A console command has no state associated with and gets executed immediately." ) TEXT( "\n" )
			TEXT( "\n" )
			TEXT( "Console variables can be put into ini files (e.g. ConsoleVariables.ini or BaseEngine.ini) with this syntax:" ) TEXT( "\n" )
			TEXT( "<Console variable> = <value>" ) TEXT( "\n" )
			TEXT( "\n" )
			TEXT( "DumpConsoleCommands         Lists all console variables and commands that are registered (Some are not registered)" ) TEXT( "\n" )
			TEXT( "<Console variable>          Get the console variable state" ) TEXT( "\n" )
			TEXT( "<Console variable> ?        Get the console variable help text" ) TEXT( "\n" )
			TEXT( "<Console variable> <value>  Set the console variable value" ) TEXT( "\n" )
			TEXT( "<Console command> [Params]  Execute the console command with optional parameters" ) TEXT( "\n" )
		);

		FString FilePath = FPaths::ProjectSavedDir() + TEXT( "ConsoleHelp.html" );

		UE_LOG( LogServerConsole, Display, TEXT( "\n" )
			TEXT( "To browse console variables open this: '%s'" )
		, *FilePath );

		ConsoleCommandLibrary_DumpLibraryHTML( GEngine->GetWorld(), *GEngine, FilePath );
	#endif
}

#if WITH_SERVER_CODE
	#if PLATFORM_WINDOWS
		#include "AllowWindowsPlatformTypes.h"

		namespace ConsoleConstants
		{
			uint32 WIN_ATTACH_PARENT_PROCESS = ATTACH_PARENT_PROCESS;
			uint32 WIN_STD_INPUT_HANDLE = STD_INPUT_HANDLE;
			uint32 WIN_STD_OUTPUT_HANDLE = STD_OUTPUT_HANDLE;
			uint32 WIN_STD_ERROR_HANDLE = STD_ERROR_HANDLE;
		}

		#include "HideWindowsPlatformTypes.h"
	#endif

	FServerConsole::FServerConsole()
	{
		m_pConsole = static_cast<FOutputDeviceConsolePlatform*>( GLogConsole );
		m_iCommandHistoryIndex = -1;

		#if PLATFORM_WINDOWS
			m_hOutputHandle = INVALID_HANDLE_VALUE;
			m_hInputHandle = INVALID_HANDLE_VALUE;
		#endif
	}

	FServerConsole::~FServerConsole()
	{
	}

	void FServerConsole::Show( bool bShowWindow )
	{
		if( !m_pConsole->IsShown() ) m_pConsole->Show( bShowWindow );

		#if PLATFORM_WINDOWS
			if( m_hOutputHandle == INVALID_HANDLE_VALUE )
			{
				m_hOutputHandle = ::GetStdHandle( ConsoleConstants::WIN_STD_OUTPUT_HANDLE );
				m_hInputHandle = ::GetStdHandle( ConsoleConstants::WIN_STD_INPUT_HANDLE );

				// Fix input mode as it defaults to 439, which is bigger then all possible flags combined...
				if( m_hInputHandle != INVALID_HANDLE_VALUE ) ::SetConsoleMode( m_hInputHandle, ENABLE_PROCESSED_INPUT );
			}
		#elif PLATFORM_MAC
		#elif PLATFORM_LINUX
		#else
			#error You shall not pass!
		#endif
	}

	bool FServerConsole::IsShown()
	{
		return m_pConsole->IsShown();
	}

	bool FServerConsole::IsAttached()
	{
		return m_pConsole->IsAttached();
	}

	void FServerConsole::Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory, const double fTime )
	{
		FScopeLock hLock( &m_hLock );

		#if PLATFORM_WINDOWS
			COORD hCursorPosition = GetCursorPosition();
		#endif

		ClearInputLine();

		m_pConsole->Serialize( sData, eVerbosity, sCategory, fTime );

		RedrawInputLine();

		#if PLATFORM_WINDOWS
			hCursorPosition.Y = GetCursorPosition().Y;

			SetCursorPosition( hCursorPosition );
		#endif
	}

	void FServerConsole::Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory )
	{
		Serialize( sData, eVerbosity, sCategory, -1.0 );
	}
#endif
