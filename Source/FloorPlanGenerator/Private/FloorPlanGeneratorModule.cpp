#include "FloorPlanGeneratorModule.h"
#include "ToolMenus.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Engine/Texture2D.h"
#include "FloorPlanProcessor.h"

#define LOCTEXT_NAMESPACE "FFloorPlanGeneratorModule"

void FFloorPlanGeneratorModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("FloorPlanGenerator module started"));
    RegisterMenuExtensions();
}

void FFloorPlanGeneratorModule::ShutdownModule()
{
    UnregisterMenuExtensions();
    UE_LOG(LogTemp, Log, TEXT("FloorPlanGenerator module shutdown"));
}

void FFloorPlanGeneratorModule::RegisterMenuExtensions()
{
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FFloorPlanGeneratorModule::OnToolMenusStartup));
}

void FFloorPlanGeneratorModule::OnToolMenusStartup()
{
    FToolMenuOwnerScoped OwnerScoped(this);
    
    // Add menu item to Content Browser context menu for textures
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.Texture2D");
    if (Menu)
    {
        FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
        Section.AddMenuEntry(
            "GenerateFromFloorPlan",
            LOCTEXT("GenerateFromFloorPlan", "Generate 3D Structure from Floor Plan"),
            LOCTEXT("GenerateFromFloorPlanTooltip", "Generate walls, floors, and ceilings from this floor plan image"),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FFloorPlanGeneratorModule::OnGenerateFromFloorPlan))
        );
    }
}

void FFloorPlanGeneratorModule::UnregisterMenuExtensions()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FFloorPlanGeneratorModule::OnGenerateFromFloorPlan()
{
    // Get selected assets from Content Browser
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    TArray<FAssetData> SelectedAssets;
    ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

    for (const FAssetData& AssetData : SelectedAssets)
    {
        if (UTexture2D* FloorPlanTexture = Cast<UTexture2D>(AssetData.GetAsset()))
        {
            UE_LOG(LogTemp, Log, TEXT("Processing floor plan: %s"), *AssetData.AssetName.ToString());
            
            // Create and configure the floor plan processor
            UFloorPlanProcessor* Processor = NewObject<UFloorPlanProcessor>();
            if (Processor)
            {
                // Set default parameters
                Processor->SetWallHeight(300.0f); // 10 feet in cm
                Processor->SetDoorHeight(244.0f); // 8 feet in cm
                Processor->SetWindowHeight(152.0f); // 5 feet in cm
                Processor->SetWallThickness(10.0f); // 4 inches in cm
                
                // Process the floor plan
                Processor->ProcessFloorPlan(FloorPlanTexture);
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFloorPlanGeneratorModule, FloorPlanGenerator)
