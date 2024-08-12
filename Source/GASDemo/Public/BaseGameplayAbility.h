// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameplayAbility.generated.h"

class UMaterialInstance;
class UBaseGameplayAbility;

/* 技能消耗类型枚举 */
UENUM(BlueprintType)
enum class ECostType : uint8
{
	Health,
	Mana,
	Strength
};

/* 技能信息结构体 */
USTRUCT(BlueprintType)
struct FGameplayAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	float CoolDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	ECostType CostType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	float CostValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	TObjectPtr<UMaterialInstance> IconMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	TSubclassOf<UBaseGameplayAbility> AbilityClass;

	/* 构造 */
	FGameplayAbilityInfo();
	FGameplayAbilityInfo(float CoolDown, ECostType CostType, float CostValue, TObjectPtr<UMaterialInstance> IconMaterial, TSubclassOf<UBaseGameplayAbility> AbilityClass);

};

/**
 *
 */
UCLASS()
class GASDEMO_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilityInfo")
	TObjectPtr<UMaterialInstance> IconMaterial;

	/* 根据等级获取信息 */
	UFUNCTION(BlueprintCallable, Category="AbilityInfo")
	FGameplayAbilityInfo GetAbilityInfoByLevel(int level);
	
};
