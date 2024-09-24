#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RhythmEventManager.generated.h"

// Declare a delegate for triggering different events based on note placement
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotePlacementEvent, FString, NotePlacement);

UCLASS()
class CTIS_ZOMBIEGAME_API ARhythmEventManager : public AActor
{
    GENERATED_BODY()

public:    
    // Sets default values for this actor's properties
    ARhythmEventManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Load and parse the beat map data
    UFUNCTION(BlueprintCallable, Category = "Rhythm")
    void LoadBeatMap(const TArray<FString>& BeatMapData);

    // Schedule events for each beat
    void ScheduleBeatEvents();

    // Handle the beat event based on note placement
    void HandleBeatEvent(const FString& NotePlacement);

    // Delegate for note placement events
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FNotePlacementEvent OnNotePlacementEvent;

    // Load and parse the beat map data from the file path
    UFUNCTION(BlueprintCallable, Category = "Rhythm")
    void LoadBeatMapFromFile();

    // File path for the beat map
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
    FString BeatMapFilePath;

    // Blueprint event to trigger when the beat map has ended
    UFUNCTION(BlueprintImplementableEvent, Category = "Rhythm")
    void OnBeatMapEnd();


    // Blueprint events for different note placements
    UFUNCTION(BlueprintImplementableEvent, Category = "Rhythm")
    void OnFarLeftBeat();

    UFUNCTION(BlueprintImplementableEvent, Category = "Rhythm")
    void OnLeftBeat();

    UFUNCTION(BlueprintImplementableEvent, Category = "Rhythm")
    void OnMiddleBeat();

    UFUNCTION(BlueprintImplementableEvent, Category = "Rhythm")
    void OnRightBeat();

    UFUNCTION(BlueprintImplementableEvent, Category = "Rhythm")
    void OnFarRightBeat();

private:
    // Structure to store a beat with timing and note placement
    struct FBeatData
    {
        float TimeAfterPreviousBeat;
        FString NotePlacement;
    };

    // Array to store all beats from the map
    TArray<FBeatData> BeatMap;

    // Timer to track time between beats
    FTimerHandle BeatTimerHandle;

    // Current beat index
    int32 CurrentBeatIndex;

    // Helper function to load file content
    bool LoadFileToString(const FString& FilePath, FString& FileContents);

    // Handle individual beat events
    void HandleNextBeat();

    // Time accumulator for tracking beats
    float TimeAccumulator;
};
