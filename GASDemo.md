# GAS学习笔记

参考视频：【UE5.3 GAS入门教程重置版】https://www.bilibili.com/video/BV1Jx4y1Z7ig?p=16&vd_source=d3d25f7c63c499bfba14fbc3face9775

## 创建项目并导入资源

使用第三人称模板

导入商城中ParagonShinbi

导入UP给的资源

---

## 一些基础设置

复制一个BP_ThirdPerson作为BaseCharacter，创建两个派生类PlayerCharacter和EnemyCharacter，应用Shinbi的Mesh（其中的一个材质会出现乱飘的bug，替换为透明材质）和动画蓝图，添加一个CapsuleCollision，作为武器的碰撞检测，常态不开启碰撞

创建一个GameMode蓝图和一个PlayerController蓝图，并设置默认

---

## 创建一个平A及CD

### 创建平A

先为BaseCharacter添加一个AbilitySystem组件

创建一个BP_BaseAbility

创建一个GA_Melee继承自BaseAbility，添加一个标签

<img src="C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807153949543.png" alt="image-20240807153949543"  />

在BaseCharacter的BeginPlay()中学习技能（平A为Player和Enemy共有）

<img src="C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807154151696.png" alt="image-20240807154151696"  />

创建一个函数用于激活技能

<img src="C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807154318388.png" alt="image-20240807154318388" style="zoom:80%;" />

创建一个自定义事件，激活平A

![image-20240807154500121](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807154500121.png)

在PlayerController中调用该事件

![image-20240807154605012](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807154605012.png)

GA中逻辑

![image-20240807154705032](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807154705032.png)

### 平A中的碰撞处理

MontageToPlay中为导入的ParagonShinbi中创建好的MeleeMontage，创建一个AnimNotifyState蓝图ANS_CollisionMelee，在重写函数NotifyBegin和NotifyEnd中开启和关闭武器的碰撞

![image-20240807155623798](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807155623798.png)

（End中同理）

在Montage中添加NotifyState

![image-20240807155756304](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807155756304.png)

### 添加CD

创建GE_MeleeCD，并进行配置，最后在GA逻辑中要Commit

![image-20240807160529705](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807160529705.png)

![image-20240807160502192](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807160502192.png)



---

## 创建角色属性

创建一个C++类BaseAttributeSet，继承自AttributeSet

```c++
/* BaseAttributeSet.h */
// 引入头文件
#include "AbilitySystemComponent.h"
// 基类中提供的辅助宏
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
// 各项属性
public:
	// 生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health);
	// 最大生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth);
	// 法力值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Mana);
	// 最大法力值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxMana);
	// 能量值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Strength);
	// 最大能量值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
	FGameplayAttributeData MaxStrength;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxStrength);
```

创建一个DT_CharacterAttribute，在其中填写属性值

![image-20240807162005211](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807162005211.png)

为BaseCharacter的AbilitySystem添加AttributeSet

![image-20240807162108872](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807162108872.png)



---

## 平A伤害

创建一个GE_MeleeDamage，做如下配置

![image-20240807162551555](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807162551555.png)

在Player和Enemy的武器碰撞组件的重叠事件中应用GE（通过ActorTag检测重叠）

![image-20240807162717147](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807162717147.png)

![image-20240807162744725](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807162744725.png)

### 溢出处理

在BaseAttributeSet中重写基类中后处理函数，对属性做夹值处理

```c++
void UBaseAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0, GetMaxMana()));
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetStrength(FMath::Clamp(GetStrength(), 0.0, GetMaxStrength()));
	}

}
```

### 伤害的配表

本地创建.csv文件，第一行第一列为空，后续列为等级（从0开始），第二行第一列为名称，后续列为值

将文件拖入编辑器，选择Import As：CurveTable

在GE_MeleeDamage使用该表

![image-20240807164113021](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164113021.png)



---

## 敌人的行为树

为Enemy创建BehaviorTree、Blackboard、AIController，为BP_EnemyCharacter设置AIController，在AIController中Run BehaviorTree

![image-20240807164532118](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164532118.png)

![image-20240807164621128](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164621128.png)

![image-20240807164635219](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164635219.png)

BT_Enemy：

![image-20240807164712909](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164712909.png)

BTT_FindPlayer：

![image-20240807164807135](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164807135.png)

