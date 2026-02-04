// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Char/CoreEnemy.h"

#include "Component/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/EnemyHPBarWidget.h"

ACoreEnemy::ACoreEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// HP Widget (Attaches on Root)
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(RootComponent);
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, -200.f));
	// Makes it face screen
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarWidget->SetDrawSize(FVector2D(120.f, 16.f));
	// Hidden until enemy selected
	HPBarWidget->SetVisibility(false);
	
	// Health Component
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void ACoreEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HPBarWidget)
	{
		HPBarWidget->InitWidget();
	}
	
	if (Health)
	{
		Health->OnHealthChanged.AddDynamic(this, &ACoreEnemy::HandleHealthChanged);
	}
}

void ACoreEnemy::SetSelected(bool bSelected)
{
	if (bIsSelected == bSelected) return;
	
	bIsSelected = bSelected;
	UpdateHPBarVisibility();
}

void ACoreEnemy::HandleHealthChanged(float NewHP, float MaxHP)
{
	if (UEnemyHPBarWidget* W = GetHPBarWidget())
	{
		W->SetHP(NewHP, MaxHP);
	}
}

UEnemyHPBarWidget* ACoreEnemy::GetHPBarWidget() const
{
	if (!HPBarWidget) return nullptr;
	
	return Cast<UEnemyHPBarWidget>(HPBarWidget->GetUserWidgetObject());
}

void ACoreEnemy::UpdateHPBarVisibility()
{
	if (!HPBarWidget) return;

	if (HPBarWidgetClass && HPBarWidget->GetWidgetClass() != HPBarWidgetClass)
	{
		HPBarWidget->SetWidgetClass(HPBarWidgetClass);
	}
	HPBarWidget->SetVisibility(bIsSelected);
}
