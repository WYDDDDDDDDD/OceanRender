// Copyright Epic Games, Inc. All Rights Reserved.

#include "oceanbody.h"
#include"oceanComputeShader.h"
#include "Interfaces/IPluginManager.h"
#include "Engine/VolumeTexture.h"
#include"LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/Commands.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorToolkit.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateStyle.h"


#define LOCTEXT_NAMESPACE "FoceanbodyModule"

//FShaderParametersMetadata FWaterbodyParameters::StaticStructMetadata;




void FOceanCommand::RegisterCommands()
{
	UI_COMMAND(CommandA, "OceanCommandA", "Execute Ocean CommandA", EUserInterfaceActionType::Button, FInputGesture());
}




void FoceanbodyModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("oceanbody"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/oceanbody/Shaders"), PluginShaderDir);
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	//FOceanCommand::Register();
	//CreateToolbarButton();
}

void FoceanbodyModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	//FOceanCommand::Unregister();
}

// void FoceanbodyModule::CreateToolbarButton()
// {
// 	
// 	// const ISlateStyle* CoreStyle2 = &FCoreStyle::GetCoreStyle();
// 	//  const ISlateStyle& CoreStyle1 =FCoreStyle::GetCoreStyle();
// 	// const FName CoreStyle = CoreStyle1.GetStyleSetName() ;
// 	// if (CoreStyle2&&CoreStyle!="" )
// 	// {
// 	// 	UE_LOG(LogTemp, Log, TEXT("CoreStyle is available."));
// 	// }
// 	// else
// 	// {
// 	// 	UE_LOG(LogTemp, Warning, TEXT("CoreStyle is not available."));
// 	// }
// 	
// 	// Bind command
// 	
//  //-------------------------OnGenerateTextureClicked的按钮————————————————————————————————————————————————————————————————————
// 	// 注册一个命令 (例如，点击按钮时执行的函数)
// 	CommandList = MakeShareable(new FUICommandList);
// 	CommandList->MapAction(
// 		FOceanCommand::Get().CommandA,
// 		FExecuteAction::CreateRaw(this, &FoceanbodyModule::OnGenerateTextureClicked),
// 		FCanExecuteAction()
// 	);
// 	
// 	// 获取 LevelEditor 模块
// 	
// 	  //FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor");
//
// 	
// 	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
//  
// 	ToolbarExtender->AddToolBarExtension(
// 		"Content",
// 		EExtensionHook::After,
// 		CommandList,
// 		FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& ToolbarBuilder)
// 		{
// 			ToolbarBuilder.AddToolBarButton(
// 			   FUIAction(
// 				   FExecuteAction::CreateRaw(this, &FoceanbodyModule::OnGenerateTextureClicked)
// 			   ),
// 			   NAME_None,
// 			   FText::FromString(TEXT("Generate Texture")),
// 			   FText::FromString(TEXT("Generates the texture for the shader")),
// 			   FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Build")
// 		   );
// 		})
// 	);
// 	FLevelEditorModule* pLevelEditorModule =FModuleManager::GetModulePtr<FLevelEditorModule>(FName(TEXT("LevelEditor")));
// 	//FLevelEditorModule& pLevelEditorModule =FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
// 	pLevelEditorModule->GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
//
// 	
// }

 void FoceanbodyModule::OnGenerateTextureClicked()
{
	// 获取并加载纹理的代码
	FSoftObjectPath TexturePath(TEXT("Texture3D'/Content/Resources/SingleScatteringLUT.raw'"));
	UTextureRenderTargetVolume* RenderTarget = Cast<UTextureRenderTargetVolume>(TexturePath.TryLoad());
	FSoftObjectPath Texture_dPath(TEXT("Texture3D'/Content/Resources/SingleScatteringDensityLUT.raw'"));
	UTextureRenderTargetVolume* RenderTarget_d = Cast<UTextureRenderTargetVolume>(TexturePath.TryLoad());

	if (RenderTarget&&RenderTarget_d)
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded Texture"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load texture"));
	}
	//EnqueueRenderCommand(RenderTarget, RenderTarget_d);
	UE_LOG(LogTemp, Log, TEXT("Shader Complete"));
}

void FoceanbodyModule::EnqueueRenderCommand(UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_d, FWaterbodyParameters WaterBody,FRHICommandListImmediate& RHICmdList)
{
	TShaderMapRef<FOceanCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	UTextureRenderTargetVolume* RenderTargetParam = RenderTarget;
	UTextureRenderTargetVolume* RenderTarget_dParam = RenderTarget_d;
	ComputeShader->BuildAndExecuteGraph(
				RHICmdList,
				WaterBody,
				RenderTargetParam,
				RenderTarget_dParam
				);

	// ENQUEUE_RENDER_COMMAND(ComputeShader)(
	// 	[
	// 		ComputeShader,
	// 		WaterBody,
	// 		RenderTargetParam,
	// 		RenderTarget_dParam
	// 	](FRHICommandListImmediate& RHICmdList)
	// 	{
	// 		ComputeShader->BuildAndExecuteGraph(
	// 			RHICmdList,
	// 			WaterBody,
	// 			RenderTargetParam,
	// 			RenderTarget_dParam
	// 			);
	// 	});
}

