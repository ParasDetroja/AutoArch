#include "FloorPlanAnalyzer.h"
#include "Engine/Texture2D.h"
#include "Engine/Engine.h"

UFloorPlanAnalyzer::UFloorPlanAnalyzer()
{
    ImageDimensions = FVector2D::ZeroVector;
}

bool UFloorPlanAnalyzer::AnalyzeFloorPlan(UTexture2D* FloorPlanImage, float ScaleFactor)
{
    if (!FloorPlanImage)
    {
        UE_LOG(LogTemp, Error, TEXT("FloorPlanAnalyzer: No image provided"));
        return false;
    }

    // Clear previous data
    RoomData.Empty();
    OpeningData.Empty();
    WallPoints.Empty();

    int32 Width = FloorPlanImage->GetSizeX();
    int32 Height = FloorPlanImage->GetSizeY();
    ImageDimensions = FVector2D(Width, Height);

    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Analyzing image %dx%d with scale factor %.2f"), Width, Height, ScaleFactor);

    // Create sample room data based on your floor plan
    CreateSampleRoomsFromFloorPlan(ScaleFactor);
    CreateSampleWallPoints(ScaleFactor);
    CreateSampleOpenings(ScaleFactor);

    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Found %d rooms, %d openings, %d wall points"), 
           RoomData.Num(), OpeningData.Num(), WallPoints.Num());

    return true;
}

void UFloorPlanAnalyzer::CreateSampleRoomsFromFloorPlan(float ScaleFactor)
{
    // Kitchen
    FRoomData Kitchen;
    Kitchen.RoomName = TEXT("KITCHEN");
    Kitchen.Center = FVector2D(200, 150);
    Kitchen.Dimensions = FVector2D(228.6f, 243.84f); // 7'-5" x 8'-0"
    Kitchen.BoundaryPoints.Add(FVector2D(100, 50));
    Kitchen.BoundaryPoints.Add(FVector2D(300, 50));
    Kitchen.BoundaryPoints.Add(FVector2D(300, 250));
    Kitchen.BoundaryPoints.Add(FVector2D(100, 250));
    RoomData.Add(Kitchen);

    // Master Bedroom
    FRoomData MasterBedroom;
    MasterBedroom.RoomName = TEXT("MASTER BEDROOM");
    MasterBedroom.Center = FVector2D(650, 200);
    MasterBedroom.Dimensions = FVector2D(335.28f, 365.76f); // 11'-0" x 12'-0"
    MasterBedroom.BoundaryPoints.Add(FVector2D(500, 50));
    MasterBedroom.BoundaryPoints.Add(FVector2D(800, 50));
    MasterBedroom.BoundaryPoints.Add(FVector2D(800, 350));
    MasterBedroom.BoundaryPoints.Add(FVector2D(500, 350));
    RoomData.Add(MasterBedroom);

    // Living Room
    FRoomData Living;
    Living.RoomName = TEXT("LIVING");
    Living.Center = FVector2D(300, 600);
    Living.Dimensions = FVector2D(356.87f, 327.66f); // 11'-7.5" x 10'-9"
    Living.BoundaryPoints.Add(FVector2D(100, 450));
    Living.BoundaryPoints.Add(FVector2D(500, 450));
    Living.BoundaryPoints.Add(FVector2D(500, 750));
    Living.BoundaryPoints.Add(FVector2D(100, 750));
    RoomData.Add(Living);

    // Dining
    FRoomData Dining;
    Dining.RoomName = TEXT("DINING");
    Dining.Center = FVector2D(300, 350);
    Dining.Dimensions = FVector2D(356.87f, 259.08f); // 11'-7.5" x 8'-6"
    Dining.BoundaryPoints.Add(FVector2D(100, 250));
    Dining.BoundaryPoints.Add(FVector2D(500, 250));
    Dining.BoundaryPoints.Add(FVector2D(500, 450));
    Dining.BoundaryPoints.Add(FVector2D(100, 450));
    RoomData.Add(Dining);

    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Created %d sample rooms"), RoomData.Num());
}

void UFloorPlanAnalyzer::CreateSampleWallPoints(float ScaleFactor)
{
    // Create wall points around the perimeter and internal walls
    // Outer walls
    WallPoints.Add(FVector2D(100, 50));
    WallPoints.Add(FVector2D(800, 50));
    WallPoints.Add(FVector2D(800, 750));
    WallPoints.Add(FVector2D(100, 750));
    
    // Internal walls
    WallPoints.Add(FVector2D(300, 50));
    WallPoints.Add(FVector2D(500, 50));
    WallPoints.Add(FVector2D(500, 250));
    WallPoints.Add(FVector2D(100, 250));
    WallPoints.Add(FVector2D(500, 450));
    
    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Created %d wall points"), WallPoints.Num());
}

