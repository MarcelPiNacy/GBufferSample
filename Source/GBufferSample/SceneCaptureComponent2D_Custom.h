#pragma once
#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "SceneCaptureComponent2D_Custom.generated.h"



UCLASS(hidecategories = (Collision, Object, Physics, SceneComponent, Mobility))
class GBUFFERSAMPLE_API USceneCaptureComponent2D_Custom : public USceneCaptureComponent2D
{
	GENERATED_BODY()
public:

	USceneCaptureComponent2D_Custom(const FObjectInitializer & = FObjectInitializer::Get());

	const AActor* ViewActor;

	virtual const AActor* GetViewOwner() const override;
};