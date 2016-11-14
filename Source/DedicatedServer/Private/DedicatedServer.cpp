// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#include "DedicatedServerPrivatePCH.h"
#include "DedicatedServer.h"

DEFINE_LOG_CATEGORY( LogDedicatedServer );

TScopedPointer<FServerConsole> g_pConsole;

void FDedicatedServerModule::StartupModule()
{
	#if WITH_SERVER_CODE
		if( IsRunningDedicatedServer() )
		{
			bool bUseConsole = FParse::Param( FCommandLine::Get(), TEXT( "console" ) );

			if( bUseConsole && !TIsSame<decltype( GLogConsole ), FServerConsole>::Value )
			{
				g_pConsole = new FServerConsole();

				if( g_pConsole.IsValid() )
				{
					g_pConsole->Show( true );

					GLog->RemoveOutputDevice( GLogConsole );

					GLogConsole = g_pConsole.GetOwnedPointer();

					GLog->AddOutputDevice( GLogConsole );
				}

				m_hTick = Async<void>( EAsyncExecution::Thread, [this]() -> void
				{
					while( !m_bShutdown )
					{
						if( g_pConsole.IsValid() && g_pConsole->IsShown() ) g_pConsole->Tick();
					}

				} );
			}
		}
	#endif
}

void FDedicatedServerModule::ShutdownModule()
{
	m_bShutdown = true;

	#if WITH_SERVER_CODE
		if( g_pConsole.IsValid() && m_hTick.IsValid() )
		{
			g_pConsole->SendNullInput();

			m_hTick.Wait();
		}
	#endif
}

IMPLEMENT_MODULE( FDedicatedServerModule, DedicatedServer )