![image-20240807164821953](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164821953.png)

BTT_MeleeAttack：

![image-20240807164853700](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807164853700.png)



---

## 生命值改变时广播

```c++
/* GASDemoCharacter.h */
/* 在生命值变化时广播的多播代理 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangeEvent, float, NewValue);

public:
	/* 声明生命值改变多播代理 */
	UPROPERTY(BlueprintAssignable, Category="Ability")
	FOnHealthChangeEvent HealthChangeEvent;

	void OnHealthAttributeValueChange(const FOnAttributeChangeData& Data);
```

```c++
/* GASDemoCharacter.cpp */
void AGASDemoCharacter::BeginPlay()
{
    // 在Ability system component中，为提供的属性值改变委托添加函数，在函数中广播生命值改变
    UAbilitySystemComponent* SelfAbilitySystemComponent = this->FindComponentByClass<UAbilitySystemComponent>();
    if (SelfAbilitySystemComponent)
    {
        SelfAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetHealthAttribute()).AddUObject(this, &AGASDemoCharacter::OnHealthAttributeValueChange);
    }
}
/* 生命值改变时广播 */
void AGASDemoCharacter::OnHealthAttributeValueChange(const FOnAttributeChangeData& Data)
{
	HealthChangeEvent.Broadcast(Data.NewValue);
}
```

在BP_BaseCharacter的BeginPlay中绑定委托

![image-20240807170815733](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807170815733.png)



---

## 死亡处理

在BP_BaseCharacter中创建事件SetIsDead

![image-20240807171126578](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807171126578.png)

在HealthChange事件中调用该事件

![image-20240807171227293](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807171227293.png)

在BP_PlayerCharacter和BP_EnemyCharacter中分别重写HealthChange事件（注意都要先调用基类事件）

BP_PlayerCharacter：

![image-20240807171344429](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807171344429.png)

BP_EnemyCharacter：

![image-20240807171711331](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807171711331.png)

![image-20240807171716147](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807171716147.png)



---

## UI制作（略）

制作一个敌人血条添加到BP_EnemyCharacter的Widget Component，在生命值改变时更新

![image-20240807172203644](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807172203644.png)

制作一个玩家状态栏，在生命值改变时更新

![image-20240807172218745](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807172218745.png)

在PlayerController的BeginPlay中将其添加到视口

![image-20240807172330225](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807172330225.png)



---

## 被动回复

创建一个GA_BaseRegen，添加一个Cost GE：GE_BaseRegen，在GE中做如下配置

![image-20240807172843017](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807172843017.png)

![image-20240807172948755](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807172948755.png)

在GA中Commit并在BP_PlayerCharacter的BeginPlay中学习并激活技能

![image-20240807173102169](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807173102169.png)

![image-20240807173119638](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240807173119638.png)



---

## 初始化技能信息

### 构建技能信息结构体

创建一个C++类BaseGameplayAbility，继承自UGameplayAbility（注意要将之前创建的BP_BaseAbility的父类改为自己写的BaseGameplyAbility ）

创建一个技能信息结构体

```c++
/* BaseGameplayAbility.h */
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

/* BaseGameplayAbility类 */
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
```

```c++
/* BaseGameplayAbility.cpp */
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
```

### 创建技能UI

创建WBP_AbilitySlot

其中编写初始化及在CD时更新等逻辑

#### 根据技能信息初始化

![image-20240812152033065](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812152033065.png)

#### 在CD开始时更新

![image-20240812152321970](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812152321970.png)

在BP_BaseAbility中调用

![image-20240812153228343](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812153228343.png)

#### 在CD中时在Tick()中每帧更新

![image-20240812152401786](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812152401786.png)

![image-20240812152420211](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812152420211.png)

### 初始化

在玩家角色的BeginPlay()获得技能时进行初始化

![image-20240812151616388](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812151616388.png)

函数InitAbilityInfo将信息传送给UI进行初始化

![image-20240812151951520](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812151951520.png)



---

## 标准化创建技能流程

1. 创建一个GA，继承自BP_BaseAbility，设置GA的Icon和Tag
2. 为该GA创建GE_Cost和GE_Cooldown，做相应配置
3. 在BP_PlayerCharacter中将GAclass加入AbilityClassArr（该数组用于获取技能时初始化信息），将Tag加入GameplayTagContainer（该数组为基类创建，用于激活技能）
4. 在BP_PlayerCharacter中编写自定义事件，用于激活技能
5. 在BP_PlayerController中定义输入事件，调用4中编写的自定义事件
6. 在GA中编写技能逻辑



