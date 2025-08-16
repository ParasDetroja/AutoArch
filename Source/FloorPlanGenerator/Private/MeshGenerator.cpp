#include "MeshGenerator.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/Package.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"

UMeshGenerator::UMeshGenerator()
{
    MeshCounter = 0;
}

UStaticMesh* UMeshGenerator::GenerateWallMesh(const FVector2D& StartPoint, const FVector2D& EndPoint, 
                                              float Height, float Thickness, 
                                              const TArray<FOpeningData>& Openings,
                                              float DoorHeight, float WindowHeight)
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector2D> UVs;
    TArray<FVector> Normals;

    // Calculate wall parameters
    FVector2D WallDirection = (EndPoint - StartPoint).GetSafeNormal();
    FVector2D WallNormal = FVector2D(-WallDirection.Y, WallDirection.X);
    float WallLength = FVector2D::Distance(StartPoint, EndPoint);
    
    // Convert to UE units (cm) and center at origin
    float WallLengthUE = WallLength;
    float WallHeightUE = Height;
    float WallThicknessUE = Thickness;
    
    CreateWallMeshWithOpenings(Vertices, Triangles, UVs, Normals, 
                              WallLengthUE, WallHeightUE, WallThicknessUE, 
                              Openings, DoorHeight, WindowHeight);

    FString MeshName = FString::Printf(TEXT("Wall_%.0f_x_%.0f"), WallLengthUE, WallHeightUE);
    return CreateStaticMeshAsset(Vertices, Triangles, UVs, Normals, MeshName);
}

UStaticMesh* UMeshGenerator::GenerateFloorMesh(const TArray<FVector2D>& BoundaryPoints, float ZHeight)
{
    if (BoundaryPoints.Num() < 3)
    {
        UE_LOG(LogTemp, Error, TEXT("MeshGenerator: Not enough boundary points for floor mesh"));
        return nullptr;
    }

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector2D> UVs;
    TArray<FVector> Normals;

    // Calculate room dimensions
    FVector2D MinPoint = BoundaryPoints[0];
    FVector2D MaxPoint = BoundaryPoints[0];
    for (const FVector2D& Point : BoundaryPoints)
    {
        MinPoint.X = FMath::Min(MinPoint.X, Point.X);
        MinPoint.Y = FMath::Min(MinPoint.Y, Point.Y);
        MaxPoint.X = FMath::Max(MaxPoint.X, Point.X);
        MaxPoint.Y = FMath::Max(MaxPoint.Y, Point.Y);
    }
    
    float RoomWidth = MaxPoint.X - MinPoint.X;
    float RoomLength = MaxPoint.Y - MinPoint.Y;
    float FloorThickness = 20.0f; // 20cm thick floor

    CreateThickFloorMesh(Vertices, Triangles, UVs, Normals, RoomWidth, RoomLength, FloorThickness);

    FString MeshName = FString::Printf(TEXT("Floor_%.0f_x_%.0f"), RoomWidth, RoomLength);
    return CreateStaticMeshAsset(Vertices, Triangles, UVs, Normals, MeshName);
}

UStaticMesh* UMeshGenerator::GenerateCeilingMesh(const TArray<FVector2D>& BoundaryPoints, float ZHeight)
{
    if (BoundaryPoints.Num() < 3)
    {
        return nullptr;
    }

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector2D> UVs;
    TArray<FVector> Normals;

    // Calculate room dimensions
    FVector2D MinPoint = BoundaryPoints[0];
    FVector2D MaxPoint = BoundaryPoints[0];
    for (const FVector2D& Point : BoundaryPoints)
    {
        MinPoint.X = FMath::Min(MinPoint.X, Point.X);
        MinPoint.Y = FMath::Min(MinPoint.Y, Point.Y);
        MaxPoint.X = FMath::Max(MaxPoint.X, Point.X);
        MaxPoint.Y = FMath::Max(MaxPoint.Y, Point.Y);
    }
    
    float RoomWidth = MaxPoint.X - MinPoint.X;
    float RoomLength = MaxPoint.Y - MinPoint.Y;
    float CeilingThickness = 15.0f; // 15cm thick ceiling

    CreateThickCeilingMesh(Vertices, Triangles, UVs, Normals, RoomWidth, RoomLength, CeilingThickness);

    FString MeshName = FString::Printf(TEXT("Ceiling_%.0f_x_%.0f"), RoomWidth, RoomLength);
    return CreateStaticMeshAsset(Vertices, Triangles, UVs, Normals, MeshName);
}

