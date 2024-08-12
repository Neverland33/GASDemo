// Fill out your copyright notice in the Description page of Project Settings.


#include "IgniteTargetActor.h"
#include "Engine/OverlapResult.h"

void AIgniteTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	PrimaryPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());

}

void AIgniteTargetActor::ConfirmTargetingAndContinue()
{
	APawn* SelfPawn = PrimaryPC->GetPawn();
	FVector PlayerLocation;
	if (SelfPawn)
	{
		PlayerLocation = SelfPawn->GetActorLocation();
	}

	// 球形重叠检测的结果
	TArray<FOverlapResult> OverlapResults;
	// 检测到的敌人
	TArray<TWeakObjectPtr<AActor>> OverlapActors;

	// 检测参数
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;				// 不开启复杂检测
	QueryParams.bReturnPhysicalMaterial = false;	// 不检测物理材质
	
	if (SelfPawn)
	{
		QueryParams.AddIgnoredActor(SelfPawn);		// 忽略自身
	}

	// 球形重叠检测
	bool QueryResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		PlayerLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(IgniteRadius),
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

	// 0号负载：检测到的EnemyActors
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
