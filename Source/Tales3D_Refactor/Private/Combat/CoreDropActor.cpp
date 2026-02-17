// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Combat/CoreDropActor.h"

#include "Char/CoreCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Framework/CorePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/DropItemNameWidget.h"

ACoreDropActor::ACoreDropActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetSimulatePhysics(true);

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(RootComponent);
	NameWidget->SetWidgetSpace(EWidgetSpace::Screen);
	NameWidget->SetDrawSize(FVector2D(180.f, 36.f));
	NameWidget->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	NameWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ACoreDropActor::OnMeshBeginOverlap);
}

void ACoreDropActor::BeginPlay()
{
	Super::BeginPlay();
	ApplyItemNameToWidget();
	UpdateNameWidgetWorldLocation();
}

void ACoreDropActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateNameWidgetWorldLocation();
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

void ACoreDropActor::UpdateNameWidgetWorldLocation()
{
	if (!NameWidget) return;
	NameWidget->SetWorldLocation(GetActorLocation() + FVector(0.f, 0.f, NameWidgetHeight));
}

void ACoreDropActor::OnMeshBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bPickedUp) return;

	ACoreCharacter* Character = Cast<ACoreCharacter>(OtherActor);
	if (!Character || !Character->IsPlayerControlled()) return;

	if (ACorePlayerController* PC = Cast<ACorePlayerController>(Character->GetController()))
	{
		PC->ShowPickupMessage(ItemName);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	bPickedUp = true;
	Destroy();
}

