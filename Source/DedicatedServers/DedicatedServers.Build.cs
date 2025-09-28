using UnrealBuildTool;

public class DedicatedServers : ModuleRules
{
	public DedicatedServers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        	//PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PhysicsCore", "GameLiftServerSDK" });
        	PublicDependencyModuleNames.AddRange(new string[]
	        {
		        "Core", 
		        "CoreUObject", 
		        "Engine", 
		        "GameLiftServerSDK",
		        "GameplayTags",
				"HTTP", //this is the module to use FHttpModule::Get() and so on
				"Json",
				"JsonUtilities", //I don't see anything being used in this module so far
				"NetCore"
	        });

		//PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "Slate", "SlateCore" });
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate", 
			"SlateCore",
			"UMG"  //stephen confident putting it here lol (with some risk to reduce compile time)
		});
	}
}
