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
//// 以下を忘れずに追加！
//#include "Components/TextBlock.h"
//#include "Components/CanvasPanel.h"
//#include "Components/CanvasPanelSlot.h"
//#include "Blueprint/WidgetTree.h"   // ← これが重要！！
//#include "Fonts/SlateFontInfo.h"     // ← UE5ではこっち！
//#include "Misc/Paths.h"             // FPaths::EngineContentDir 用
//
//void UWaveWidget::NativeConstruct()
//{
//    Super::NativeConstruct();
//
//    UE_LOG(LogTemp, Warning, TEXT("WaveWidget::NativeConstruct called!"));
//
//    // WidgetTree は UUserWidget が持つ組み込み変数
//    if (!WidgetTree)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("WidgetTree is null in WaveWidget"));
//        return;
//    }
//
//    // Canvasパネル作成
//    UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
//    WidgetTree->RootWidget = RootPanel;
//
//    // テキスト作成
//    WaveTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
//    WaveTextBlock->SetText(FText::FromString(TEXT("Wave 1 / 1")));
//    WaveTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
//    WaveTextBlock->SetJustification(ETextJustify::Center);
//    WaveTextBlock->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));
//
//    // Canvasに追加
//    RootPanel->AddChild(WaveTextBlock);
//
//    // スロット設定
//    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(WaveTextBlock->Slot))
//    {
//        CanvasSlot->SetAutoSize(true);
//        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.05f)); // 上部中央
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
