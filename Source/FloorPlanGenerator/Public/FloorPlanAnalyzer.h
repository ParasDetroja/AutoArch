#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "FloorPlanAnalyzer.generated.h"

USTRUCT(BlueprintType)
struct FLOORPLANGENERATOR_API FRoomData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RoomName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector2D> BoundaryPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Dimensions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Center;

    FRoomData()
    {
        RoomName = TEXT("");
        Dimensions = FVector2D::ZeroVector;
        Center = FVector2D::ZeroVector;
    }
};

USTRUCT(BlueprintType)
struct FLOORPLANGENERATOR_API FOpeningData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsDoor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Rotation;

    FOpeningData()
    {
        bIsDoor = true;
        Position = FVector2D::ZeroVector;
        Size = FVector2D::ZeroVector;
        Rotation = 0.0f;
    }
};

UCLASS(BlueprintType)
class FLOORPLANGENERATOR_API UFloorPlanAnalyzer : public UObject
{
    GENERATED_BODY()

public:
    UFloorPlanAnalyzer();

    // Main analysis function
    UFUNCTION(BlueprintCallable, Category = "Floor Plan Analysis")
    bool AnalyzeFloorPlan(UTexture2D* FloorPlanImage, float ScaleFactor);

    // Getters for analyzed data
    UFUNCTION(BlueprintPure, Category = "Floor Plan Analysis")
    const TArray<FRoomData>& GetRoomData() const { return RoomData; }

    UFUNCTION(BlueprintPure, Category = "Floor Plan Analysis")
    const TArray<FOpeningData>& GetOpeningData() const { return OpeningData; }

    UFUNCTION(BlueprintPure, Category = "Floor Plan Analysis")
    const TArray<FVector2D>& GetWallPoints() const { return WallPoints; }

    UFUNCTION(BlueprintPure, Category = "Floor Plan Analysis")
    FVector2D GetImageDimensions() const { return ImageDimensions; }

private:
    // Image processing functions
    bool ExtractImageData(UTexture2D* Texture, TArray<FColor>& PixelData, int32& Width, int32& Height);
    void DetectRooms(const TArray<FColor>& PixelData, int32 Width, int32 Height, float ScaleFactor);
    void DetectWalls(const TArray<FColor>& PixelData, int32 Width, int32 Height, float ScaleFactor);
    void DetectOpenings(const TArray<FColor>& PixelData, int32 Width, int32 Height, float ScaleFactor);
    
    // Helper functions
    bool IsBlackPixel(const FColor& Pixel) const;
    bool IsWhitePixel(const FColor& Pixel) const;
    FVector2D PixelToWorldCoordinates(int32 X, int32 Y, float ScaleFactor) const;
    void ParseDimensionText(const FString& Text, float& Width, float& Height) const;
    FString ExtractRoomNameFromRegion(const TArray<FColor>& PixelData, int32 Width, int32 Height, 
                                     const FIntPoint& MinPoint, const FIntPoint& MaxPoint) const;
    
    // Sample data creation functions
    void CreateSampleRoomsFromFloorPlan(float ScaleFactor);
    void CreateSampleWallPoints(float ScaleFactor);
    void CreateSampleOpenings(float ScaleFactor);

    // Analyzed data
    UPROPERTY()
    TArray<FRoomData> RoomData;

    UPROPERTY()
    TArray<FOpeningData> OpeningData;

    UPROPERTY()
    TArray<FVector2D> WallPoints;

    UPROPERTY()
    FVector2D ImageDimensions;
};