---

## 技能1：Healing

回复生命值

GA_Healing

按键后播放Montage，给玩家施加一个瞬间回复生命值的GE，在Montage播放完后结束

![image-20240812154817790](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812154817790.png)

GE_Healing

![image-20240812154915867](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812154915867.png)

创建一个GameplayCue，将tag加给cost，在释放时生成一个粒子特效

![image-20240812155325396](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812155325396.png)

![image-20240812155346383](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812155346383.png)



---

## 技能2：Dash

向前冲刺，击退碰到的敌人并造成伤害

GA_Dash

对玩家施加一个推力，播放Montage，等待消息将接触的敌人击退（消息通过玩家一个SphereCollision的BeginOverlap事件发送）

![image-20240812155528844](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812155528844.png)

![image-20240812155624024](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812155624024.png)

![image-20240812155639820](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812155639820.png)

消息的发送

![image-20240812160020329](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812160020329.png)

在AnimNotifyState中控制DashCollision碰撞检测的开启和关闭



---

## 技能3：Laser

按住持续释放一束向前的激光，对接触的敌人持续造成伤害，松开按键结束施放

创建一个BP_LaserActor

添加骨骼网格体和一个碰撞



在BP_PlayerCharacter中创建一个事件分发器，用于控制结束技能，在GA中绑定事件

![image-20240812161514492](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812161514492.png)

在BP_PlayerController中按键抬起时call

![image-20240812161614908](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812161614908.png)



GA_Laser

![image-20240812161902374](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812161902374.png)

![image-20240812161917098](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812161917098.png)

消息在LaserActor的BeginOverlap时发送（delay是为了在发送前已经在wait）

![image-20240812162053858](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812162053858.png)



---

## 技能4：Shockwave

按键进入选取状态，再次按键取消选取，鼠标左键确认选取，在选取区域释放一个冲击波，击退敌人并造成伤害

创建一个C++类AShockwaveTargetActor，继承自AGameplayAbilityTargetActor

```c++
/* ShockwaveTargetActor.h */
// 重写两个父类函数
// 开始选取时
virtual void StartTargeting(UGameplayAbility* Ability) override;
// 包装并传送选取信息
virtual void ConfirmTargetingAndContinue() override;

// 获取摄像机看向的位置
UFUNCTION(BlueprintCallable, Category="ShockwaveTarget")
bool GetPlayerLookAtPoint(FVector &Out_LookPoint);

// 选取区域的半径
UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true), Category="Default")
float ShockwaveRadius;
```

```c++
/* ShockwaveTargetActor.cpp */
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
	
    // 父类相同操作
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
```

继承一个蓝图类BP_ShockwaveTargetActor，添加一个贴花，在Tick()中设置其位置

![image-20240812163858917](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812163858917.png)

BP_PlayerController和BP_PlayerCharacter中

![image-20240812164007828](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812164007828.png)

![image-20240812164028240](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812164028240.png)

![image-20240812164045437](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812164045437.png)

GA_Shockwave

![image-20240812164130029](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812164130029.png)

在WaitTargetData节点中选择创建的BP_ShockwaveTargetActor

![image-20240812164252519](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812164252519.png)

GetTargetDataEndPoint和GetActorsFromTargetData分别获取包装的0号和1号负载

![image-20240812164311580](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812164311580.png)



---

## 技能5：Ignite

对周围的敌人瞬间造成一次伤害，并施加一个点燃状态，持续造成伤害，一段时间后结束

同样创建一个IgniteTargetActor用于施放时检测周围敌人

```c++
/* IgniteTargetActor.cpp */
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
```

GA_Ignite

![image-20240812165047961](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812165047961.png)

分别创建一个GE_Ignite_Damage_Instant和GE_Ignite_Damage_Last

在Instant中加入Last

![image-20240812165227250](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812165227250.png)

创建一个GC继承自GameplayCueNotify_Looping

![image-20240812165445279](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812165445279.png)

在GE_Ignite_Damage_Last中加入该Cue的Tag

![image-20240812165534669](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20240812165534669.png)