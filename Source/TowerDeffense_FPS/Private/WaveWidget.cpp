#include "WaveWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UWaveWidget::UpdateWaveText(int32 CurrentWave)
{
    if (WaveTextBlock)
    {
       WaveTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentWave)));
        //WaveTextBlock->SetText(FText::AsNumber(CurrentWave));
    }

    UE_LOG(LogTemp, Warning, TEXT("UpdateWaveText called: %d"), CurrentWave);
}

//#include "WaveWidget.h"
//
//// �ȉ���Y�ꂸ�ɒǉ��I
//#include "Components/TextBlock.h"
//#include "Components/CanvasPanel.h"
//#include "Components/CanvasPanelSlot.h"
//#include "Blueprint/WidgetTree.h"   // �� ���ꂪ�d�v�I�I
//#include "Fonts/SlateFontInfo.h"     // �� UE5�ł͂������I
//#include "Misc/Paths.h"             // FPaths::EngineContentDir �p
//
//void UWaveWidget::NativeConstruct()
//{
//    Super::NativeConstruct();
//
//    UE_LOG(LogTemp, Warning, TEXT("WaveWidget::NativeConstruct called!"));
//
//    // WidgetTree �� UUserWidget �����g�ݍ��ݕϐ�
//    if (!WidgetTree)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("WidgetTree is null in WaveWidget"));
//        return;
//    }
//
//    // Canvas�p�l���쐬
//    UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
//    WidgetTree->RootWidget = RootPanel;
//
//    // �e�L�X�g�쐬
//    WaveTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
//    WaveTextBlock->SetText(FText::FromString(TEXT("Wave 1 / 1")));
//    WaveTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
//    WaveTextBlock->SetJustification(ETextJustify::Center);
//    WaveTextBlock->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));
//
//    // Canvas�ɒǉ�
//    RootPanel->AddChild(WaveTextBlock);
//
//    // �X���b�g�ݒ�
//    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(WaveTextBlock->Slot))
//    {
//        CanvasSlot->SetAutoSize(true);
//        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.05f)); // �㕔����
//        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.0f));
//    }
//}
//
//void UWaveWidget::UpdateWaveText(int32 CurrentWave, int32 MaxWave)
//{
//    if (!WaveTextBlock)
//    {
//        UE_LOG(LogTemp, Error, TEXT("WaveTextBlock is null!"));
//        return;
//    }
//
//    FString Text = FString::Printf(TEXT("Wave %d / %d"), CurrentWave, MaxWave);
//    WaveTextBlock->SetText(FText::FromString(Text));
//
//    UE_LOG(LogTemp, Warning, TEXT("WaveWidget::UpdateWaveText called! %s"), *Text);
//}
