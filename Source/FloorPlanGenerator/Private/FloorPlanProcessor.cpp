#include "FloorPlanProcessor.h"
#include "FloorPlanAnalyzer.h"
#include "StructureBuilder.h"
#include "Engine/World.h"
#include "Editor.h"

UFloorPlanProcessor::UFloorPlanProcessor()
{
    // Objects will be created when needed
    Analyzer = nullptr;
    Builder = nullptr;
}

void UFloorPlanProcessor::ProcessFloorPlan(UTexture2D* FloorPlanImage)
{
    if (!FloorPlanImage)
    {
        UE_LOG(LogTemp, Error, TEXT("FloorPlanProcessor: No floor plan image provided"));
        return;
    }

    // Create analyzer and builder if not already created
    if (!Analyzer)
    {
        Analyzer = NewObject<UFloorPlanAnalyzer>(this);
    }
    if (!Builder)
    {
        Builder = NewObject<UStructureBuilder>(this);
    }
    
    if (!Analyzer || !Builder)
    {
        UE_LOG(LogTemp, Error, TEXT("FloorPlanProcessor: Failed to create Analyzer or Builder"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("FloorPlanProcessor: Starting floor plan processing"));

    // Step 1: Analyze the floor plan image
    if (!Analyzer->AnalyzeFloorPlan(FloorPlanImage, ScaleFactor))
    {
        UE_LOG(LogTemp, Error, TEXT("FloorPlanProcessor: Failed to analyze floor plan"));
        return;
    }

    // Step 2: Configure builder parameters
    Builder->SetWallHeight(WallHeight);
    Builder->SetDoorHeight(DoorHeight);
    Builder->SetWindowHeight(WindowHeight);
    Builder->SetWallThickness(WallThickness);

    // Step 3: Build the 3D structure
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("FloorPlanProcessor: No valid world context"));
        return;
    }

    Builder->BuildStructure(World, Analyzer);

    UE_LOG(LogTemp, Log, TEXT("FloorPlanProcessor: Floor plan processing completed"));
}
