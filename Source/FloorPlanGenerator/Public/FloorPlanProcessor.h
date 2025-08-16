#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "FloorPlanProcessor.generated.h"

class UFloorPlanAnalyzer;
class UStructureBuilder;

UCLASS(BlueprintType, Blueprintable)
class FLOORPLANGENERATOR_API UFloorPlanProcessor : public UObject
{
    GENERATED_BODY()

public:
    UFloorPlanProcessor();

    // Main processing function
    UFUNCTION(BlueprintCallable, Category = "Floor Plan Generator")
    void ProcessFloorPlan(UTexture2D* FloorPlanImage);

    // Parameter setters
    UFUNCTION(BlueprintCallable, Category = "Floor Plan Generator")
    void SetWallHeight(float Height) { WallHeight = Height; }

    UFUNCTION(BlueprintCallable, Category = "Floor Plan Generator")
    void SetDoorHeight(float Height) { DoorHeight = Height; }

    UFUNCTION(BlueprintCallable, Category = "Floor Plan Generator")
    void SetWindowHeight(float Height) { WindowHeight = Height; }

    UFUNCTION(BlueprintCallable, Category = "Floor Plan Generator")
    void SetWallThickness(float Thickness) { WallThickness = Thickness; }

    // Parameter getters
    UFUNCTION(BlueprintPure, Category = "Floor Plan Generator")
    float GetWallHeight() const { return WallHeight; }

    UFUNCTION(BlueprintPure, Category = "Floor Plan Generator")
    float GetDoorHeight() const { return DoorHeight; }

    UFUNCTION(BlueprintPure, Category = "Floor Plan Generator")
    float GetWindowHeight() const { return WindowHeight; }

    UFUNCTION(BlueprintPure, Category = "Floor Plan Generator")
    float GetWallThickness() const { return WallThickness; }

protected:
    // Configurable parameters (in centimeters for Unreal Engine)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
    float WallHeight = 300.0f; // 10 feet

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
    float DoorHeight = 244.0f; // 8 feet

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
    float WindowHeight = 152.0f; // 5 feet

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
    float WallThickness = 10.0f; // 4 inches

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
    float ScaleFactor = 30.48f; // Feet to centimeters conversion

private:
    UPROPERTY()
    UFloorPlanAnalyzer* Analyzer;

    UPROPERTY()
    UStructureBuilder* Builder;
};
