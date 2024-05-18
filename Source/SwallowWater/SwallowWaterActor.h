// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"

#include "SwallowWaterActor.generated.h"

UCLASS()
class SWALLOWWATER_API ASwallowWaterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwallowWaterActor();

protected:
	virtual void BeginDestroy() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


protected:
	void DestroyDynamicMaterials();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(EditAnywhere, Category="Simulation")
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnyWhere, Category="Simulation")
	UTextureRenderTarget2D* TmpBuffer;

	UPROPERTY(EditAnywhere, Category="Simulation")
	UMaterialInstance* SwallowWaterMaterial;

	UPROPERTY(EditAnywhere, Category="Simulation")
	UMaterialInstance* CopyMaterial;

	UPROPERTY(EditAnywhere, Category="Simulation")
	float dx;

	UPROPERTY(EditAnywhere, Category = "Simulation")
	float dt;

	UPROPERTY(EditAnywhere, Category = "Simulation")
	float AverageHeight;

	UPROPERTY(EditAnywhere, Category = "Simulation")
	float MaxHeight;

	UPROPERTY(EditAnywhere, Category="Simulation")
	float SubstepScale;

	UPROPERTY(EditAnywhere, Category="Simulation")
	bool  bEnableSimulation;

	UPROPERTY(EditAnywhere, Category = "Render")
	UStaticMeshComponent*			  StaticPlaneMesh;

	UMaterialInstanceDynamic* DynamicCopyMaterialInstance;
	UMaterialInstanceDynamic* DynamicSwallowWaterMaterialInstance;

	AActor* OverlappedActor;
	float	OverlappedActorRadius;
};
