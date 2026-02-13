// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

UENUM()
enum class EMonsterAIState : uint8
{
	PatrolIdle,
	PatrolMove,
	Chase,
	Attack
};


UCLASS()
class AMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMonsterAIController();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	
	// Perception
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;
	
	UPROPERTY()
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
	
	UFUNCTION()
	void OnTargetPerception(AActor* Actor, struct FAIStimulus Stimulus);
	
	// Patrol/Chase
	void StartPatrolIdle();
	void StartPatrolMove();
	void StartChase(AActor* TargetActor);
	void StopChase();
	
	void SetMoveSpeed(float NewSpeed) const;
	
	// Callback for Move Complete
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	void OnLoseSightExpired();
	
private:
	UPROPERTY(EditDefaultsOnly, Category="AI|Patrol")
	float WanderRadius = 150.f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Patrol")
	float IdleMin = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Patrol")
	float IdleMax = 2.f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Move")
	float WalkSpeed = 150.f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Move")
	float RunSpeed = 300.f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Chase")
	float LoseSightDelay = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Chase")
	float ChaseAcceptRadius = 120.f;
	
	// NEW (Radius Aggro Driven)
	// -----Radius Detection-----
	UPROPERTY(EditDefaultsOnly, Category="AI|Aggro")
	float AggroRadius = 150.f;
	
	// if out of radius, Chase Persist Timer starts
	UPROPERTY(EditDefaultsOnly, Category="AI|Aggro")
	float AggroLoseRadius = 250.f;
	
	// Aggro Radius Check
	float AggroCheckInterval = 0.2f;
	
	// -----Chase Persistence-----
	UPROPERTY(EditDefaultsOnly, Category="AI|Chase")
	float ChasePersistSeconds = 3.0f; // Time following target when target just out of Radius
	
	// Resets MoveTo when chasing
	UPROPERTY(EditDefaultsOnly, Category="AI|Chase")
	float RepathInterval = 0.25f;
	
	// prevents leaving post too far
	UPROPERTY(EditDefaultsOnly, Category="AI|Chase")
	float MaxChaseDistanceFromHome = 3000.f;
	
	
	
	
	// State
	EMonsterAIState State = EMonsterAIState::PatrolIdle;
	
	FVector HomeLocation = FVector::ZeroVector;
	
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
	
	FTimerHandle Timer_Idle;
	FTimerHandle Timer_LoseSight;
	
	// NEW (Radius Aggro)
	FTimerHandle Timer_AggroCheck;
	FTimerHandle Timer_Repath;
	FTimerHandle Timer_ChaseExpire;
	
	void UpdateAggro();
	void RepathToTarget();
	APawn* GetPlayerPawn() const;
	void EnsureAggreTimer(); // Ensures aggro timer neven dies
	
	// -----Attack-----
	// if in range, attacks
	UPROPERTY(EditDefaultsOnly, Category="AI|Attack")
	float AttackRange = 230.f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Attack")
	float AttackInterval = 2.f;
	
	// Prevents stopping too far from Player
	UPROPERTY(EditDefaultsOnly, Category="AI|Attack")
	float AttackAcceptRadius = 110.f;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Attack")
	float AttackDamage = 10.f;
	
	FTimerHandle Timer_Attack;
	
	void EnterAttack(APawn* PlayerPawn);
	void ExitAttackToChase();
	void PerformAttackTick();
	bool IsInAttackRange(APawn* PlayerPawn) const;
};
