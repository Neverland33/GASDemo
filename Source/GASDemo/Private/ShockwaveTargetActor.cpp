// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockwaveTargetActor.h"
#include "Engine/OverlapResult.h"

void AShockwaveTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	PrimaryPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());

}

void AShockwaveTargetActor::ConfirmTargetingAndContinue()
{
	// 获得视角看向的位置
	FVector LookPoint;
	GetPlayerLookAtPoint(LookPoint);

	// 球形重叠检测的结果
	TArray<FOverlapResult> OverlapResults;
	// 检测到的敌人
	TArray<TWeakObjectPtr<AActor>> OverlapActors;

	// 检测参数
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;				// 不开启复杂检测
	QueryParams.bReturnPhysicalMaterial = false;	// 不检测物理材质
	APawn* SelfPawn = PrimaryPC->GetPawn();
	if (SelfPawn)
	{
		QueryParams.AddIgnoredActor(SelfPawn);		// 忽略自身
	}

	// 球形重叠检测
	bool QueryResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		LookPoint,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(ShockwaveRadius),
		QueryParams
	);

	if (QueryResult)
	{
		// 遍历检测结果，筛选出其中的敌人对象
		for (int i = 0; i < OverlapResults.Num(); i++)
		{
			APawn* Enemy = Cast<APawn>(OverlapResults[i].GetActor());
			if (Enemy && !OverlapActors.Contains(Enemy))
			{
				OverlapActors.Add(Enemy);
			}
		}
	}

	// 包装数据
	FGameplayAbilityTargetDataHandle ShockwaveTargetDataHandle;

	// 0号负载：处理位置信息
	FGameplayAbilityTargetData_LocationInfo* CenterLocation = new FGameplayAbilityTargetData_LocationInfo();
	CenterLocation->TargetLocation.LiteralTransform = FTransform(LookPoint);	
	CenterLocation->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	ShockwaveTargetDataHandle.Add(CenterLocation);

	// 1号负载：检测到的EnemyActors
	FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray();
	if (OverlapActors.Num() > 0)
	{
		ActorArray->SetActors(OverlapActors);
		ShockwaveTargetDataHandle.Add(ActorArray);
	}

	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		TargetDataReadyDelegate.Broadcast(ShockwaveTargetDataHandle);
	}

}

bool AShockwaveTargetActor::GetPlayerLookAtPoint(FVector& Out_LookPoint)
{
	// 获得射线检测起始点
	FVector ViewLocation;
	FRotator ViewRotation;
	PrimaryPC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// 检测结果
	FHitResult HitResult;

	// 检测参数
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;			// 开启复杂检测
	APawn* SelfPawn = PrimaryPC->GetPawn();
	if (SelfPawn)
	{
		QueryParams.AddIgnoredActor(SelfPawn);	// 忽略自身
	}

	// 射线检测
	bool bTraceResult = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ViewLocation,
		ViewLocation + ViewRotation.Vector() * 5000.0f,
		ECC_Visibility,
		QueryParams
	);
	if (bTraceResult)
	{
		Out_LookPoint = HitResult.ImpactPoint;
	}

	return bTraceResult;
}
