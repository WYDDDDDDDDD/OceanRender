// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using UnrealBuildTool;

public class ocean1 : ModuleRules
{
	private string ModulePath
	{
		get
		{
			return ModuleDirectory;
		}
 
	}
	private string ProjectSourcePath
	{
		get
		{
			return Path.GetFullPath(Path.Combine(ModulePath, ".."));
		}
	}
	private string ThirdPartyPath
	{
		get
		{
			return Path.GetFullPath(Path.Combine(ProjectSourcePath, "ThirdParty"));
		}

	}
	

	public ocean1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseRTTI = true;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" ,"ProceduralMeshComponent","Slate", "SlateCore"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		//Type = ModuleType.External;
 
		// 添加需要设置的宏
		//PublicDefinitions.Add("WITH_MYTHIRDPARTYLIBRARY=1");
 
		// 添加插件的包含路径
		

		bEnableUndefinedIdentifierWarnings = false;
		PublicIncludePaths.Add(Path.Combine( "D:/GDAL322", "include"));
		PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));

// 添加导入库或静态库
		PublicAdditionalLibraries.Add(Path.Combine("D:/GDAL322", "lib", "gdal_i.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(Path.Combine(ThirdPartyPath, "lib",  "openvdb.lib") ));
		PublicAdditionalLibraries.Add(Path.Combine(Path.Combine(ThirdPartyPath, "lib",  "tbb12.lib") ));

// 延迟加载的 DLL 名称
		//PublicDelayLoadDLLs.Add("gdal.dll");
		//PublicDelayLoadDLLs.Add("C:\\Program Files\\OpenVDB\\bin\\openvdb.dll");

// 确保 DLL 文件位于系统路径中，或者使用以下代码在运行时设置 DLL 搜索路径
		//RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "gdal.dll"));

	}
}
