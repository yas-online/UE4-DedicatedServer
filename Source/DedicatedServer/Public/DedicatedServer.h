// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

class FDedicatedServerModule : public IModuleInterface
{
	public:
		/** IModuleInterface implementation */
		void StartupModule() override;
		void ShutdownModule() override;

	private:
		bool								m_bShutdown;

		#if WITH_SERVER_CODE
			bool							m_bUseConsole;
			TSharedPtr<FServerConsole>		m_pConsole;

			TFuture<void>					m_hTick;
		#endif
};
