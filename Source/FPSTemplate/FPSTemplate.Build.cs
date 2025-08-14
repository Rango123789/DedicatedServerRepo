// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSTemplate : ModuleRules
{
	public FPSTemplate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //GameLiftServerSDK is nested in Plugins/GameLiftPlugin/Source/GameLiftServer/Source/SourceGameLiftServerSDK that is needed, but I guess add the bigger will auto-add to smaller too? I guess so but it will be less effective right? yeah!
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PhysicsCore", "DedicatedServers" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "Slate", "SlateCore" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
