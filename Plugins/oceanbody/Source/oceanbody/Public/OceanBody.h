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

	void InitializeWaterbody(FWaterbodyParameters& WaterBody);
	void UpdateWaterbody(FWaterbodyParameters& WaterBody);
	//void CreateToolbarButton();
	void OnGenerateTextureClicked();
	void EnqueueRenderCommand(FRDGTextureRef RenderTarget, FRDGTextureRef RenderTarget_d, FWaterbodyParameters WaterBody,FRDGBuilder& GraphBuilder);
	void EnqueueRenderCommand_m(FRDGTextureRef ScatteringDensityLUTTexture, FRHISamplerState* SamplerState, FRDGTextureRef RenderTarget,FWaterbodyParameters WaterBody,FRDGBuilder& GraphBuilder);
	void EnqueueRenderCommand_d(FRDGTextureRef ScatteringLUT_InputTexture, FRHISamplerState* SamplerState, FRDGTextureRef RenderTarget,FWaterbodyParameters WaterBody,FRDGBuilder& GraphBuilder);
	void EnqueueRenderCommand_s(const TArray<FRDGTextureRef> &MultiScatteringLUTsTexture, const TArray<FRDGTextureRef> &ScatteringDensityLUTsTexture, const TArray<FRHISamplerState*> &MultiSamplerState,
		const TArray<FRHISamplerState*> &DensitySamplerState,  UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_m,FRDGBuilder& GraphBuilder);
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
