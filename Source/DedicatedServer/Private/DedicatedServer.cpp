// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#include "DedicatedServerPrivatePCH.h"
#include "DedicatedServer.h"

#define LOCTEXT_NAMESPACE "FDedicatedServerModule"

void FDedicatedServerModule::StartupModule()
{
	#if WITH_SERVER_CODE
		if( IsRunningDedicatedServer() )
		{
			m_bUseConsole = FParse::Param( FCommandLine::Get(), TEXT( "console" ) );

			if( m_bUseConsole && !TIsSame<decltype( GLogConsole ), FServerConsole>::Value )
			{
				m_pConsole = MakeShareable( new FServerConsole() );

				if( m_pConsole.IsValid() )
				{
					m_pConsole->Show( true );

					GLog->RemoveOutputDevice( GLogConsole );

					GLogConsole = m_pConsole.Get();

					GLog->AddOutputDevice( GLogConsole );

					m_hTick = Async<void>( EAsyncExecution::Thread, [this]()
					{
						while( !m_bShutdown ) m_pConsole->Tick();
					} );
				}
			}
		}
	#endif
}

void FDedicatedServerModule::ShutdownModule()
{
	m_bShutdown = true;

	#if WITH_SERVER_CODE
		if( m_bUseConsole )
		{
			m_hTick.Wait();

			m_pConsole.Reset();
		}
	#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FDedicatedServerModule, DedicatedServer )
