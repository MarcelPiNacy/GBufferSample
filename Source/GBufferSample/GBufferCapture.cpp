#include "GBufferCapture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Runtime/ImageWriteQueue/Public/ImageWriteQueue.h"
#include "HighResScreenshot.h"
#include "ImageUtil.h"
#include <thread>
#include <atomic>
#include <mutex>

// Sets default values
AGBufferCapture::AGBufferCapture(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    CaptureRenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("CaptureRenderTarget"));
    CaptureRenderTarget->CompressionSettings = TextureCompressionSettings::TC_Default;
    CaptureRenderTarget->SRGB = false;
    CaptureRenderTarget->bAutoGenerateMips = false;
    CaptureRenderTarget->bGPUSharedFlag = true;
    CaptureRenderTarget->AddressX = TextureAddress::TA_Clamp;
    CaptureRenderTarget->AddressY = TextureAddress::TA_Clamp;

    CaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D_Custom>(TEXT("GBUFFER CAPTURE"));
    CaptureComponent2D->ViewActor = this;
    CaptureComponent2D->bCaptureEveryFrame = false;
    CaptureComponent2D->SetupAttachment(RootComponent);
    CaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
    CaptureComponent2D->bCaptureOnMovement = false;
    CaptureComponent2D->bCaptureEveryFrame = false;
    CaptureComponent2D->bAlwaysPersistRenderingState = true;
}

// Called when the game starts or when spawned
void AGBufferCapture::BeginPlay()
{
    CaptureGBuffer();
	Super::BeginPlay();
}

void AGBufferCapture::CaptureGBuffer()
{
    if (CaptureComponent2D->TextureTarget == nullptr)
        return;

#if 0
    CaptureComponent2D->CaptureScene();
#else
    uint64 Mask = 0b1111111111;
    auto GBufferPtr = new FGBufferData();
    auto& GBuffer = *GBufferPtr;
    GBuffer.DesiredTexturesMask = Mask;
    GBuffer.OwningActor = CaptureComponent2D->GetViewOwner();
    for (size_t i = 0; i != FGBufferData::TextureCount; ++i)
        if ((GBuffer.DesiredTexturesMask & (UINT64_C(1) << i)) != 0)
            GBuffer.Payloads[i].Readback.Reset(new FRHIGPUTextureReadback(TEXT("GBUFFER READBACK")));
    CaptureComponent2D->CaptureSceneWithGBuffer(GBuffer);
    
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [GBufferPtr]
    {
        auto& GBuffer = *GBufferPtr;
        for (int i = 0; i != FGBufferData::TextureCount; ++i)
        {
            if ((GBuffer.DesiredTexturesMask & (UINT64_C(1) << i)) == 0)
                continue;
            static constexpr const TCHAR* TextureNames[] =
            {
                TEXT("SceneColor"), TEXT("SceneDepth"),
                TEXT("GBufferA"), TEXT("GBufferB"),
                TEXT("GBufferC"), TEXT("GBufferD"),
                TEXT("GBufferE"), TEXT("GBufferF"),
                TEXT("Velocity"), TEXT("SSAO")
            };
            auto& Payload = GBuffer.Payloads[i];
            auto Readback = Payload.Readback.Get();
            bool Dummy = false;
            while (!Readback->IsReady())
            {
                Dummy = Payload.bIsDummyTexture.load(std::memory_order_acquire);
                if (Dummy)
                    break;
                std::this_thread::yield();
            }
            if (Dummy)
                continue;
            check(Payload.Format != PF_Unknown);
            UE_LOG(LogTemp, Log, TEXT("ImageUtil::ExtractTexturePixelsFromReadback"));
            auto Pixels = ImageUtil::ExtractTexturePixelsFromReadback(
                Readback, Payload.InternalExtent,
                Payload.Extent, Payload.Format);
            check(Pixels.Num() != 0);
            check(Pixels.Num() == Payload.Extent.X * Payload.Extent.Y);
            static int Counter = 0;
            ++Counter;
            auto ImageTask = MakeUnique<FImageWriteTask>();
            ImageTask->PixelData = MakeUnique<TImagePixelData<FColor>>(Payload.Extent, TArray64<FColor>(MoveTemp(Pixels)));
            ImageTask->Filename = FString::Printf(TEXT("C:/Screenshots/%u-%s.png"), Counter, TextureNames[i]);
            ImageTask->Format = EImageFormat::PNG;
            ImageTask->CompressionQuality = (int32)EImageCompressionQuality::Uncompressed;
            ImageTask->bOverwriteFile = true;
            ImageTask->PixelPreProcessors.Add(TAsyncAlphaWrite<FColor>(255));
            check(GetHighResScreenshotConfig().ImageWriteQueue->Enqueue(MoveTemp(ImageTask)).Get());
        }
        delete GBufferPtr;
    });
#endif
}

// Called every frame
void AGBufferCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
