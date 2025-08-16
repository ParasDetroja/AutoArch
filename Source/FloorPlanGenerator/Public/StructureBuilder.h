#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/StaticMeshComponent.h"
#include "FloorPlanAnalyzer.h"
#include "StructureBuilder.generated.h"

// Wall definition structure for accurate layout generation
USERSTRUCT(BlueprintType)
struct FWallDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString WallName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Length = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FOpeningData> Openings;

    FWallDefinition()
    {
        WallName = TEXT("");
        Length = 0.0f;
    }
};

class UFloorPlanAnalyzer;
class UMeshGenerator;

UCLASS(BlueprintType)
class FLOORPLANGENERATOR_API UStructureBuilder : public UObject
{
    GENERATED_BODY()

public:
    UStructureBuilder();

    // Main building function
    UFUNCTION(BlueprintCallable, Category = "Structure Builder")
    void BuildStructure(UWorld* World, UFloorPlanAnalyzer* Analyzer);

    // Parameter setters
    UFUNCTION(BlueprintCallable, Category = "Structure Builder")
    void SetWallHeight(float Height) { WallHeight = Height; }

    UFUNCTION(BlueprintCallable, Category = "Structure Builder")
    void SetDoorHeight(float Height) { DoorHeight = Height; }

    UFUNCTION(BlueprintCallable, Category = "Structure Builder")
    void SetWindowHeight(float Height) { WindowHeight = Height; }

    UFUNCTION(BlueprintCallable, Category = "Structure Builder")
    void SetWallThickness(float Thickness) { WallThickness = Thickness; }

private:
    // Accurate floor plan generation functions
    void GenerateFloorPlanAssets(UFloorPlanAnalyzer* Analyzer);
    void GenerateAccurateWallLayout(UFloorPlanAnalyzer* Analyzer);
    TArray<FWallDefinition> CreateFloorPlanWallLayout();
    
    // Legacy building functions (now handled by asset generation)
    void BuildWalls(UWorld* World, UFloorPlanAnalyzer* Analyzer);
    void BuildFloors(UWorld* World, UFloorPlanAnalyzer* Analyzer);
    void BuildCeilings(UWorld* World, UFloorPlanAnalyzer* Analyzer);
    void CreateOpenings(UWorld* World, UFloorPlanAnalyzer* Analyzer);

    // Helper functions
    AActor* CreateMeshActor(UWorld* World, const FString& Name);
    void ApplyMaterial(UStaticMeshComponent* MeshComponent, const FString& MaterialPath);

    // Parameters
    float WallHeight = 300.0f;
    float DoorHeight = 244.0f;
    float WindowHeight = 152.0f;
    float WallThickness = 10.0f;

    UPROPERTY()
    UMeshGenerator* MeshGenerator;
};
