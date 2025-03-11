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





void FOceanCommand::RegisterCommands()
{
	UI_COMMAND(CommandA, "OceanCommandA", "Execute Ocean CommandA", EUserInterfaceActionType::Button, FInputGesture());
}




void FoceanbodyModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("oceanbody"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/oceanbody/Shaders"), PluginShaderDir);
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FoceanbodyModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

void FoceanbodyModule::InitializeWaterbody(FWaterbodyParameters& WaterBody)
{
	WaterBody.water_depth = 200;
	WaterBody.fog_density = 1.f;
	WaterBody.zeta = 0.75;     // 前向散射的比例
	WaterBody.gf = 0.62;       // 前向散射HG相位函数中的g，范围[0, 1]
	WaterBody.gb = -0.2;      // 后向散射HG相位函数中的g，范围[-1, 0]
	WaterBody.hdr_exposure = 10;
	WaterBody.C = 0.01;
	WaterBody.absorbtion_d_400nm = 0.0f;
	WaterBody.absorbtion_y_440nm = 0.0f;

	
	WaterBody.CDOM_absorbtion = FVector3f(0.034735f, 0.214381f, 1.0f) * WaterBody.absorbtion_y_440nm;
	WaterBody.minerals_absorbtion = FVector3f(0.045959f, 0.192050f, 0.644036f) * WaterBody.absorbtion_d_400nm;
	WaterBody.minerals_scattering = FVector3f(0.0635f, 0.075f, 0.09f);
	WaterBody.phytoplankton_absorbtion = FVector3f(0.015f, 0.01f, 0.035f) * WaterBody.C;
	WaterBody.phytoplankton_scattering = FVector3f(0.24264f, 0.3f, 0.375f) * FMath::Pow(WaterBody.C, 0.62f);
	WaterBody.pure_water_absorbtion = FVector3f(0.45f, 0.0638f, 0.0145f);
	WaterBody.pure_water_scattering = FVector3f(0.0007f, 0.0015f, 0.0038f);    // from Optical properties of the clearest natural waters

	WaterBody.total_scattering = WaterBody.pure_water_scattering + WaterBody.minerals_scattering + WaterBody.phytoplankton_scattering;
	WaterBody.total_absorbtion = WaterBody.pure_water_absorbtion + WaterBody.minerals_absorbtion + WaterBody.phytoplankton_absorbtion + WaterBody.CDOM_absorbtion;
	WaterBody.total_extinction = WaterBody.total_scattering + WaterBody.total_absorbtion;
}

void FoceanbodyModule::UpdateWaterbody(FWaterbodyParameters& WaterBody)
{
	
	WaterBody.CDOM_absorbtion = FVector3f(0.034735f, 0.214381f, 1.0f) * WaterBody.absorbtion_y_440nm;
	WaterBody.minerals_absorbtion = FVector3f(0.045959f, 0.192050f, 0.644036f) * WaterBody.absorbtion_d_400nm;
	WaterBody.phytoplankton_absorbtion = FVector3f(0.015f, 0.01f, 0.035f) * WaterBody.C;
	WaterBody.phytoplankton_scattering = FVector3f(0.24264f, 0.3f, 0.375f) * FMath::Pow(WaterBody.C, 0.62f);

	
	WaterBody.total_scattering = WaterBody.pure_water_scattering + WaterBody.minerals_scattering + WaterBody.phytoplankton_scattering;
	WaterBody.total_absorbtion = WaterBody.pure_water_absorbtion + WaterBody.minerals_absorbtion + WaterBody.phytoplankton_absorbtion + WaterBody.CDOM_absorbtion;
	WaterBody.total_extinction = WaterBody.total_scattering + WaterBody.total_absorbtion;
	
}


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

void FoceanbodyModule::EnqueueRenderCommand(FRDGTextureRef RenderTarget, FRDGTextureRef RenderTarget_d, FWaterbodyParameters WaterBody,FRDGBuilder& GraphBuilder)
{
	TShaderMapRef<FOceanCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	
	ComputeShader->BuildAndExecuteGraph(
				GraphBuilder,
				WaterBody,
				RenderTarget,
				RenderTarget_d
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

void FoceanbodyModule::EnqueueRenderCommand_m(FRDGTextureRef ScatteringDensityLUTTexture, FRHISamplerState* SamplerState,
	FRDGTextureRef RenderTarget,FWaterbodyParameters WaterBody,FRDGBuilder& GraphBuilder)
{
	TShaderMapRef<FOceanCS_m> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	//TUniformBufferRef<FWaterbodyParameters>& UniformBuffer = getUniform(WaterBody);
	FRHISamplerState* SamplerState_Param = SamplerState;
	ComputeShader->BuildAndExecuteGraph(
				GraphBuilder,
				WaterBody,
				ScatteringDensityLUTTexture,
				SamplerState_Param,
				RenderTarget
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

void FoceanbodyModule::EnqueueRenderCommand_d(FRDGTextureRef ScatteringLUT_InputTexture, FRHISamplerState* SamplerState,
	FRDGTextureRef RenderTarget, FWaterbodyParameters WaterBody,FRDGBuilder& GraphBuilder)
{
	TShaderMapRef<FOceanCS_d> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	
	//TUniformBufferRef<FWaterbodyParameters>& UniformBuffer = getUniform(WaterBody);
	ComputeShader->BuildAndExecuteGraph(
				GraphBuilder,
				WaterBody,
				ScatteringLUT_InputTexture,
				SamplerState,
				RenderTarget
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

void FoceanbodyModule::EnqueueRenderCommand_s(const TArray<FRDGTextureRef>& MultiScatteringLUTsTexture,
	const TArray<FRDGTextureRef>& ScatteringDensityLUTsTexture, const TArray<FRHISamplerState*>& MultiSamplerState,
	const TArray<FRHISamplerState*>& DensitySamplerState, UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_m,FRDGBuilder& GraphBuilder)
{
	TShaderMapRef<FOceanCS_s> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	ComputeShader->BuildAndExecuteGraph(
				GraphBuilder,
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