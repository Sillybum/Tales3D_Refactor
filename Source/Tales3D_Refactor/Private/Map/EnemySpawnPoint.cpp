// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Map/EnemySpawnPoint.h"

#include "Char/CoreEnemy.h"

// Sets default values
AEnemySpawnPoint::AEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		SpawnNow();
	}
}

void AEnemySpawnPoint::SpawnNow()
{
	if (SpawnedMonster || !MonsterClass) return;

	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnedMonster = World->SpawnActor<ACoreEnemy>(MonsterClass, GetActorTransform(), Params);
		if (SpawnedMonster)
		{
			SpawnedMonster->OnDestroyed.AddDynamic(this, &AEnemySpawnPoint::OnSpawnedMonsterDestroyed);
		}
	}
}

void AEnemySpawnPoint::OnSpawnedMonsterDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor == SpawnedMonster)
	{
		SpawnedMonster = nullptr;
		ScheduleRespawn();
	}
}

void AEnemySpawnPoint::ScheduleRespawn()
{
	if (RespawnDelay <= 0.f)
	{
		SpawnNow();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AEnemySpawnPoint::SpawnNow, RespawnDelay, false);
	}
}
