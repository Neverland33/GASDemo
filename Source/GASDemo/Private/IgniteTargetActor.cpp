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

	// �����ص����Ľ��
	TArray<FOverlapResult> OverlapResults;
	// ��⵽�ĵ���
	TArray<TWeakObjectPtr<AActor>> OverlapActors;

	// ������
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;				// ���������Ӽ��
	QueryParams.bReturnPhysicalMaterial = false;	// ������������
	
	if (SelfPawn)
	{
		QueryParams.AddIgnoredActor(SelfPawn);		// ��������
	}

	// �����ص����
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
		// �����������ɸѡ�����еĵ��˶���
		for (int i = 0; i < OverlapResults.Num(); i++)
		{
			APawn* Enemy = Cast<APawn>(OverlapResults[i].GetActor());
			if (Enemy && !OverlapActors.Contains(Enemy))
			{
				OverlapActors.Add(Enemy);
			}
		}
	}

	// ��װ����
	FGameplayAbilityTargetDataHandle ShockwaveTargetDataHandle;

	// 0�Ÿ��أ���⵽��EnemyActors
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
