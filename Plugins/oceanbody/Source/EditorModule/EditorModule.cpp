#include "EditorModule.h"

#include "Engine/VolumeTexture.h"
#include "Templates/SharedPointer.h"
#define LOCTEXT_NAMESPACE "FOceanEditorModule"
#include"Engine/TextureRenderTargetVolume.h"
void FOceanCommands::RegisterCommands()
{
	UI_COMMAND(Command, "OceanCommand", "Execute Ocean Command", EUserInterfaceActionType::Button, FInputGesture());
	// UI_COMMAND(Command_m, "OceanCommand_m", "Execute Ocean Command_m", EUserInterfaceActionType::Button, FInputGesture());
	// UI_COMMAND(Command_d, "OceanCommand_d", "Execute Ocean Command_d", EUserInterfaceActionType::Button, FInputGesture());
	// UI_COMMAND(Command_s, "OceanCommand_s", "Execute Ocean Command_s", EUserInterfaceActionType::Button, FInputGesture());
}

void FOceanEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();
	
	FOceanCommands::Register();
	CreateToolbarButton();
}

void FOceanEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	FOceanCommands::Unregister();
}

void FOceanEditorModule::CreateToolbarButton()
{
	FLevelEditorModule* pLevelEditorModule =FModuleManager::GetModulePtr<FLevelEditorModule>(FName(TEXT("LevelEditor")));
	
	
	CommandList = MakeShareable(new FUICommandList);
	CommandList->MapAction(
		FOceanCommands::Get().Command,
		FExecuteAction::CreateRaw(this, &FOceanEditorModule::Command_Tex),
		FCanExecuteAction()
	);
	// CommandList->MapAction(
	// 	FOceanCommands::Get().Command_m,
	// 	FExecuteAction::CreateRaw(this, &FOceanEditorModule::Command_Texm),
	// 	FCanExecuteAction()
	// );
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	// ToolbarExtender->AddMenuExtension("Log", EExtensionHook::After, MakeShareable(new FUICommandList),
	// 	   FNewMenuDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
	// 	   {
	// 		   MenuBuilder.AddSeparator();
	// 		   MenuBuilder.AddMenuEntry(LOCTEXT("LabelPrintLog", "打印Hello World"),
	// 									LOCTEXT("Tooltip", "在Log中打印Hello world"),
	// 									FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Blueprint"),
	// 									FUIAction(FExecuteAction::CreateLambda([]()
	// 									{
	// 										UE_LOG(LogTemp, Log, TEXT("Hello world"))
	// 									})));
	// 	   }));

	ToolbarExtender->AddMenuBarExtension("Help", EExtensionHook::After, MakeShareable(new FUICommandList),
		FMenuBarExtensionDelegate::CreateLambda([this](FMenuBarBuilder& MenuBarBuilder)
		{
			MenuBarBuilder.AddPullDownMenu(LOCTEXT("MenubarName", "Oceanbody"),
							LOCTEXT("Menubar Tool Tip", "新菜单提示"),
							FNewMenuDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
							{
								MenuBuilder.AddSeparator();
								MenuBuilder.AddMenuEntry(LOCTEXT("LabelPrintLog", "generate tex"),
											  LOCTEXT("Tooltip", "run the shader"),
											  FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Blueprint"),
											  FUIAction(FExecuteAction::CreateRaw(this, &FOceanEditorModule::Command_Tex)));
								// MenuBuilder.AddMenuEntry(LOCTEXT("LabelPrintLog", "waterbody_multiscattering_pre"),
							 //   LOCTEXT("Tooltip", "run the shader"),
							 //   FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Blueprint"),
							 //   FUIAction(FExecuteAction::CreateRaw(this, &FOceanEditorModule::Command_Texm)));
								//
								// MenuBuilder.AddMenuEntry(LOCTEXT("LabelPrintLog", "waterbody_scattering_density_pre"),
							 //   LOCTEXT("Tooltip", "run the shader"),
							 //   FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Blueprint"),
							 //   FUIAction(FExecuteAction::CreateRaw(this, &FOceanEditorModule::Command_Texd)));
								//
								// MenuBuilder.AddMenuEntry(LOCTEXT("LabelPrintLog", "waterbody_scattering_synthesize_pre"),
							 //   LOCTEXT("Tooltip", "run the shader"),
							 //   FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Blueprint"),
							 //   FUIAction(FExecuteAction::CreateRaw(this, &FOceanEditorModule::Command_Texs)));
							 }),
							 
														 "SubMenuHook");
		}));
	pLevelEditorModule->GetMenuExtensibilityManager()->AddExtender(ToolbarExtender);
	// ToolbarExtender->AddToolBarExtension(
	// 	"Settings",
	// 	EExtensionHook::After,
	// 	CommandList,
	// 	FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& ToolbarBuilder)
	// 	{
	// 		ToolbarBuilder.AddToolBarButton(
	// 		   FUIAction(
	// 			   FExecuteAction::CreateRaw(this, &FOceanEditorModule::Command_Tex)
	// 		   ),
	// 		   NAME_None,
	// 		   FText::FromString(TEXT("Generate Texture")),
	// 		   FText::FromString(TEXT("Generates the texture for the shader")),
	// 		   FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Build")
	// 	   );
	// 	})
	// );
	
	pLevelEditorModule->GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FOceanEditorModule::Command_Tex()
{
	FoceanbodyModule& OceanbodyModule = FModuleManager::LoadModuleChecked<FoceanbodyModule>("OceanBody");
	oceanbodyModule = &OceanbodyModule;
	FSoftObjectPath TexturePath(TEXT("/Game/Resources/LUTs/ScatteringLUT"));
	//auto ScatteringLUT = (TexturePath.TryLoad());
	UTextureRenderTargetVolume* ScatteringLUT  = Cast<UTextureRenderTargetVolume>(TexturePath.TryLoad());
	if(ScatteringLUT == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load texture"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded Texture"));
	}
	FSoftObjectPath Texture_dPath(TEXT("/Game/Resources/LUTs/MultiScatteringDensityLUT"));
	UTextureRenderTargetVolume* MultiScatteringDensityLUT = Cast<UTextureRenderTargetVolume>(Texture_dPath.TryLoad());
	if(MultiScatteringDensityLUT == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load texture"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded Texture"));
	}
	TArray<UTextureRenderTargetVolume*> MultiScatteringLUTs;
	MultiScatteringLUTs.SetNum(NUM_SCATTERING_ORDERS+1);
	TArray<UTextureRenderTargetVolume*> ScatteringDensityLUTs;
	ScatteringDensityLUTs.SetNum(NUM_SCATTERING_ORDERS+1);
	
	for(size_t i = 0; i < NUM_SCATTERING_ORDERS; i++)
	{
		FString AssetPath_m = FString::Printf(TEXT("/Game/Resources/LUTs/MultiScatteringLUTs_%d"),  i);
		FSoftObjectPath TexturePath_m(AssetPath_m);
		if (Cast<UTextureRenderTargetVolume>(TexturePath_m.TryLoad()) == NULL)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load texture"));
			
		}
		else
		{
			MultiScatteringLUTs[i] = Cast<UTextureRenderTargetVolume>(TexturePath_m.TryLoad());
		}
		
		//MultiScatteringLUTs[i] = Cast<UVolumeTexture>(TexturePath_m.TryLoad());
		FString AssetPath = FString::Printf(TEXT("/Game/Resources/LUTs/ScatteringDensityLUTs_%d"),  i);
		FSoftObjectPath TexturePath_s(AssetPath);
		//ScatteringDensityLUTs[i] = Cast<UTextureRenderTargetVolume>(TexturePath.TryLoad());
		ScatteringDensityLUTs[i] = Cast<UTextureRenderTargetVolume>(TexturePath_s.TryLoad());
	}
	
	
	FWaterbodyParameters Waterbody;
	
	ENQUEUE_RENDER_COMMAND(ComputeShader)(
		[MultiScatteringLUTs, ScatteringDensityLUTs,Waterbody,ScatteringLUT,MultiScatteringDensityLUT, this ]
		(FRHICommandListImmediate& RHICmdList)
	{
			
			TArray<FRHITexture*> MultiScatteringLUTs_RHI;
		MultiScatteringLUTs_RHI.Reserve(NUM_SCATTERING_ORDERS+1);
		MultiScatteringLUTs_RHI.SetNum(NUM_SCATTERING_ORDERS);
		TArray<FRHITexture* > ScatteringDensityLUTs_RHI;
		ScatteringDensityLUTs_RHI.Reserve(NUM_SCATTERING_ORDERS+1);
		ScatteringDensityLUTs_RHI.SetNum(NUM_SCATTERING_ORDERS);
		TArray<FRHISamplerState*> MultiSamplerState, DensitySamplerState;
		MultiSamplerState.Reserve(NUM_SCATTERING_ORDERS+1);
		DensitySamplerState.Reserve(NUM_SCATTERING_ORDERS+1);
		MultiSamplerState.SetNum(NUM_SCATTERING_ORDERS);
		DensitySamplerState.SetNum(NUM_SCATTERING_ORDERS);
		oceanbodyModule->EnqueueRenderCommand(MultiScatteringLUTs[0], ScatteringDensityLUTs[0], Waterbody,RHICmdList);
		for(size_t scattering_order = 2; scattering_order <= NUM_SCATTERING_ORDERS; scattering_order++)
		{
			int LUTs_index = scattering_order - 1;
			oceanbodyModule->EnqueueRenderCommand_d(MultiScatteringLUTs[LUTs_index-1]->Resource->TextureRHI,TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),ScatteringDensityLUTs[LUTs_index],Waterbody,RHICmdList);
			oceanbodyModule->EnqueueRenderCommand_m(ScatteringDensityLUTs[LUTs_index]->Resource->TextureRHI, TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(), MultiScatteringLUTs[LUTs_index], Waterbody,RHICmdList);
		}
		for(size_t i = 0; i<NUM_SCATTERING_ORDERS; i++)
		{
			
			MultiScatteringLUTs_RHI[i] = MultiScatteringLUTs[i]->Resource->TextureRHI.GetReference();
			ScatteringDensityLUTs_RHI[i] = ScatteringDensityLUTs[i]->Resource->TextureRHI.GetReference();
			DensitySamplerState[i] = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
			MultiSamplerState[i] = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
		}
		oceanbodyModule->EnqueueRenderCommand_s(MultiScatteringLUTs_RHI, ScatteringDensityLUTs_RHI, MultiSamplerState, DensitySamplerState, ScatteringLUT, MultiScatteringDensityLUT,RHICmdList);
	});
	
	
	

	
	// if (RenderTarget&&RenderTarget_d)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Loaded Texture"));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Failed to load texture"));
	// }
	//
	
	UE_LOG(LogTemp, Log, TEXT("Shader Complete"));
}



#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FOceanEditorModule, EditorModule)