// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreDropActor.generated.h"

UCLASS()
class ACoreDropActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoreDropActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<class UStaticMeshComponent> Mesh;

};
