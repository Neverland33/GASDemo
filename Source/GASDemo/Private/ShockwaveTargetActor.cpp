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
	// ����ӽǿ����λ��
	FVector LookPoint;
	GetPlayerLookAtPoint(LookPoint);

	// �����ص����Ľ��
	TArray<FOverlapResult> OverlapResults;
	// ��⵽�ĵ���
	TArray<TWeakObjectPtr<AActor>> OverlapActors;

	// ������
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;				// ���������Ӽ��
	QueryParams.bReturnPhysicalMaterial = false;	// ������������
	APawn* SelfPawn = PrimaryPC->GetPawn();
	if (SelfPawn)
	{
		QueryParams.AddIgnoredActor(SelfPawn);		// ��������
	}

	// �����ص����
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

	// 0�Ÿ��أ�����λ����Ϣ
	FGameplayAbilityTargetData_LocationInfo* CenterLocation = new FGameplayAbilityTargetData_LocationInfo();
	CenterLocation->TargetLocation.LiteralTransform = FTransform(LookPoint);	
	CenterLocation->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	ShockwaveTargetDataHandle.Add(CenterLocation);

	// 1�Ÿ��أ���⵽��EnemyActors
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
	// ������߼����ʼ��
	FVector ViewLocation;
	FRotator ViewRotation;
	PrimaryPC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// �����
	FHitResult HitResult;

	// ������
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;			// �������Ӽ��
	APawn* SelfPawn = PrimaryPC->GetPawn();
	if (SelfPawn)
	{
		QueryParams.AddIgnoredActor(SelfPawn);	// ��������
	}

	// ���߼��
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
