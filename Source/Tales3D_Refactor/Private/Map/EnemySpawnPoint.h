// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "EnemySpawnPoint.generated.h"

class ACoreEnemy;

UCLASS()
class AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn")
	TSubclassOf<ACoreEnemy> MonsterClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn")
	float RespawnDelay = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn")
	bool bSpawnOnBeginPlay = true;
	
	UFUNCTION(BlueprintCallable, Category="Spawn")
	void SpawnNow();

private:
	UPROPERTY(Transient)
	TObjectPtr<ACoreEnemy> SpawnedMonster;
	FTimerHandle RespawnTimerHandle;
	
	UFUNCTION()
	void OnSpawnedMonsterDestroyed(AActor* DestroyedActor);
	void ScheduleRespawn();

};
