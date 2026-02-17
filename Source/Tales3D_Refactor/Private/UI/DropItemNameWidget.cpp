// TalesWeaver Clone Project (GitHub:Sillybum)


#include "UI/DropItemNameWidget.h"
#include "Components/TextBlock.h"

void UDropItemNameWidget::SetItemName(const FText& NewItemName)
{
	if (!Text_ItemName) return;
	Text_ItemName->SetText(NewItemName);
}

