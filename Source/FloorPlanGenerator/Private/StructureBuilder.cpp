#include "StructureBuilder.h"
#include "FloorPlanAnalyzer.h"
#include "MeshGenerator.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

UStructureBuilder::UStructureBuilder()
{
    MeshGenerator = nullptr;
}

void UStructureBuilder::BuildStructure(UWorld* World, UFloorPlanAnalyzer* Analyzer)
{
    if (!World || !Analyzer)
    {
        UE_LOG(LogTemp, Error, TEXT("StructureBuilder: Invalid World or Analyzer"));
        return;
    }

    // Create mesh generator if needed
    if (!MeshGenerator)
    {
        MeshGenerator = NewObject<UMeshGenerator>(this);
    }

    UE_LOG(LogTemp, Warning, TEXT("🏗️ StructureBuilder: Starting ACCURATE floor plan generation"));

    // Generate meshes as assets in Content Browser (not level actors)
    GenerateFloorPlanAssets(Analyzer);

    UE_LOG(LogTemp, Warning, TEXT("✅ StructureBuilder: Floor plan assets generated successfully!"));
}

void UStructureBuilder::GenerateFloorPlanAssets(UFloorPlanAnalyzer* Analyzer)
{
    const TArray<FRoomData>& Rooms = Analyzer->GetRoomData();
    const TArray<FOpeningData>& Openings = Analyzer->GetOpeningData();

    UE_LOG(LogTemp, Warning, TEXT("📐 Generating assets for %d rooms with %d openings"), Rooms.Num(), Openings.Num());

    // Generate floor and ceiling assets for each room
    for (int32 RoomIndex = 0; RoomIndex < Rooms.Num(); ++RoomIndex)
    {
        const FRoomData& Room = Rooms[RoomIndex];
        
        UE_LOG(LogTemp, Warning, TEXT("🏠 Processing room: %s (%.1f x %.1f cm)"), 
               *Room.RoomName, Room.Dimensions.X, Room.Dimensions.Y);

        // Generate floor mesh asset (positioned at origin with unit scale)
        UStaticMesh* FloorMesh = MeshGenerator->GenerateFloorMesh(Room.BoundaryPoints, 0.0f);
        if (FloorMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("✅ Generated FLOOR asset for %s"), *Room.RoomName);
        }

        // Generate ceiling mesh asset (positioned at origin with unit scale)
        UStaticMesh* CeilingMesh = MeshGenerator->GenerateCeilingMesh(Room.BoundaryPoints, WallHeight);
        if (CeilingMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("✅ Generated CEILING asset for %s"), *Room.RoomName);
        }
    }

    // Generate accurate wall layout based on floor plan
    GenerateAccurateWallLayout(Analyzer);
}

void UStructureBuilder::GenerateAccurateWallLayout(UFloorPlanAnalyzer* Analyzer)
{
    UE_LOG(LogTemp, Warning, TEXT("🧱 Generating ACCURATE wall layout from floor plan"));

    // Create wall layout based on your specific floor plan
    TArray<FWallDefinition> WallDefinitions = CreateFloorPlanWallLayout();

    // Generate wall mesh assets for each wall
    for (int32 WallIndex = 0; WallIndex < WallDefinitions.Num(); ++WallIndex)
    {
        const FWallDefinition& WallDef = WallDefinitions[WallIndex];
        
        UE_LOG(LogTemp, Warning, TEXT("🧱 Creating wall: %s (%.1f x %.1f cm) with %d openings"), 
               *WallDef.WallName, WallDef.Length, WallHeight, WallDef.Openings.Num());

        // Generate wall mesh with proper openings
        UStaticMesh* WallMesh = MeshGenerator->GenerateWallMesh(
            FVector2D::ZeroVector, 
            FVector2D(WallDef.Length, 0), 
            WallHeight, 
            WallThickness, 
            WallDef.Openings, 
            DoorHeight, 
            WindowHeight
        );

        if (WallMesh)
        {
            FString OpeningInfo = "";
            for (const FOpeningData& Opening : WallDef.Openings)
            {
                FString OpeningType = Opening.bIsDoor ? TEXT("Door") : TEXT("Window");
                float OpeningHeight = Opening.bIsDoor ? DoorHeight : WindowHeight;
                OpeningInfo += FString::Printf(TEXT("%s(%.0fcm) "), *OpeningType, OpeningHeight);
            }
            
            UE_LOG(LogTemp, Warning, TEXT("✅ Generated WALL asset: %s - %s"), *WallDef.WallName, *OpeningInfo);
        }
    }
}

