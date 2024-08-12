// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayEffectTypes.h"
#include "BaseGameplayAbility.h"
#include "GASDemoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/* 在属性变化时广播的多播代理 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangeEvent, float, NewValue);

UCLASS(config=Game)
class AGASDemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	AGASDemoCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	/* 声明生命值改变多播代理 */
	UPROPERTY(BlueprintAssignable, Category="Ability")
	FOnAttributeChangeEvent HealthChangeEvent;

	void OnHealthAttributeValueChange(const FOnAttributeChangeData& Data);

	/* 声明法力值改变多播代理 */
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FOnAttributeChangeEvent ManaChangeEvent;

	void OnManaAttributeValueChange(const FOnAttributeChangeData& Data);

	/* 声明能量值改变多播代理 */
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FOnAttributeChangeEvent StrengthChangeEvent;

	void OnStrengthAttributeValueChange(const FOnAttributeChangeData& Data);

	/* 学习技能时获取技能信息 */
	UFUNCTION(BlueprintCallable, Category="BaseCharacter")
	FGameplayAbilityInfo GetGameplayAbilityInfo(TSubclassOf<UBaseGameplayAbility> AbilityClass, int level);
};

