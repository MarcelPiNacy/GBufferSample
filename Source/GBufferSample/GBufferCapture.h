// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Renderer/Public/GBufferView.h"
#include "SceneCaptureComponent2D_Custom.h"
#include "GBufferCapture.generated.h"


class UTextureRenderTarget2D;

UCLASS()
class GBUFFERSAMPLE_API AGBufferCapture : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGBufferCapture(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CaptureGBuffer();

public:

	UPROPERTY(EditAnywhere)
	USceneCaptureComponent2D_Custom* CaptureComponent2D;

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* CaptureRenderTarget;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
