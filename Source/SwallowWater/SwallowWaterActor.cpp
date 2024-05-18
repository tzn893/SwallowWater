// Fill out your copyright notice in the Description page of Project Settings.


#include "Kismet/KismetRenderingLibrary.h"
#include "Components/CapsuleComponent.h"

#include "SwallowWaterCharacter.h"
#include "SwallowWaterActor.h"

// Sets default values
ASwallowWaterActor::ASwallowWaterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	dx = 1e-2f;
	dt = 5e-4f;
	SubstepScale = 0.2f;
	AverageHeight = 1.0f;
	MaxHeight = 1.2f;
	bEnableSimulation = false;

	RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>("RT_SwallowWaterDefaultRT");
	TmpBuffer = CreateDefaultSubobject<UTextureRenderTarget2D>("RT_SwallowWaterDefaultTmpBuffer");
	SwallowWaterMaterial = CreateDefaultSubobject<UMaterialInstance>("MTI_SwallowWaterDefaultMaterial");
	CopyMaterial = CreateDefaultSubobject<UMaterialInstance>("MTI_SwallowWaterDefaultCopyMaterial");

	StaticPlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>("SM_DefaultWaterPlaneMesh");

	OverlappedActor = nullptr;
	OverlappedActorRadius = 0;
}

void ASwallowWaterActor::BeginDestroy()
{
	Super::BeginDestroy();

	DestroyDynamicMaterials();
}

// Called when the game starts or when spawned
void ASwallowWaterActor::BeginPlay()
{
	Super::BeginPlay();
	check(RenderTarget);
	check(TmpBuffer);

	check(SwallowWaterMaterial);
	check(CopyMaterial);

	FVector scale = GetActorScale();
	check(scale.X == scale.Y);

	DestroyDynamicMaterials();


	DynamicCopyMaterialInstance = UMaterialInstanceDynamic::Create(CopyMaterial, nullptr);
	DynamicSwallowWaterMaterialInstance = UMaterialInstanceDynamic::Create(SwallowWaterMaterial, nullptr);

	DynamicCopyMaterialInstance->SetTextureParameterValue(FName("SrcTexture"), RenderTarget);
	DynamicSwallowWaterMaterialInstance->SetTextureParameterValue(FName("RTWater"), TmpBuffer);
	DynamicSwallowWaterMaterialInstance->SetScalarParameterValue(FName("dt"), dt);
	DynamicSwallowWaterMaterialInstance->SetScalarParameterValue(FName("dx"), dx);
	DynamicSwallowWaterMaterialInstance->SetScalarParameterValue(FName("AverageHeight"), AverageHeight);
	DynamicSwallowWaterMaterialInstance->SetScalarParameterValue(FName("MaxHeight"), MaxHeight);

	StaticPlaneMesh->OnComponentBeginOverlap.AddDynamic(this, &ASwallowWaterActor::BeginOverlap);
	StaticPlaneMesh->OnComponentEndOverlap.AddDynamic(this, &ASwallowWaterActor::EndOverlap);
}


void ASwallowWaterActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	FVector OverlappedPosition;
	
	float OverlappedMappedRadius = 0;
	float OverlappedMappedU = 2;
	float OverlappedMappedV = 2;
	bool  bImpressTriggered = false;

	if (OverlappedActor)
	{
		OverlappedPosition = OverlappedActor->GetActorLocation();
		// ֻ������ٶȴ���ĳ��ֵ�Żᴥ��impress
		bImpressTriggered = OverlappedActor->GetVelocity().Size() > 0.1f;

		// plane mesh Ϊ2x2���������������ռ���plane mesh �����Ϊ2*PlaneMeshScale.x x 2*PlaneMeshScale.y
		float PlaneScale = 1.0;
		{
			FVector PlaneScale3 = GetActorScale();
			PlaneScale = FMath::Max(PlaneScale3.X, PlaneScale3.Y);
		}
		FVector PlanePosition = GetActorLocation();

		// ��õĳ��ȵ�λ�����ף���blender�ĵ�λ���ף�����ڼ���ʱҪ���Ե�λ100����
		const float UnitCast = 100.0f;

		FVector OverlappedUV = (OverlappedPosition - PlanePosition) / (PlaneScale * 2 * UnitCast) + 0.5;
		OverlappedMappedU = OverlappedUV.X;
		OverlappedMappedV = OverlappedUV.Y;

		OverlappedMappedRadius = OverlappedActorRadius / (PlaneScale * 2 * UnitCast);
	}


	if (bEnableSimulation)
	{
		if (bImpressTriggered)
		{
			DynamicSwallowWaterMaterialInstance->SetScalarParameterValue("ImpressRadius", OverlappedMappedRadius);
			DynamicSwallowWaterMaterialInstance->SetScalarParameterValue("ImpressU", 0.5);
			DynamicSwallowWaterMaterialInstance->SetScalarParameterValue("ImpressV", 0.5);
		}
		else
		{
			DynamicSwallowWaterMaterialInstance->SetScalarParameterValue("ImpressRadius", 0);
			DynamicSwallowWaterMaterialInstance->SetScalarParameterValue("ImpressU", 2);
			DynamicSwallowWaterMaterialInstance->SetScalarParameterValue("ImpressV", 2);
		}

		int SubstepCount = (int)FMath::Max((DeltaSeconds * SubstepScale) / dt, 1.0f);
		for (int i = 0; i < SubstepCount; i++)
		{
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), TmpBuffer, DynamicCopyMaterialInstance);
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, DynamicSwallowWaterMaterialInstance);
		}
	}
}

void ASwallowWaterActor::DestroyDynamicMaterials()
{
	if (DynamicCopyMaterialInstance)
	{
		DynamicCopyMaterialInstance->ConditionalBeginDestroy();
		DynamicCopyMaterialInstance = nullptr;
	}
	if (DynamicCopyMaterialInstance)
	{
		DynamicCopyMaterialInstance->ConditionalBeginDestroy();
		DynamicCopyMaterialInstance = nullptr;
	}
}

void ASwallowWaterActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (ASwallowWaterCharacter* OverlappedWaterActor = Cast<ASwallowWaterCharacter>(OtherActor))
	{
		OverlappedActor = OverlappedWaterActor;
		OverlappedActorRadius = OverlappedWaterActor->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

		FVector ActorScale = OverlappedWaterActor->GetActorScale();
		OverlappedActorRadius *= FMath::Max3(ActorScale.X, ActorScale.Y, ActorScale.Z);
	}

	/*
	if (UCapsuleComponent* OverlappedCapsule = Cast<UCapsuleComponent>(OverlappedComponent))
	{
		OverlappedActorRadius = OverlappedCapsule->GetUnscaledCapsuleRadius();
		OverlappedActor = OtherActor;
	}
	*/
}

void ASwallowWaterActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedActor == OtherActor)
	{
		OverlappedActor = nullptr;
	}
}

