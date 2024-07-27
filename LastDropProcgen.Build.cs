// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LastDropProcgen : ModuleRules
{
	public LastDropProcgen(ReadOnlyTargetRules Target) : base(Target)
	{
		// Turn of Unity
		bUseUnity = false;
		
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GeometryFramework",
				"GeometryScriptingCore",
				"ProceduralMeshComponent",
				"RHI",
				"RenderCore",
				"InputCore",
				"UE5Coro"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
