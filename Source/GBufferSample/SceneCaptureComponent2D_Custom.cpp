#include "SceneCaptureComponent2D_Custom.h"

USceneCaptureComponent2D_Custom::USceneCaptureComponent2D_Custom(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
}

const AActor* USceneCaptureComponent2D_Custom::GetViewOwner() const
{
    return ViewActor;
}