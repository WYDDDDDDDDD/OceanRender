
#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CoreMinimal.h"
#include "IRenderDocPlugin.h"
#include "LevelEditor.h"
#include"oceanbody/Public/OceanBody.h"


class FOceanEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void CreateToolbarButton();
	void Command_Tex();
	void InitRenderdoc();
	
private:
	FoceanbodyModule*  oceanbodyModule;
	IRenderDocPlugin&  RenderDocAPI= IRenderDocPlugin::Get();
	TSharedPtr<class FUICommandList> CommandList;
};
class FOceanCommands : public TCommands<FOceanCommands>
{
public:
	
	FOceanCommands() : TCommands<FOceanCommands>(
	"OceanCommands",
	NSLOCTEXT("Contexts", "OceanCommands", "Ocean Plugin"),
	NAME_None,
	FName(*FString("todo")))
	{
		//FLevelEditorModule* pLevelEditorModule =FModuleManager::GetModulePtr<FLevelEditorModule>(FName(TEXT("LevelEditor")));
	}

	// TCommands<>的接口：注册命令
	virtual void RegisterCommands() override;
public:
	
	//命令A
	TSharedPtr<class FUICommandInfo > Command;
	
};
