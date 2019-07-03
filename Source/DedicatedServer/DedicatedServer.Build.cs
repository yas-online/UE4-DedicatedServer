// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class DedicatedServer : ModuleRules
{
	public DedicatedServer( ReadOnlyTargetRules Target ) : base( Target )
	{
		//PublicIncludePaths.AddRange( new string[] { "DedicatedServer/Public" } );
		//PrivateIncludePaths.AddRange( new string[] { "DedicatedServer/Private" } );

		PublicDependencyModuleNames.AddRange( new string[] { "Core", "ApplicationCore", "CoreUObject", "Engine", "InputCore", "HTTP", "Json", "OnlineSubsystem", "OnlineSubsystemSteam" } );
		PrivateDependencyModuleNames.AddRange( new string[] {  } );
	}
}