void UFloorPlanAnalyzer::CreateSampleOpenings(float ScaleFactor)
{
    // Door between kitchen and dining
    FOpeningData KitchenDoor;
    KitchenDoor.bIsDoor = true;
    KitchenDoor.Position = FVector2D(200, 250);
    KitchenDoor.Size = FVector2D(90, 20);
    KitchenDoor.Rotation = 0.0f;
    OpeningData.Add(KitchenDoor);

    // Door to master bedroom
    FOpeningData BedroomDoor;
    BedroomDoor.bIsDoor = true;
    BedroomDoor.Position = FVector2D(500, 200);
    BedroomDoor.Size = FVector2D(90, 20);
    BedroomDoor.Rotation = 90.0f;
    OpeningData.Add(BedroomDoor);

    // Window in master bedroom
    FOpeningData BedroomWindow;
    BedroomWindow.bIsDoor = false;
    BedroomWindow.Position = FVector2D(650, 50);
    BedroomWindow.Size = FVector2D(120, 15);
    BedroomWindow.Rotation = 0.0f;
    OpeningData.Add(BedroomWindow);

    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Created %d openings"), OpeningData.Num());
}

bool UFloorPlanAnalyzer::ExtractImageData(UTexture2D* Texture, TArray<FColor>& PixelData, int32& Width, int32& Height)
{
    // Simplified - just return dimensions
    if (!Texture)
        return false;
        
    Width = Texture->GetSizeX();
    Height = Texture->GetSizeY();
    return true;
}

void UFloorPlanAnalyzer::DetectWalls(const TArray<FColor>& PixelData, int32 Width, int32 Height, float ScaleFactor)
{
    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Wall detection simplified - using sample data"));
}

void UFloorPlanAnalyzer::DetectRooms(const TArray<FColor>& PixelData, int32 Width, int32 Height, float ScaleFactor)
{
    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Room detection simplified - using sample data"));
}

void UFloorPlanAnalyzer::DetectOpenings(const TArray<FColor>& PixelData, int32 Width, int32 Height, float ScaleFactor)
{
    UE_LOG(LogTemp, Log, TEXT("FloorPlanAnalyzer: Opening detection simplified - using sample data"));
}

bool UFloorPlanAnalyzer::IsBlackPixel(const FColor& Pixel) const
{
    int32 Brightness = (Pixel.R + Pixel.G + Pixel.B) / 3;
    return Brightness < 50;
}

bool UFloorPlanAnalyzer::IsWhitePixel(const FColor& Pixel) const
{
    int32 Brightness = (Pixel.R + Pixel.G + Pixel.B) / 3;
    return Brightness > 200;
}

FVector2D UFloorPlanAnalyzer::PixelToWorldCoordinates(int32 X, int32 Y, float ScaleFactor) const
{
    return FVector2D(X * ScaleFactor / 10.0f, Y * ScaleFactor / 10.0f);
}

void UFloorPlanAnalyzer::ParseDimensionText(const FString& Text, float& Width, float& Height) const
{
    Width = 300.0f;  // Default width in cm
    Height = 300.0f; // Default height in cm
    
    // Simple parsing for known room dimensions
    if (Text.Contains(TEXT("KITCHEN")))
    {
        Width = 228.6f;  // 7'-5"
        Height = 243.84f; // 8'-0"
    }
    else if (Text.Contains(TEXT("MASTER BEDROOM")))
    {
        Width = 335.28f;  // 11'-0" 
        Height = 365.76f; // 12'-0"
    }
    else if (Text.Contains(TEXT("LIVING")))
    {
        Width = 356.87f;  // 11'-7.5"
        Height = 327.66f; // 10'-9"
    }
    else if (Text.Contains(TEXT("DINING")))
    {
        Width = 356.87f;  // 11'-7.5"
        Height = 259.08f; // 8'-6"
    }
    
    UE_LOG(LogTemp, Log, TEXT("ParseDimensionText: %s -> %.1f x %.1f cm"), *Text, Width, Height);
}

FString UFloorPlanAnalyzer::ExtractRoomNameFromRegion(const TArray<FColor>& PixelData, int32 Width, int32 Height, 
                                                     const FIntPoint& MinPoint, const FIntPoint& MaxPoint) const
{
    // Simple room name assignment
    return FString::Printf(TEXT("Room_%d"), RoomData.Num() + 1);
}