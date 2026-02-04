// TalesWeaver Clone Project (GitHub:Sillybum)


#include "UI/EnemyHPBarWidget.h"

#include "Components/ProgressBar.h"

void UEnemyHPBarWidget::SetHP(float NewHP, float NewMaxHP)
{
	if (!ProgressBar_HP) return;
	
	const float Percent = (NewMaxHP <= 0.f) ? 0.f : FMath::Clamp(NewHP/NewMaxHP, 0.f, 1.f);
	ProgressBar_HP->SetPercent(Percent);
}
