// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FRConModule : public IModuleInterface
{
	public:
		/** IModuleInterface implementation */
		void StartupModule() override;
		void ShutdownModule() override;
};