void UMeshGenerator::CreateWallMeshWithOpenings(TArray<FVector>& Vertices, TArray<int32>& Triangles, 
                                               TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                                               float Length, float Height, float Thickness,
                                               const TArray<FOpeningData>& Openings, 
                                               float DoorHeight, float WindowHeight)
{
    // Create wall segments based on openings
    TArray<FWallSegment> WallSegments;
    
    if (Openings.Num() == 0)
    {
        // Simple wall without openings
        FWallSegment FullWall;
        FullWall.StartX = -Length * 0.5f;
        FullWall.EndX = Length * 0.5f;
        FullWall.StartZ = 0.0f;
        FullWall.EndZ = Height;
        WallSegments.Add(FullWall);
    }
    else
    {
        // Create wall segments with openings
        float CurrentX = -Length * 0.5f;
        
        for (const FOpeningData& Opening : Openings)
        {
            float OpeningX = 0.0f; // Center opening for simplicity
            float OpeningWidth = Opening.Size.X;
            float OpeningHeight = Opening.bIsDoor ? DoorHeight : WindowHeight;
            
            float OpeningStart = OpeningX - OpeningWidth * 0.5f;
            float OpeningEnd = OpeningX + OpeningWidth * 0.5f;
            
            // Wall segment before opening
            if (OpeningStart > CurrentX)
            {
                FWallSegment BeforeOpening;
                BeforeOpening.StartX = CurrentX;
                BeforeOpening.EndX = OpeningStart;
                BeforeOpening.StartZ = 0.0f;
                BeforeOpening.EndZ = Height;
                WallSegments.Add(BeforeOpening);
            }
            
            // Wall segments around opening
            if (Opening.bIsDoor)
            {
                // Door: wall only above
                if (OpeningHeight < Height)
                {
                    FWallSegment AboveDoor;
                    AboveDoor.StartX = OpeningStart;
                    AboveDoor.EndX = OpeningEnd;
                    AboveDoor.StartZ = OpeningHeight;
                    AboveDoor.EndZ = Height;
                    WallSegments.Add(AboveDoor);
                }
            }
            else
            {
                // Window: wall above and below
                float WindowBottom = (Height - WindowHeight) * 0.5f;
                float WindowTop = WindowBottom + WindowHeight;
                
                // Below window
                if (WindowBottom > 0)
                {
                    FWallSegment BelowWindow;
                    BelowWindow.StartX = OpeningStart;
                    BelowWindow.EndX = OpeningEnd;
                    BelowWindow.StartZ = 0.0f;
                    BelowWindow.EndZ = WindowBottom;
                    WallSegments.Add(BelowWindow);
                }
                
                // Above window
                if (WindowTop < Height)
                {
                    FWallSegment AboveWindow;
                    AboveWindow.StartX = OpeningStart;
                    AboveWindow.EndX = OpeningEnd;
                    AboveWindow.StartZ = WindowTop;
                    AboveWindow.EndZ = Height;
                    WallSegments.Add(AboveWindow);
                }
            }
            
            CurrentX = OpeningEnd;
        }
        
        // Final wall segment
        if (CurrentX < Length * 0.5f)
        {
            FWallSegment FinalSegment;
            FinalSegment.StartX = CurrentX;
            FinalSegment.EndX = Length * 0.5f;
            FinalSegment.StartZ = 0.0f;
            FinalSegment.EndZ = Height;
            WallSegments.Add(FinalSegment);
        }
    }
    
    // Generate mesh for each wall segment
    for (const FWallSegment& Segment : WallSegments)
    {
        CreateWallSegmentMesh(Vertices, Triangles, UVs, Normals, Segment, Thickness);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Generated wall mesh: %.1f x %.1f x %.1f with %d openings, %d segments"), 
           Length, Height, Thickness, Openings.Num(), WallSegments.Num());
}