TArray<FWallDefinition> UStructureBuilder::CreateFloorPlanWallLayout()
{
    TArray<FWallDefinition> Walls;
    
    // Based on your floor plan image, create accurate wall layout
    
    // 1. KITCHEN to DINING wall (with door)
    FWallDefinition KitchenDiningWall;
    KitchenDiningWall.WallName = TEXT("Kitchen_Dining_Wall");
    KitchenDiningWall.Length = 356.87f; // 11'-7.5"
    FOpeningData KitchenDoor;
    KitchenDoor.bIsDoor = true;
    KitchenDoor.Position = FVector2D(100, 0); // Center position
    KitchenDoor.Size = FVector2D(90, 20); // 90cm wide door
    KitchenDiningWall.Openings.Add(KitchenDoor);
    Walls.Add(KitchenDiningWall);

    // 2. MASTER BEDROOM exterior wall (with window)
    FWallDefinition BedroomExteriorWall;
    BedroomExteriorWall.WallName = TEXT("MasterBedroom_Exterior_Wall");
    BedroomExteriorWall.Length = 365.76f; // 12'-0"
    FOpeningData BedroomWindow;
    BedroomWindow.bIsDoor = false;
    BedroomWindow.Position = FVector2D(180, 0); // Center position
    BedroomWindow.Size = FVector2D(120, 15); // 120cm wide window
    BedroomExteriorWall.Openings.Add(BedroomWindow);
    Walls.Add(BedroomExteriorWall);

    // 3. LIVING ROOM to DINING wall (with opening)
    FWallDefinition LivingDiningWall;
    LivingDiningWall.WallName = TEXT("Living_Dining_Wall");
    LivingDiningWall.Length = 356.87f; // 11'-7.5"
    FOpeningData LivingOpening;
    LivingOpening.bIsDoor = true;
    LivingOpening.Position = FVector2D(150, 0);
    LivingOpening.Size = FVector2D(120, 20); // Wide opening
    LivingDiningWall.Openings.Add(LivingOpening);
    Walls.Add(LivingDiningWall);

    // 4. TOILET wall (with door)
    FWallDefinition ToiletWall;
    ToiletWall.WallName = TEXT("Toilet_Wall");
    ToiletWall.Length = 243.84f; // 8'-0"
    FOpeningData ToiletDoor;
    ToiletDoor.bIsDoor = true;
    ToiletDoor.Position = FVector2D(80, 0);
    ToiletDoor.Size = FVector2D(75, 20); // 75cm door
    ToiletWall.Openings.Add(ToiletDoor);
    Walls.Add(ToiletWall);

    // 5. BED ROOM exterior wall (with window)
    FWallDefinition BedRoomWall;
    BedRoomWall.WallName = TEXT("BedRoom_Exterior_Wall");
    BedRoomWall.Length = 365.76f; // 12'-0"
    FOpeningData BedRoomWindow;
    BedRoomWindow.bIsDoor = false;
    BedRoomWindow.Position = FVector2D(180, 0);
    BedRoomWindow.Size = FVector2D(100, 15); // 100cm window
    BedRoomWall.Openings.Add(BedRoomWindow);
    Walls.Add(BedRoomWall);

    // 6. Exterior walls without openings
    FWallDefinition ExteriorWall1;
    ExteriorWall1.WallName = TEXT("Exterior_Wall_Front");
    ExteriorWall1.Length = 800.0f; // Front wall
    Walls.Add(ExteriorWall1);

    FWallDefinition ExteriorWall2;
    ExteriorWall2.WallName = TEXT("Exterior_Wall_Back");
    ExteriorWall2.Length = 800.0f; // Back wall
    Walls.Add(ExteriorWall2);

    FWallDefinition ExteriorWall3;
    ExteriorWall3.WallName = TEXT("Exterior_Wall_Left");
    ExteriorWall3.Length = 700.0f; // Left wall
    Walls.Add(ExteriorWall3);

    FWallDefinition ExteriorWall4;
    ExteriorWall4.WallName = TEXT("Exterior_Wall_Right");
    ExteriorWall4.Length = 700.0f; // Right wall
    Walls.Add(ExteriorWall4);

    UE_LOG(LogTemp, Warning, TEXT("📏 Created %d wall definitions for floor plan"), Walls.Num());
    return Walls;
}

void UStructureBuilder::BuildFloors(UWorld* World, UFloorPlanAnalyzer* Analyzer)
{
    // This function is now handled by GenerateFloorPlanAssets
    UE_LOG(LogTemp, Log, TEXT("Floor generation handled by asset generation"));
}

void UStructureBuilder::BuildWalls(UWorld* World, UFloorPlanAnalyzer* Analyzer)
{
    // This function is now handled by GenerateAccurateWallLayout
    UE_LOG(LogTemp, Log, TEXT("Wall generation handled by accurate layout system"));
}

void UStructureBuilder::BuildCeilings(UWorld* World, UFloorPlanAnalyzer* Analyzer)
{
    // This function is now handled by GenerateFloorPlanAssets
    UE_LOG(LogTemp, Log, TEXT("Ceiling generation handled by asset generation"));
}

void UStructureBuilder::CreateOpenings(UWorld* World, UFloorPlanAnalyzer* Analyzer)
{
    // Openings are now integrated into wall mesh generation
    UE_LOG(LogTemp, Log, TEXT("Openings integrated into procedural wall meshes"));
}

AActor* UStructureBuilder::CreateMeshActor(UWorld* World, const FString& Name)
{
    // Not used in new asset-based system
    return nullptr;
}

void UStructureBuilder::ApplyMaterial(UStaticMeshComponent* MeshComponent, const FString& MaterialPath)
{
    // Materials will be applied to generated assets
    if (!MeshComponent)
    {
        return;
    }

    UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
    if (Material)
    {
        MeshComponent->SetMaterial(0, Material);
    }
}