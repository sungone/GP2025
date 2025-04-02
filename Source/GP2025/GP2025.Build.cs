// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class GP2025 : ModuleRules
{
    public GP2025(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "GP2025" });

        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", 
            "Sockets", "Networking", "Json", "JsonUtilities", "PhysicsCore" , "Slate" , "SlateCore" ,
            "NavigationSystem" , "Niagara"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