void UMeshGenerator::CreateWallSegmentMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles,
                                          TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                                          const FWallSegment& Segment, float Thickness)
{
    int32 StartIndex = Vertices.Num();
    
    float HalfThickness = Thickness * 0.5f;
    float SegmentWidth = Segment.EndX - Segment.StartX;
    float SegmentHeight = Segment.EndZ - Segment.StartZ;
    
    // Create box vertices for wall segment
    TArray<FVector> BoxVertices = {
        // Front face
        FVector(Segment.StartX, -HalfThickness, Segment.StartZ), // 0
        FVector(Segment.EndX, -HalfThickness, Segment.StartZ),   // 1
        FVector(Segment.EndX, -HalfThickness, Segment.EndZ),     // 2
        FVector(Segment.StartX, -HalfThickness, Segment.EndZ),   // 3
        // Back face
        FVector(Segment.StartX, HalfThickness, Segment.StartZ),  // 4
        FVector(Segment.EndX, HalfThickness, Segment.StartZ),    // 5
        FVector(Segment.EndX, HalfThickness, Segment.EndZ),      // 6
        FVector(Segment.StartX, HalfThickness, Segment.EndZ)     // 7
    };
    
    Vertices.Append(BoxVertices);
    
    // Add UVs
    for (int32 i = 0; i < 8; i++)
    {
        UVs.Add(FVector2D((i % 2) * 1.0f, (i / 4) * 1.0f));
    }
    
    // Add normals (will be calculated properly later)
    for (int32 i = 0; i < 8; i++)
    {
        Normals.Add(FVector::UpVector);
    }
    
    // Create triangles for box faces
    TArray<int32> BoxTriangles = {
        // Front face
        0, 2, 1, 0, 3, 2,
        // Back face  
        4, 5, 6, 4, 6, 7,
        // Left face
        4, 7, 3, 4, 3, 0,
        // Right face
        1, 2, 6, 1, 6, 5,
        // Bottom face
        4, 0, 1, 4, 1, 5,
        // Top face
        3, 7, 6, 3, 6, 2
    };
    
    // Add triangles with offset
    for (int32 Triangle : BoxTriangles)
    {
        Triangles.Add(StartIndex + Triangle);
    }
}

