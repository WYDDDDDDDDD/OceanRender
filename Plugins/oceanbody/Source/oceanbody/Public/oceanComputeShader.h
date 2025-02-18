#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include"DataDrivenShaderPlatformInfo.h"
#include"ShaderParameterMacros.h"
#include"Engine/TextureRenderTargetVolume.h"
#define  NUM_SCATTERING_ORDERS  4

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FWaterbodyParameters,)
	SHADER_PARAMETER(float, water_depth)
	SHADER_PARAMETER(float, fog_density)
	SHADER_PARAMETER(float, zeta)
	SHADER_PARAMETER(float, gf)
	SHADER_PARAMETER(float, gb)
	SHADER_PARAMETER(float, hdr_exposure)
	SHADER_PARAMETER(FVector3f, pure_water_scattering)
	SHADER_PARAMETER(FVector3f, minerals_scattering)
	SHADER_PARAMETER(FVector3f, phytoplankton_scattering)
	SHADER_PARAMETER(FVector3f, pure_water_absorbtion)
	SHADER_PARAMETER(FVector3f, minerals_absorbtion)
	SHADER_PARAMETER(FVector3f, phytoplankton_absorbtion)
	SHADER_PARAMETER(FVector3f, CDOM_absorbtion)
	SHADER_PARAMETER(FVector3f, total_scattering)
	SHADER_PARAMETER(FVector3f, total_absorbtion)
	SHADER_PARAMETER(FVector3f, total_extinction)

END_GLOBAL_SHADER_PARAMETER_STRUCT()


class FOceanCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FOceanCS)
	SHADER_USE_PARAMETER_STRUCT(FOceanCS, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_STRUCT_REF(FWaterbodyParameters, waterbody)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture3D<FVector4f>, SingleScatteringLUT)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture3D<FVector4f>, SingleScatteringDensityLUT)

		// SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FVector>, Vertices)
		//
		// SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<FVector4>, OutputTexture)

	END_SHADER_PARAMETER_STRUCT()
	// static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
	//    FShaderCompilerEnvironment&OutEnvironment)
	//    {
	//    	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	//    	
	//    }

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
	}
	void BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList,FWaterbodyParameters WaterBody, UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_d);
};

class FOceanCS_m : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FOceanCS_m)
	SHADER_USE_PARAMETER_STRUCT(FOceanCS_m, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		//SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_STRUCT_REF(FWaterbodyParameters, waterbody)
		SHADER_PARAMETER_TEXTURE(Texture3D, ScatteringDensityLUT)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler_ScatteringDensityLUT)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture3D<FVector4f>, ScatteringLUT_Output)
	END_SHADER_PARAMETER_STRUCT()
	// static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
	//    FShaderCompilerEnvironment&OutEnvironment)
	//    {
	//    	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	//    	
	//    }

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
	}

	void BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList,FWaterbodyParameters WaterBody, FRHITexture* ScatteringDensityLUTTexture,
	                          FRHISamplerState* SamplerState, UTextureRenderTargetVolume* RenderTarget);
	
};
class FOceanCS_d : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FOceanCS_d)
	SHADER_USE_PARAMETER_STRUCT(FOceanCS_d, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		//SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_STRUCT_REF(FWaterbodyParameters, waterbody)
		SHADER_PARAMETER_TEXTURE(Texture3D, ScatteringLUT_Input)
		SHADER_PARAMETER_SAMPLER(SamplerState, sampler_ScatteringLUT_Input)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture3D<FVector4f>, ScatteringDensityLUT_Output)
	END_SHADER_PARAMETER_STRUCT()
	// static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
	//    FShaderCompilerEnvironment&OutEnvironment)
	//    {
	//    	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	//    	
	//    }

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
	}

	void BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList, FWaterbodyParameters WaterBody, FRHITexture* ScatteringLUT_InputTexture,
							  FRHISamplerState* SamplerState, UTextureRenderTargetVolume* RenderTarget);
	
};
class FOceanCS_s : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FOceanCS_s)
	SHADER_USE_PARAMETER_STRUCT(FOceanCS_s, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			// Texture3D 数组
		SHADER_PARAMETER_TEXTURE_ARRAY(Texture3D, MultiScatteringLUTs, [NUM_SCATTERING_ORDERS])
		SHADER_PARAMETER_TEXTURE_ARRAY(Texture3D, ScatteringDensityLUTs, [NUM_SCATTERING_ORDERS])
		// SamplerState 数组
		SHADER_PARAMETER_SAMPLER_ARRAY(SamplerState, sampler_MultiScatteringLUTs, [NUM_SCATTERING_ORDERS])
		SHADER_PARAMETER_SAMPLER_ARRAY(SamplerState, sampler_ScatteringDensityLUTs, [NUM_SCATTERING_ORDERS])

		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture3D<FVector4f>, ScatteringLUT)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture3D<FVector4f>, MultiScatteringDensityLUT)
	
	END_SHADER_PARAMETER_STRUCT()
	// static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
	//    FShaderCompilerEnvironment&OutEnvironment)
	//    {
	//    	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	//    	
	//    }

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
	}

	void BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList, const TArray<FRHITexture*> &MultiScatteringLUTsTexture,
		const TArray<FRHITexture*> &ScatteringDensityLUTsTexture, const TArray<FRHISamplerState*> &MultiSamplerState,
		const TArray<FRHISamplerState*> &DensitySamplerState,  UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_m );
	
};
