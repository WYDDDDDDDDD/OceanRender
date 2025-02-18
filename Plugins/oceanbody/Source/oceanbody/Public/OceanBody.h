// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include"oceanComputeShader.h"

#define OCEANBODYMODULE_API DLLEXPORT
class OCEANBODYMODULE_API FoceanbodyModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//void CreateToolbarButton();
	void OnGenerateTextureClicked();
	void EnqueueRenderCommand(UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_d, FWaterbodyParameters WaterBody,FRHICommandListImmediate& RHICmdList);
	void EnqueueRenderCommand_m(FRHITexture* ScatteringDensityLUTTexture, FRHISamplerState* SamplerState, UTextureRenderTargetVolume* RenderTarget,FWaterbodyParameters WaterBody,FRHICommandListImmediate& RHICmdList);
	void EnqueueRenderCommand_d(FRHITexture* ScatteringLUT_InputTexture, FRHISamplerState* SamplerState, UTextureRenderTargetVolume* RenderTarget,FWaterbodyParameters WaterBody,FRHICommandListImmediate& RHICmdList);
	void EnqueueRenderCommand_s(const TArray<FRHITexture*> &MultiScatteringLUTsTexture, const TArray<FRHITexture*> &ScatteringDensityLUTsTexture, const TArray<FRHISamplerState*> &MultiSamplerState,
		const TArray<FRHISamplerState*> &DensitySamplerState,  UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_m,FRHICommandListImmediate& RHICmdList);
private:
};
struct CommandInfo
{
	TSharedPtr< FUICommandInfo > GenerateTexture;
};
class FOceanCommand : public TCommands<FOceanCommand>
{
public:
	
	FOceanCommand() : TCommands<FOceanCommand>(
	"OceanCommand",
	NSLOCTEXT("Contexts", "OceanCommand", "Ocean Plugin"),
	NAME_None,
	FName(*FString("todo")))
	{
	}

	// TCommands<>的接口：注册命令
	virtual void RegisterCommands() override;
	
public:
	//命令A
	TSharedPtr<class FUICommandInfo > CommandA;
};