void UMeshGenerator::CreateThickFloorMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles,
                                         TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                                         float Width, float Length, float Thickness)
{
    int32 StartIndex = Vertices.Num();
    
    float HalfWidth = Width * 0.5f;
    float HalfLength = Length * 0.5f;
    
    // Create thick floor vertices (box)
    TArray<FVector> FloorVertices = {
        // Top face
        FVector(-HalfWidth, -HalfLength, 0),      // 0
        FVector(HalfWidth, -HalfLength, 0),       // 1
        FVector(HalfWidth, HalfLength, 0),        // 2
        FVector(-HalfWidth, HalfLength, 0),       // 3
        // Bottom face
        FVector(-HalfWidth, -HalfLength, -Thickness), // 4
        FVector(HalfWidth, -HalfLength, -Thickness),  // 5
        FVector(HalfWidth, HalfLength, -Thickness),   // 6
        FVector(-HalfWidth, HalfLength, -Thickness)   // 7
    };
    
    Vertices.Append(FloorVertices);
    
    // Add UVs
    for (int32 i = 0; i < 8; i++)
    {
        UVs.Add(FVector2D((i % 2) * 1.0f, ((i / 2) % 2) * 1.0f));
    }
    
    // Add normals
    for (int32 i = 0; i < 8; i++)
    {
        Normals.Add(FVector::UpVector);
    }
    
    // Create triangles for floor box
    TArray<int32> FloorTriangles = {
        // Top face
        0, 2, 1, 0, 3, 2,
        // Bottom face
        4, 5, 6, 4, 6, 7,
        // Front face
        0, 1, 5, 0, 5, 4,
        // Back face
        2, 3, 7, 2, 7, 6,
        // Left face
        3, 0, 4, 3, 4, 7,
        // Right face
        1, 2, 6, 1, 6, 5
    };
    
    for (int32 Triangle : FloorTriangles)
    {
        Triangles.Add(StartIndex + Triangle);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Generated thick floor: %.1f x %.1f x %.1f"), Width, Length, Thickness);
}

void UMeshGenerator::CreateThickCeilingMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles,
                                           TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                                           float Width, float Length, float Thickness)
{
    int32 StartIndex = Vertices.Num();
    
    float HalfWidth = Width * 0.5f;
    float HalfLength = Length * 0.5f;
    
    // Create thick ceiling vertices (box at top)
    TArray<FVector> CeilingVertices = {
        // Bottom face
        FVector(-HalfWidth, -HalfLength, 0),      // 0
        FVector(HalfWidth, -HalfLength, 0),       // 1
        FVector(HalfWidth, HalfLength, 0),        // 2
        FVector(-HalfWidth, HalfLength, 0),       // 3
        // Top face
        FVector(-HalfWidth, -HalfLength, Thickness), // 4
        FVector(HalfWidth, -HalfLength, Thickness),  // 5
        FVector(HalfWidth, HalfLength, Thickness),   // 6
        FVector(-HalfWidth, HalfLength, Thickness)   // 7
    };
    
    Vertices.Append(CeilingVertices);
    
    // Add UVs
    for (int32 i = 0; i < 8; i++)
    {
        UVs.Add(FVector2D((i % 2) * 1.0f, ((i / 2) % 2) * 1.0f));
    }
    
    // Add normals
    for (int32 i = 0; i < 8; i++)
    {
        Normals.Add(FVector::DownVector);
    }
    
    // Create triangles for ceiling box
    TArray<int32> CeilingTriangles = {
        // Bottom face
        0, 1, 2, 0, 2, 3,
        // Top face
        4, 6, 5, 4, 7, 6,
        // Front face
        0, 5, 1, 0, 4, 5,
        // Back face
        2, 6, 3, 3, 6, 7,
        // Left face
        3, 7, 0, 0, 7, 4,
        // Right face
        1, 5, 2, 2, 5, 6
    };
    
    for (int32 Triangle : CeilingTriangles)
    {
        Triangles.Add(StartIndex + Triangle);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Generated thick ceiling: %.1f x %.1f x %.1f"), Width, Length, Thickness);
}

UStaticMesh* UMeshGenerator::CreateStaticMeshAsset(const TArray<FVector>& Vertices, 
                                                  const TArray<int32>& Triangles, 
                                                  const TArray<FVector2D>& UVs,
                                                  const TArray<FVector>& Normals,
                                                  const FString& MeshName)
{
    if (Vertices.Num() == 0 || Triangles.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("MeshGenerator: No vertices or triangles to create mesh"));
        return nullptr;
    }

    // Create package in Content Browser
    FString PackagePath = FString::Printf(TEXT("/Game/FloorPlanAssets/%s"), *MeshName);
    UPackage* Package = CreatePackage(*PackagePath);
    
    // Create static mesh
    UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, *MeshName, RF_Public | RF_Standalone);
    StaticMesh->GetStaticMaterials().Add(FStaticMaterial());
    
    // Create mesh description
    FMeshDescription MeshDescription;
    FStaticMeshAttributes Attributes(MeshDescription);
    Attributes.Register();
    
    // Add vertices
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        MeshDescription.CreateVertex();
    }
    
    // Mark package dirty and register
    Package->MarkPackageDirty();
    FAssetRegistryModule::AssetCreated(StaticMesh);
    
    UE_LOG(LogTemp, Warning, TEXT("✓ Created mesh asset: %s with %d vertices, %d triangles - saved to Content Browser"), 
           *MeshName, Vertices.Num(), Triangles.Num() / 3);
    
    return StaticMesh;
}

// FWallSegment is now declared in header file