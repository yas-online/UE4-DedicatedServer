// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN( LogServerConsole, Log, All );

#if WITH_SERVER_CODE
	#if PLATFORM_WINDOWS
		#include "Private/Windows/WindowsPlatformOutputDevicesPrivate.h"
		#define FOutputDeviceConsolePlatform FOutputDeviceConsoleWindows
	#elif PLATFORM_MAC
		#include "Private/Mac/MacPlatformOutputDevicesPrivate.h"
		#define FOutputDeviceConsolePlatform FOutputDeviceConsoleMac
	#elif PLATFORM_LINUX
		#include "Private/Linux/LinuxPlatformOutputDevicesPrivate.h"
		#define FOutputDeviceConsolePlatform FOutputDeviceConsoleLinux
	#else
		#error How did you get here?
	#endif

	class FServerConsole : public FOutputDeviceConsolePlatform
	{
		public:
			FServerConsole();
			virtual ~FServerConsole();

			void Tick();
			void SendNullInput();

			void Show( bool bShowWindow ) override;
			bool IsShown() override;
			bool IsAttached() override;

			void Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory, const double fTime ) override;
			void Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory ) override;

		protected:
			void ClearInputLine();
			void RedrawInputLine();

			#if PLATFORM_WINDOWS
				COORD								GetCursorPosition();
				bool								SetCursorPosition( COORD hCursorPosition );
			#endif

		private:
			FOutputDeviceConsolePlatform*			m_pConsole;
			FString									m_sInput;
			FString									m_sUserInput;
			TArray<FString>							m_hCommandHistory;
			int32									m_iCommandHistoryIndex;

			mutable FCriticalSection				m_hLock;

			#if PLATFORM_WINDOWS
				HANDLE								m_hOutputHandle;
				HANDLE								m_hInputHandle;

				INPUT_RECORD						m_hCachedInputEvent;
			#endif
	};
#endif
