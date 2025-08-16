# Floor Plan Generator Plugin Documentation

## Overview
Unreal Engine plugin that automatically generates 3D interior structures from floor plan images with configurable wall, window, and door heights. Supports real-world scaling from dimension text and single-click generation.

## User Preferences
Preferred communication style: Simple, everyday language.

## System Architecture
- **Plugin Structure**: C++ runtime module with Blueprint integration
- **Core Components**:
  - FloorPlanProcessor: Main entry point for processing
  - FloorPlanAnalyzer: Image analysis and dimension extraction  
  - StructureBuilder: 3D structure creation coordination
  - MeshGenerator: Procedural mesh generation with opening logic
- **Wall Opening Logic**: 
  - Windows: Wall packed from top and bottom sides
  - Doors: Wall packed from top side only
- **Materials**: Separate materials for walls, floors, and ceilings

## Recent Changes
- Enhanced wall mesh generation with proper door/window openings (August 15, 2025)
- Added dimension text parsing for room sizes from floor plans
- Implemented wall segment creation with configurable heights
- Added single-click generation via Content Browser context menu

## External Dependencies
- Unreal Engine 5.x
- ProceduralMeshComponent module
- ImageWrapper module for texture processing
- Regex support for dimension text parsing