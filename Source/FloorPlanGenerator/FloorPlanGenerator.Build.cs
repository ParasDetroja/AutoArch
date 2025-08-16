using UnrealBuildTool;

public class FloorPlanGenerator : ModuleRules
{
    public FloorPlanGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
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
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "ToolMenus",
                "Slate",
                "SlateCore",
                "ProceduralMeshComponent",
                "MeshDescription",
                "StaticMeshDescription"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "ToolMenus",
                "ContentBrowser",
                "AssetTools",
                "ProceduralMeshComponent",
                "MeshDescription",
                "StaticMeshDescription"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}
