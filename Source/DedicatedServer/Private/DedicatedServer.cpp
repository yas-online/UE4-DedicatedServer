// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#include "DedicatedServerPrivatePCH.h"
#include "DedicatedServer.h"

DEFINE_LOG_CATEGORY( LogDedicatedServer );

// Note: this is guesswork as you can't really reliably get a version string out of a integer (god damn it valve, why not use the input string for UpToDateCheck as well?)
FString Str2SemVer( const FString &sVersion )
{
	size_t iDigits = sVersion.Len();

	/*if( iDigits == 2 ) return FString::Printf( TEXT( "%c.%c" ), sVersion[0], sVersion[1] );
	else*/ if( iDigits == 3 || iDigits == 4 ) return FString::Printf( TEXT( "%c.%c.%c.%c" ), sVersion[0], sVersion[1], sVersion[2], ( iDigits == 4 ? sVersion[3] : 0 ) );
	// Everyting from 5 to 7 digits is not determinateable, it'll fallback to raw output below
	else if( iDigits == 8 ) return FString::Printf( TEXT( "%c%c.%c%c.%c%c.%c%c" ), sVersion[0], sVersion[1], sVersion[2], sVersion[3], sVersion[4], sVersion[5], sVersion[6], sVersion[7] );

	return sVersion;
}

TUniquePtr<FServerConsole> g_pConsole;

void FDedicatedServerModule::StartupModule()
{
	#if WITH_SERVER_CODE
		if( IsRunningDedicatedServer() )
		{
			bool bUseConsole = FParse::Param( FCommandLine::Get(), TEXT( "console" ) );

			if( bUseConsole && !TIsSame<decltype( GLogConsole ), FServerConsole>::Value )
			{
				g_pConsole = TUniquePtr<FServerConsole>( new FServerConsole() );

				if( g_pConsole.IsValid() )
				{
					g_pConsole->Show( true );

					GLog->RemoveOutputDevice( GLogConsole );

					GLogConsole = g_pConsole.Get();

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

			bool bUseAutoUpdate = FParse::Param( FCommandLine::Get(), TEXT( "autoupdate" ) );
			if( bUseAutoUpdate )
			{
				FString sVersion = GConfig->GetStr( TEXT( "OnlineSubsystemSteam" ), TEXT( "GameVersion" ), GEngineIni );
				IOnlineSubsystem* pSteam = IOnlineSubsystem::Get( STEAM_SUBSYSTEM );

				m_hAutoUpdateTicker = FTicker::GetCoreTicker().AddTicker( FTickerDelegate::CreateLambda( [=]( float fDeltaTime ) -> bool
				{
					UE_LOG( LogDedicatedServer, Display, TEXT( "Checking for updates..." ) );

					// ToDo: Do we have alternatives to the Steam API check?
					if( pSteam )
					{
						TSharedRef<class IHttpRequest> hHTTPRequest = FHttpModule::Get().CreateRequest();

						// Note: UpToDateCheck is expecting a uint32 where the rest of Steamworks uses a semver / integer string, so we have to convert it here...
						hHTTPRequest->SetURL( FString::Printf( TEXT( "https://api.steampowered.com/ISteamApps/UpToDateCheck/v0001/?appid=%s&version=%s" ), *pSteam->GetAppId(), *sVersion.Replace( TEXT( "." ), TEXT( "" ) ) ) );
						hHTTPRequest->SetVerb( TEXT( "GET" ) );
						hHTTPRequest->OnProcessRequestComplete().BindLambda( [=]( FHttpRequestPtr pHTTPRequest, FHttpResponsePtr pHTTPResponse, bool bSucceeded ) -> void
						{
							if( bSucceeded && pHTTPResponse->GetResponseCode() == EHttpResponseCodes::Ok )
							{
								TSharedPtr<FJsonObject> pJSONObject;
								TSharedRef<TJsonReader<>> hJSONReader = TJsonReaderFactory<>::Create( pHTTPResponse->GetContentAsString() );
								if( FJsonSerializer::Deserialize( hJSONReader, pJSONObject ) && pJSONObject.IsValid() )
								{
									TSharedPtr<FJsonObject> pResponse = pJSONObject->GetObjectField( TEXT( "response" ) );
									if( pResponse.IsValid() && pResponse->GetBoolField( TEXT( "success" ) ) )
									{
										if( !pResponse->GetBoolField( TEXT( "up_to_date" ) ) )
										{
											// ToDo: find a better way which also respects the message field...
											FString sRequiredVersion = FString::FromInt( pResponse->GetIntegerField( TEXT( "required_version" ) ) );
											UE_LOG( LogDedicatedServer, Warning, TEXT( "The server is outdated( %s -> %s ), restarting..." ), *sVersion, ( sRequiredVersion.Equals( sVersion ) ? *sRequiredVersion : *Str2SemVer( sRequiredVersion ) )/*, pResponse->GetStringField( "message" )*/ );
											GIsRequestingExit = true;
											m_bShutdown = true;
										}
										else UE_LOG( LogDedicatedServer, Display, TEXT( "The server is using the most recent version( %s )" ), *sVersion );
									}
								}
							}
						} );

						hHTTPRequest->ProcessRequest();
					}

					if( m_bShutdown ) return false;
					return true;
				} ), 60.0f );
			}
		}
	#endif
}

void FDedicatedServerModule::ShutdownModule()
{
	m_bShutdown = true;

	#if WITH_SERVER_CODE
		if( m_hAutoUpdateTicker.IsValid() ) FTicker::GetCoreTicker().RemoveTicker( m_hAutoUpdateTicker );

		if( g_pConsole.IsValid() && m_hTick.IsValid() )
		{
			g_pConsole->SendNullInput();

			m_hTick.Wait();
		}
	#endif
}

IMPLEMENT_MODULE( FDedicatedServerModule, DedicatedServer )
