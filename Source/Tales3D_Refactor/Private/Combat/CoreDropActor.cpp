// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Combat/CoreDropActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/DropItemNameWidget.h"

ACoreDropActor::ACoreDropActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Mesh->SetEnableGravity(true);
	Mesh->SetSimulatePhysics(true);

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(RootComponent);
	NameWidget->SetWidgetSpace(EWidgetSpace::Screen);
	NameWidget->SetDrawSize(FVector2D(180.f, 36.f));
	NameWidget->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	NameWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACoreDropActor::BeginPlay()
{
	Super::BeginPlay();
	ApplyItemNameToWidget();
}

void ACoreDropActor::ApplyItemNameToWidget()
{
	if (!NameWidget) return;

	if (NameWidgetClass && NameWidget->GetWidgetClass() != NameWidgetClass)
	{
		NameWidget->SetWidgetClass(NameWidgetClass);
	}
	NameWidget->InitWidget();

	if (UDropItemNameWidget* W = Cast<UDropItemNameWidget>(NameWidget->GetUserWidgetObject()))
	{
		W->SetItemName(ItemName);
	}
}

