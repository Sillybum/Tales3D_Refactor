// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Combat/CoreDropActor.h"
#include "Components/StaticMeshComponent.h"

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
}

