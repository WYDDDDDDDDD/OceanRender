#include "oceanComputeShader.h"
#include "RenderGraph.h"
#include "RenderTargetPool.h"
#include "Engine/VolumeTexture.h"
#include "DataDrivenShaderPlatformInfo.h"
#include"Engine/Texture.h"
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FWaterbodyParameters, "waterbody");
//IMPLEMENT_UNIFORM_BUFFER_STRUCT(FWaterbodyParameters, "WATERBODY");
IMPLEMENT_GLOBAL_SHADER(FOceanCS, "/oceanbody/Shaders/waterbody_scattering_pre.usf", "MainCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FOceanCS_m, "/oceanbody/Shaders/waterbody_multiscattering_pre.usf", "MainCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FOceanCS_d, "/oceanbody/Shaders/waterbody_scattering_density_pre.usf", "MainCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FOceanCS_s, "/oceanbody/Shaders/waterbody_scattering_synthesize_pre.usf", "MainCS", SF_Compute);

//IMPLEMENT_UNIFORM_BUFFER_STRUCT(FWaterbodyParameters, "FWaterbodyParametersdistributed under the License is distributed on an "AS IS" BASIS,");
void FOceanCS::BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList,FWaterbodyParameters WaterBody, UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_d)
{
	// Our main point of contant with the RDG
	// We will use this to add resources (buffers) and passes to the render-graph
	FRDGBuilder GraphBuilder(RHICmdList);
	

	// This is a pointer to the shader-parameters we declared in the .h
	FParameters* PassParameters;
	// We ask the RDG to allocate some memory for our shader-parameters
	PassParameters = GraphBuilder.AllocParameters<FOceanCS::FParameters>();

	PassParameters->waterbody = TUniformBufferRef<FWaterbodyParameters>::CreateUniformBufferImmediate(WaterBody, UniformBuffer_SingleFrame);

	// --- Creating a texture for the compute shader to write to SingleScatteringLUT---
	// 1. Make a texture description 
	FRDGTextureDesc OutTextureDesc = FRDGTextureDesc::Create3D(
		FIntVector(RenderTarget->SizeX, RenderTarget->SizeY, RenderTarget->SizeZ),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_UAV,//unordered view
		1,
		1); 
	// 2. Allocate memory with above desc and get a ref to it
	FRDGTextureRef OutTextureRef = GraphBuilder.CreateTexture(OutTextureDesc, TEXT("SingleScatteringLUT"));
	// 3. Make a UAV description from our Texture Ref
	FRDGTextureUAVDesc OutTextureUAVDesc(OutTextureRef);
	// 4. Initialize it as our OutputTexture in our pass params
	PassParameters->SingleScatteringLUT = GraphBuilder.CreateUAV(OutTextureUAVDesc);

	// --- Creating a texture for the compute shader to write to SingleScatteringDensityLUT---
	
	FRDGTextureDesc OutTexture_dDesc = FRDGTextureDesc::Create3D(
		FIntVector(RenderTarget_d->SizeX, RenderTarget_d->SizeY, RenderTarget_d->SizeZ),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_UAV,//unordered view
		1,
		1); 
	
	FRDGTextureRef OutTexture_dRef = GraphBuilder.CreateTexture(OutTexture_dDesc, TEXT("SingleScatteringDensityLUT"));
	if (!OutTexture_dRef)
	{
		UE_LOG(LogTemp, Error, TEXT("OutTexture_dRef is null!"));
	}
	FRDGTextureUAVDesc OutTexture_dUAVDesc(OutTexture_dRef);
	
	PassParameters->SingleScatteringDensityLUT = GraphBuilder.CreateUAV(OutTexture_dUAVDesc);
	
	// ------ Add the compute pass ------
	// Get a reference to our global shader class
	TShaderMapRef<FOceanCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	// Add the compute shader pass to the render graph
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Compute Pass"), ComputeShader, PassParameters, FIntVector(8, 8, 8));


	// ------ Extracting to pooled render target ------
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget;
	// Copy the result of compute shader from UAV to pooled renderT-target
	GraphBuilder.QueueTextureExtraction(OutTextureRef, &PooledComputeTarget);
	// Execute the graph
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget_d;
	GraphBuilder.QueueTextureExtraction(OutTexture_dRef, &PooledComputeTarget_d);
	// Execute the graph
	GraphBuilder.Execute();
	

	// Queue the UAV we wrote to for extraction 
	RHICmdList.CopyToResolveTarget(PooledComputeTarget.GetReference()->GetRenderTargetItem().TargetableTexture,RenderTarget->GetResource()->TextureRHI , FResolveParams());
	RHICmdList.CopyToResolveTarget(PooledComputeTarget_d.GetReference()->GetRenderTargetItem().TargetableTexture,RenderTarget_d->GetResource()->TextureRHI , FResolveParams());
}
void FOceanCS_m::BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList,FWaterbodyParameters WaterBody, FRHITexture* ScatteringDensityLUTTexture,
							  FRHISamplerState* SamplerState, UTextureRenderTargetVolume* RenderTarget)
{
	// We will use this to add resources (buffers) and passes to the render-graph
	FRDGBuilder GraphBuilder(RHICmdList);
	// This is a pointer to the shader-parameters we declared in the .h
	FParameters* PassParameters;
	// We ask the RDG to allocate some memory for our shader-parameters
	PassParameters = GraphBuilder.AllocParameters<FOceanCS_m::FParameters>();
	PassParameters->waterbody = TUniformBufferRef<FWaterbodyParameters>::CreateUniformBufferImmediate(WaterBody, UniformBuffer_SingleFrame);
	PassParameters->ScatteringDensityLUT = ScatteringDensityLUTTexture;
	SamplerState = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	PassParameters->sampler_ScatteringDensityLUT = SamplerState;
	// 1. Make a texture description 
	FRDGTextureDesc OutTextureDesc = FRDGTextureDesc::Create3D(
		FIntVector(RenderTarget->SizeX, RenderTarget->SizeY, RenderTarget->SizeZ),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_UAV,//unordered view
		1,
		1); 
	// 2. Allocate memory with above desc and get a ref to it
	FRDGTextureRef OutTextureRef = GraphBuilder.CreateTexture(OutTextureDesc, TEXT("ScatteringLUT_Output"));
	// 3. Make a UAV description from our Texture Ref
	FRDGTextureUAVDesc OutTextureUAVDesc(OutTextureRef);
	// 4. Initialize it as our OutputTexture in our pass params
	PassParameters->ScatteringLUT_Output = GraphBuilder.CreateUAV(OutTextureUAVDesc);
	
	// ------ Add the compute pass ------
	// Get a reference to our global shader class
	TShaderMapRef<FOceanCS_m> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	// Add the compute shader pass to the render graph
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Compute_m Pass"), ComputeShader, PassParameters, FIntVector(8, 8, 8));


	// ------ Extracting to pooled render target ------
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget;
	// Copy the result of compute shader from UAV to pooled renderT-target
	GraphBuilder.QueueTextureExtraction(OutTextureRef, &PooledComputeTarget);
	// Execute the graph
	GraphBuilder.Execute();
	

	// Queue the UAV we wrote to for extraction 
	RHICmdList.CopyToResolveTarget(PooledComputeTarget.GetReference()->GetRenderTargetItem().TargetableTexture,RenderTarget->GetResource()->TextureRHI , FResolveParams());
}
void FOceanCS_d::BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList, FWaterbodyParameters WaterBody,FRHITexture* ScatteringLUT_InputTexture,
							  FRHISamplerState* SamplerState, UTextureRenderTargetVolume* RenderTarget)
{
	// We will use this to add resources (buffers) and passes to the render-graph
	FRDGBuilder GraphBuilder(RHICmdList);
	// This is a pointer to the shader-parameters we declared in the .h
	FParameters* PassParameters;
	// We ask the RDG to allocate some memory for our shader-parameters
	PassParameters = GraphBuilder.AllocParameters<FOceanCS_d::FParameters>();
	//PassParameters->WATERBODY = WATERBODY;
	PassParameters->ScatteringLUT_Input = ScatteringLUT_InputTexture;
	PassParameters->waterbody = TUniformBufferRef<FWaterbodyParameters>::CreateUniformBufferImmediate(WaterBody, UniformBuffer_SingleFrame);
	SamplerState = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	PassParameters->sampler_ScatteringLUT_Input = SamplerState;
	// 1. Make a texture description 
	FRDGTextureDesc OutTextureDesc = FRDGTextureDesc::Create3D(
		FIntVector(RenderTarget->SizeX, RenderTarget->SizeY, RenderTarget->SizeZ),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV,//unordered view
		1,
		1); 
	// 2. Allocate memory with above desc and get a ref to it
	FRDGTextureRef OutTextureRef = GraphBuilder.CreateTexture(OutTextureDesc, TEXT("ScatteringDensityLUT_Output"));
	// 3. Make a UAV description from our Texture Ref
	FRDGTextureUAVDesc OutTextureUAVDesc(OutTextureRef);
	// 4. Initialize it as our OutputTexture in our pass params
	PassParameters->ScatteringDensityLUT_Output = GraphBuilder.CreateUAV(OutTextureUAVDesc);
	
	// ------ Add the compute pass ------
	// Get a reference to our global shader class
	TShaderMapRef<FOceanCS_d> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	// Add the compute shader pass to the render graph
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Compute_d Pass"), ComputeShader, PassParameters, FIntVector(8, 8, 8));


	// ------ Extracting to pooled render target ------
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget;
	// Copy the result of compute shader from UAV to pooled renderT-target
	GraphBuilder.QueueTextureExtraction(OutTextureRef, &PooledComputeTarget);
	// Execute the graph
	GraphBuilder.Execute();
	

	// Queue the UAV we wrote to for extraction 
	RHICmdList.CopyToResolveTarget(PooledComputeTarget.GetReference()->GetRenderTargetItem().TargetableTexture,RenderTarget->GetResource()->TextureRHI , FResolveParams());

}
void FOceanCS_s::BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList, const TArray<FRHITexture*> &MultiScatteringLUTsTexture,
		const TArray<FRHITexture*> &ScatteringDensityLUTsTexture, const TArray<FRHISamplerState*> &MultiSamplerState,
		const TArray<FRHISamplerState*> &DensitySamplerState,  UTextureRenderTargetVolume* RenderTarget, UTextureRenderTargetVolume* RenderTarget_m )
{
	// We will use this to add resources (buffers) and passes to the render-graph
	FRDGBuilder GraphBuilder(RHICmdList);
	// This is a pointer to the shader-parameters we declared in the .h
	FParameters* PassParameters;
	// We ask the RDG to allocate some memory for our shader-parameters
	PassParameters = GraphBuilder.AllocParameters<FOceanCS_s::FParameters>();
	for (int32 i = 0; i < NUM_SCATTERING_ORDERS; ++i)
	{
		PassParameters->MultiScatteringLUTs[i] = MultiScatteringLUTsTexture[i];
		PassParameters->ScatteringDensityLUTs[i] = ScatteringDensityLUTsTexture[i];
		PassParameters->sampler_ScatteringDensityLUTs[i] = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
		PassParameters->sampler_MultiScatteringLUTs[i] = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	}
	
	// --- Creating a texture for the compute shader to write to SingleScatteringLUT---
	// 1. Make a texture description 
	FRDGTextureDesc OutTextureDesc = FRDGTextureDesc::Create3D(
		FIntVector(RenderTarget->SizeX, RenderTarget->SizeY, RenderTarget->SizeZ),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_UAV,//unordered view
		1,
		1); 
	// 2. Allocate memory with above desc and get a ref to it
	FRDGTextureRef OutTextureRef = GraphBuilder.CreateTexture(OutTextureDesc, TEXT("ScatteringLUT"));
	// 3. Make a UAV description from our Texture Ref
	FRDGTextureUAVDesc OutTextureUAVDesc(OutTextureRef);
	// 4. Initialize it as our OutputTexture in our pass params
	PassParameters->ScatteringLUT = GraphBuilder.CreateUAV(OutTextureUAVDesc);

	// --- Creating a texture for the compute shader to write to SingleScatteringDensityLUT---
	
	FRDGTextureDesc OutTexture_dDesc = FRDGTextureDesc::Create3D(
		FIntVector(RenderTarget_m->SizeX, RenderTarget_m->SizeY, RenderTarget_m->SizeZ),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_UAV,//unordered view
		1,
		1); 
	
	FRDGTextureRef OutTexture_dRef = GraphBuilder.CreateTexture(OutTexture_dDesc, TEXT("MultiScatteringDensityLUT"));

	FRDGTextureUAVDesc OutTexture_dUAVDesc(OutTexture_dRef);
	
	PassParameters->MultiScatteringDensityLUT = GraphBuilder.CreateUAV(OutTexture_dUAVDesc);
	
	// ------ Add the compute pass ------
	// Get a reference to our global shader class
	TShaderMapRef<FOceanCS_s> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	// Add the compute shader pass to the render graph
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Compute_s Pass"), ComputeShader, PassParameters, FIntVector(8, 8, 8));


	// ------ Extracting to pooled render target ------
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget;
	// Copy the result of compute shader from UAV to pooled renderT-target
	GraphBuilder.QueueTextureExtraction(OutTextureRef, &PooledComputeTarget);
	// Execute the graph
	//GraphBuilder.Execute();
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget_d;
	GraphBuilder.QueueTextureExtraction(OutTexture_dRef, &PooledComputeTarget_d);
	// Execute the graph
	GraphBuilder.Execute();
	

	// Queue the UAV we wrote to for extraction 
	RHICmdList.CopyToResolveTarget(PooledComputeTarget.GetReference()->GetRenderTargetItem().TargetableTexture,RenderTarget->GetResource()->TextureRHI , FResolveParams());
	RHICmdList.CopyToResolveTarget(PooledComputeTarget_d.GetReference()->GetRenderTargetItem().TargetableTexture,RenderTarget_m->GetResource()->TextureRHI , FResolveParams());

	
	
}