void FoceanbodyModule::EnqueueRenderCommand_m(FRHITexture* ScatteringDensityLUTTexture, FRHISamplerState* SamplerState,
	UTextureRenderTargetVolume* RenderTarget,FWaterbodyParameters WaterBody,FRHICommandListImmediate& RHICmdList)
{
	TShaderMapRef<FOceanCS_m> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	UTextureRenderTargetVolume* RenderTargetParam = RenderTarget;
	FRHITexture* ScatteringDensityLUTTexture_Param = ScatteringDensityLUTTexture;
	//TUniformBufferRef<FWaterbodyParameters>& UniformBuffer = getUniform(WaterBody);
	FRHISamplerState* SamplerState_Param = SamplerState;
	ComputeShader->BuildAndExecuteGraph(
				RHICmdList,
				WaterBody,
				ScatteringDensityLUTTexture_Param,
				SamplerState_Param,
				RenderTargetParam
				);
// 	ENQUEUE_RENDER_COMMAND(ComputeShader)(
// 		[
// 			ComputeShader,
// 			WaterBody,
// 			ScatteringDensityLUTTexture_Param,
// 			SamplerState_Param,
// 			RenderTargetParam
// 		](FRHICommandListImmediate& RHICmdList)
// 		{
// 			ComputeShader->BuildAndExecuteGraph(
// 				RHICmdList,
// 				WaterBody,
// 				ScatteringDensityLUTTexture_Param,
// 				SamplerState_Param,
// 				RenderTargetParam
// 				);
// 		});
 }

void FoceanbodyModule::EnqueueRenderCommand_d(FRHITexture* ScatteringLUT_InputTexture, FRHISamplerState* SamplerState,
	UTextureRenderTargetVolume* RenderTarget, FWaterbodyParameters WaterBody,FRHICommandListImmediate& RHICmdList)
{
	TShaderMapRef<FOceanCS_d> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	UTextureRenderTargetVolume* RenderTargetParam = RenderTarget;
	FRHITexture* ScatteringLUT_InputTexture_Param = ScatteringLUT_InputTexture;
	//TUniformBufferRef<FWaterbodyParameters>& UniformBuffer = getUniform(WaterBody);
	ComputeShader->BuildAndExecuteGraph(
				RHICmdList,
				WaterBody,
				ScatteringLUT_InputTexture_Param,
				SamplerState,
				RenderTargetParam
				);
	// ENQUEUE_RENDER_COMMAND(ComputeShader)(
	// 	[
	// 		ComputeShader,
	// 		WaterBody,
	// 		ScatteringLUT_InputTexture_Param,
	// 		SamplerState,
	// 		RenderTargetParam
	// 	](FRHICommandListImmediate& RHICmdList)
	// 	{
	// 		ComputeShader->BuildAndExecuteGraph(
	// 			RHICmdList,
	// 			WaterBody,
	// 			ScatteringLUT_InputTexture_Param,
	// 			SamplerState,
	// 			RenderTargetParam
	// 			);
	// 	});
	
}

void FoceanbodyModule::EnqueueRenderCommand_s(const TArray<FRHITexture*>& MultiScatteringLUTsTexture,
	const TArray<FRHITexture*>& ScatteringDensityLUTsTexture, const TArray<FRHISamplerState*>& MultiSamplerState,
	const TArray<FRHISamplerState*>& DensitySamplerState, UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_m,FRHICommandListImmediate& RHICmdList)
{
	TShaderMapRef<FOceanCS_s> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	ComputeShader->BuildAndExecuteGraph(
				RHICmdList,
				MultiScatteringLUTsTexture,
				ScatteringDensityLUTsTexture,
				MultiSamplerState,
				DensitySamplerState,
				RenderTarget,
				RenderTarget_m
				);

	// ENQUEUE_RENDER_COMMAND(ComputeShader)(
	// 	[
	// 		ComputeShader,
	// 		MultiScatteringLUTsTexture,
	// 		ScatteringDensityLUTsTexture,
	// 		MultiSamplerState,
	// 		DensitySamplerState,
	// 		RenderTarget,
	// 		RenderTarget_m
	// 	](FRHICommandListImmediate& RHICmdList)
	// 	{
	// 		ComputeShader->BuildAndExecuteGraph(
	// 			RHICmdList,
	// 			MultiScatteringLUTsTexture,
	// 			ScatteringDensityLUTsTexture,
	// 			MultiSamplerState,
	// 			DensitySamplerState,
	// 			RenderTarget,
	// 			RenderTarget_m
	// 			);
	// 	});
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FoceanbodyModule, OceanBody)