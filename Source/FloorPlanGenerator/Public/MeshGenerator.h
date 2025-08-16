#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StaticMesh.h"
#include "FloorPlanAnalyzer.h"
#include "MeshGenerator.generated.h"

// Wall segment structure for procedural generation
USERSTRUCT(BlueprintType)
struct FWallSegment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartX = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EndX = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartZ = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EndZ = 0.0f;

    FWallSegment()
    {
        StartX = 0.0f;
        EndX = 0.0f;
        StartZ = 0.0f;
        EndZ = 0.0f;
    }
};

UCLASS(BlueprintType)
class FLOORPLANGENERATOR_API UMeshGenerator : public UObject
{
    GENERATED_BODY()

public:
    UMeshGenerator();

    // Mesh generation functions
    UFUNCTION(BlueprintCallable, Category = "Mesh Generation")
    UStaticMesh* GenerateWallMesh(const FVector2D& StartPoint, const FVector2D& EndPoint, 
                                  float Height, float Thickness, 
                                  const TArray<FOpeningData>& Openings,
                                  float DoorHeight, float WindowHeight);

    UFUNCTION(BlueprintCallable, Category = "Mesh Generation")
    UStaticMesh* GenerateFloorMesh(const TArray<FVector2D>& BoundaryPoints, float ZHeight);

    UFUNCTION(BlueprintCallable, Category = "Mesh Generation")
    UStaticMesh* GenerateCeilingMesh(const TArray<FVector2D>& BoundaryPoints, float ZHeight);

private:
    // Helper functions for mesh creation
    void CreateBoxMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector2D>& UVs,
                       const FVector& Center, const FVector& Extent);
    
    void CreatePlaneMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector2D>& UVs,
                        const TArray<FVector>& BoundaryPoints);

    void CreateWallWithOpenings(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector2D>& UVs,
                               const FVector& WallStart, const FVector& WallEnd, float Height, float Thickness,
                               const TArray<FOpeningData>& Openings, float DoorHeight, float WindowHeight);

    // Procedural mesh generation functions
    void CreateWallMeshWithOpenings(TArray<FVector>& Vertices, TArray<int32>& Triangles, 
                                   TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                                   float Length, float Height, float Thickness,
                                   const TArray<FOpeningData>& Openings, 
                                   float DoorHeight, float WindowHeight);

    void CreateWallSegmentMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles,
                              TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                              const struct FWallSegment& Segment, float Thickness);

    void CreateThickFloorMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles,
                             TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                             float Width, float Length, float Thickness);

    void CreateThickCeilingMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles,
                               TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                               float Width, float Length, float Thickness);

    UStaticMesh* CreateStaticMeshAsset(const TArray<FVector>& Vertices, 
                                       const TArray<int32>& Triangles, 
                                       const TArray<FVector2D>& UVs,
                                       const TArray<FVector>& Normals,
                                       const FString& MeshName);

    // Utility functions
    FVector2D To2D(const FVector& Vector3D) { return FVector2D(Vector3D.X, Vector3D.Y); }
    FVector To3D(const FVector2D& Vector2D, float Z = 0.0f) { return FVector(Vector2D.X, Vector2D.Y, Z); }
    
    int32 MeshCounter = 0;
};
