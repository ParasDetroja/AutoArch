#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFloorPlanGeneratorModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenuExtensions();
    void UnregisterMenuExtensions();
    void OnToolMenusStartup();
    void OnGenerateFromFloorPlan();
};
