using UnrealBuildTool;

public class DedicatedServers : ModuleRules
{
	public DedicatedServers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        	//PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PhysicsCore", "GameLiftServerSDK" });
        	PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GameLiftServerSDK"});

		//PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "Slate", "SlateCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
