// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"

FGameplayAbilityInfo::FGameplayAbilityInfo()
	: CoolDown(0), CostType(ECostType::Mana), CostValue(0), IconMaterial(nullptr), AbilityClass(nullptr)
{ }

FGameplayAbilityInfo::FGameplayAbilityInfo(float CoolDown, ECostType CostType, float CostValue, 
											TObjectPtr<UMaterialInstance> IconMaterial, TSubclassOf<UBaseGameplayAbility> AbilityClass)
	: CoolDown(CoolDown), CostType(ECostType::Mana), CostValue(CostValue), IconMaterial(IconMaterial), AbilityClass(AbilityClass)
{ }

FGameplayAbilityInfo UBaseGameplayAbility::GetAbilityInfoByLevel(int level)
{
	// 默认值
	float CoolDown = 0;
	float CostValue = 0;
	ECostType CostType = ECostType::Mana;

	// 获取冷却和花费的GE
	UGameplayEffect* CoolDownEffect = GetCooldownGameplayEffect();
	UGameplayEffect* CostEffect = GetCostGameplayEffect();
	if (CoolDownEffect && CostEffect)
	{
		// 获取冷却值
		CoolDownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(level, CoolDown);
		// 获取花费类型和值
		if (CostEffect->Modifiers.Num() > 0)	// GE有Modifier
		{
			// 获取第一个Modifier
			FGameplayModifierInfo CostEffectModifierInfo = CostEffect->Modifiers[0];
			// 获取花费值
			CostEffectModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(level, CostValue);
			// 通过名称获取花费类型
			FString CostTypeName = CostEffectModifierInfo.Attribute.AttributeName;
			if (CostTypeName == "Health")
			{
				CostType = ECostType::Health;
			}
			if (CostTypeName == "Mana")
			{
				CostType = ECostType::Mana;
			}
			if (CostTypeName == "Strength")
			{
				CostType = ECostType::Strength;
			}
		}
	}

	return FGameplayAbilityInfo(CoolDown, CostType, CostValue, IconMaterial, GetClass());
}
