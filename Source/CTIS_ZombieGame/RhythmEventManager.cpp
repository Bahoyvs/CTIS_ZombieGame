#include "RhythmEventManager.h"
#include "TimerManager.h"

// Sets default values
ARhythmEventManager::ARhythmEventManager()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set a default file path relative to the Content folder
    BeatMapFilePath = FPaths::ProjectContentDir() + TEXT("Portals/Levels/Rhythm_Game/Assets/beat_map.txt");

    CurrentBeatIndex = 0;
    TimeAccumulator = 0.f;
}

// Called when the game starts or when spawned
void ARhythmEventManager::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ARhythmEventManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Load and parse the beat map data
void ARhythmEventManager::LoadBeatMap(const TArray<FString>& BeatMapData)
{
    for (const FString& Line : BeatMapData)
    {
        TArray<FString> ParsedData;
        Line.ParseIntoArray(ParsedData, TEXT(", "), true);

        if (ParsedData.Num() == 2)
        {
            FBeatData Beat;
            Beat.TimeAfterPreviousBeat = FCString::Atof(*ParsedData[0]);
            Beat.NotePlacement = ParsedData[1];
            BeatMap.Add(Beat);
        }
    }
}

// Schedule events for each beat
void ARhythmEventManager::ScheduleBeatEvents()
{
    for (const FBeatData& Beat : BeatMap)
    {
        FTimerHandle UnusedHandle;
        GetWorldTimerManager().SetTimer(UnusedHandle, [this, Beat]()
        {
            HandleBeatEvent(Beat.NotePlacement);
        }, Beat.TimeAfterPreviousBeat, false);
    }
}

// Handle the beat event based on note placement
void ARhythmEventManager::HandleBeatEvent(const FString& NotePlacement)
{
    OnNotePlacementEvent.Broadcast(NotePlacement);

    // Trigger Blueprint event based on note placement
    if (NotePlacement == "far-left")
    {
        OnFarLeftBeat();
    }
    else if (NotePlacement == "left")
    {
        OnLeftBeat();
    }
    else if (NotePlacement == "middle")
    {
        OnMiddleBeat();
    }
    else if (NotePlacement == "right")
    {
        OnRightBeat();
    }
    else if (NotePlacement == "far-right")
    {
        OnFarRightBeat();
    }
}

void ARhythmEventManager::LoadBeatMapFromFile()
{
    FString FileContents;

    if (LoadFileToString(BeatMapFilePath, FileContents))
    {
        // Clear any existing beat data
        BeatMap.Empty();

        // Parse the file content line by line
        TArray<FString> Lines;
        FileContents.ParseIntoArrayLines(Lines);

        for (const FString& Line : Lines)
        {
            TArray<FString> ParsedData;
            Line.ParseIntoArray(ParsedData, TEXT(", "), true);

            if (ParsedData.Num() == 2)
            {
                FBeatData Beat;
                Beat.TimeAfterPreviousBeat = FCString::Atof(*ParsedData[0]);
                Beat.NotePlacement = ParsedData[1];
                BeatMap.Add(Beat);
            }
        }

        // Start handling beats if we have valid data
        if (BeatMap.Num() > 0)
        {
            CurrentBeatIndex = 0;
            HandleNextBeat();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load beat map from file: %s"), *BeatMapFilePath);
    }
}

bool ARhythmEventManager::LoadFileToString(const FString& FilePath, FString& FileContents)
{
    return FFileHelper::LoadFileToString(FileContents, *FilePath);
}


// Handle the next beat event and schedule the next one
void ARhythmEventManager::HandleNextBeat()
{
    if (CurrentBeatIndex >= BeatMap.Num())
    {
        // We've reached the end of the beat map, trigger the end event in Blueprint
        OnBeatMapEnd();
        return;
    }

    // Get the current beat
    FBeatData& CurrentBeat = BeatMap[CurrentBeatIndex];

    // Trigger the event for the current beat
    if (CurrentBeat.NotePlacement == "far-left")
    {
        OnFarLeftBeat();  // Assuming OnFarLeftBeat() is implemented in Blueprint
    }
    else if (CurrentBeat.NotePlacement == "left")
    {
        OnLeftBeat();
    }
    else if (CurrentBeat.NotePlacement == "middle")
    {
        OnMiddleBeat();
    }
    else if (CurrentBeat.NotePlacement == "right")
    {
        OnRightBeat();
    }
    else if (CurrentBeat.NotePlacement == "far-right")
    {
        OnFarRightBeat();
    }

    // Schedule the next beat after the appropriate delay
    GetWorldTimerManager().SetTimer(BeatTimerHandle, this, &ARhythmEventManager::HandleNextBeat, CurrentBeat.TimeAfterPreviousBeat, false);

    // Move to the next beat
    CurrentBeatIndex++;
}