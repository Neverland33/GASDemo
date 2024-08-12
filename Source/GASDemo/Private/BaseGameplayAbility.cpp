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
	// Ĭ��ֵ
	float CoolDown = 0;
	float CostValue = 0;
	ECostType CostType = ECostType::Mana;

	// ��ȡ��ȴ�ͻ��ѵ�GE
	UGameplayEffect* CoolDownEffect = GetCooldownGameplayEffect();
	UGameplayEffect* CostEffect = GetCostGameplayEffect();
	if (CoolDownEffect && CostEffect)
	{
		// ��ȡ��ȴֵ
		CoolDownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(level, CoolDown);
		// ��ȡ�������ͺ�ֵ
		if (CostEffect->Modifiers.Num() > 0)	// GE��Modifier
		{
			// ��ȡ��һ��Modifier
			FGameplayModifierInfo CostEffectModifierInfo = CostEffect->Modifiers[0];
			// ��ȡ����ֵ
			CostEffectModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(level, CostValue);
			// ͨ�����ƻ�ȡ��������
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
