# CTIS_ZombieGame

A 2D/3D hybrid co-op survival game built in Unreal Engine 5 — 2D sprite characters with distinct skill sets navigating a fully 3D, destructible recreation of Bilkent University's CTIS faculty building.

**▶ [Gameplay video](https://youtu.be/PsBm4uJqYyc?si=z61Q71P6oKTqLKit)**

![CTIS_ZombieGame gameplay](https://github.com/Bahoyvs/CTIS_ZombieGame/assets/65951986/ff1c4491-1811-4c09-8905-001266ed2e8a)
<img width="1286" alt="In-game screenshot" src="https://github.com/Bahoyvs/CTIS_ZombieGame/assets/65951986/b1a57b25-8a54-46e0-b9de-d91ff342b164">
<img width="1286" alt="In-game screenshot" src="https://github.com/Bahoyvs/CTIS_ZombieGame/assets/65951986/db999a9b-6284-4213-9eaf-407445a86f8b">

---

## Tech Stack

| Layer | Technology |
| --- | --- |
| **Engine** | Unreal Engine 5.3 |
| **Languages** | C++ (runtime module, `UCLASS`/delegate/Blueprint-exposed API) + Blueprint visual scripting |
| **Networking** | Online Subsystem **EOS** (Epic Online Services) with `NetDriverEOS`, AdvancedSessions plugin for lobby/session management |
| **2D/3D hybrid** | PaperZD (sprite flipbooks + animation state machines inside a 3D world) |
| **Abilities** | Gameplay Ability System (GAS) |
| **Destruction** | Chaos — pre-fractured Geometry Collections for building walls |
| **VFX / Audio** | Niagara + Niagara Fluids, MidiEngineCore (MIDI-driven rhythm sequencing) |
| **Backend** | PlayFab |
| **Content** | 30 levels, 186 Blueprint/Widget/AnimBP assets, Quixel Megascans environment set |

---

## Core Mechanics & Features

- **Blends 2D characters with a 3D world.** Players control PaperZD sprite characters — each with a distinct skill set — through a fully 3D, Megascans-dressed environment, combining exploration, puzzle-solving and combat without collapsing into either a pure sidescroller or a pure 3D shooter.
- **Hosts co-op sessions over Epic Online Services.** Lobby creation, discovery, join and travel run through EOS with `NetDriverEOS` (falling back to `IpNetDriver`), wrapped by AdvancedSessions so session logic stays authorable in Blueprint.
- **Destroys the environment with Chaos.** Building walls ship as pre-fractured Geometry Collections rather than static meshes, so structural damage is simulated at runtime instead of swapped between damage states.
- **Drives a MIDI-synced rhythm portal** from a C++ beat-map scheduler: a beat map is parsed from disk into timed note-placement events, then dispatched to designer-authored Blueprint reactions on five lanes.

---

## Technical Architecture & Problem Solving

**The defining constraint is the hybrid itself.** A 2D sprite character in a 3D scene is not a rendering trick — it breaks assumptions across the whole stack. Sprites are camera-facing planes, so they need collision volumes that behave like 3D capsules while their visual representation stays flat; navmesh queries and vertical traversal happen in three dimensions while animation state is authored in two; and depth sorting must respect world geometry rather than a 2D layer index. PaperZD supplies the flipbook and animation-state-machine layer, but the integration work — reconciling 2D animation states with 3D movement, collision and camera framing inside a multi-floor interior — is where the project spends its complexity budget.

**The second hard problem is co-op over a real networking backend.** Rather than LAN-only or a single-player fake, the project targets Online Subsystem EOS with `NetDriverEOS` and an `IpNetDriver` fallback, which forces every interactive system to be replication-aware: session lifecycle (create → advertise → join → seamless travel), ability activation through GAS, and Chaos destruction state that must not diverge between clients. AdvancedSessions was chosen deliberately to keep session flow in Blueprint where iteration is fast, while the engine-facing plumbing stays in C++.

**The C++/Blueprint boundary is the architectural decision worth highlighting.** `ARhythmEventManager` is a compact demonstration of how the whole project is layered: C++ owns the work that is unsuitable for visual scripting — file I/O, string parsing of the beat map into an `FBeatData` array, and `FTimerManager`-driven scheduling — while every downstream reaction is exposed as a `BlueprintImplementableEvent` (`OnFarLeftBeat`, `OnLeftBeat`, `OnMiddleBeat`, `OnRightBeat`, `OnFarRightBeat`, `OnBeatMapEnd`) plus a `BlueprintAssignable` multicast delegate. A designer can re-author what a beat *does* — spawn, animate, damage, cue VFX — without recompiling the module, and the beat map itself is a plain text file, editable without the engine open. That separation (deterministic timing and data handling in C++, authored behaviour in Blueprint) is applied consistently across the gameplay systems.

---

## Installation / How to Play

**Requirements:** Unreal Engine **5.3**, Visual Studio 2022 with the *Game development with C++* workload, Windows.

```bash
git clone https://github.com/Bahoyvs/CTIS_ZombieGame.git
```

1. Right-click `CTIS_ZombieGame.uproject` → **Generate Visual Studio project files**.
2. Open the generated `.sln` and build the `CTIS_ZombieGameEditor` target (Development Editor).
3. Launch `CTIS_ZombieGame.uproject`. Boot level is `Content/MBLS/Maps/Boot_Level`.

**Marketplace plugins** (PaperZD, AsyncLoadingScreen, MinimapPlugin, PlayFab, MidiEngineCore, InteractionSystemPlugin, LowEntryExtStdLib) must be installed to your engine from the Epic Games Launcher before the project will compile — see the `Plugins` block in `CTIS_ZombieGame.uproject` for marketplace URLs.

**Co-op testing:** use **Play → Number of Players ≥ 2** with *Run Under One Process* disabled to exercise the EOS session path, or launch a packaged client against a hosted lobby.

---

## Repository Layout

```
Source/CTIS_ZombieGame/
  RhythmEventManager.{h,cpp}   beat-map parsing, timer scheduling, Blueprint event dispatch
  CTIS_ZombieGame.Build.cs     module dependencies
Plugins/AdvancedSessions/      session/lobby management (create, find, join, travel)
Config/                        engine, input, gameplay-tag and EOS configuration
Content/
  MBLS/Maps/                   boot, main menu, lobby, transition + 5 gameplay levels
  Characters/ SKILLS/          PaperZD sprite characters and ability sets
  Buildings/FractureMesh/      Chaos Geometry Collections (destructible walls)
  Portals/Levels/Rhythm_Game/  rhythm minigame + beat_map.txt
  Advanced_AI_SpawnSystem/     enemy spawn director
  Megascans/ Effects/ Movies/  environment art, Niagara VFX, cutscenes
```

---

## Status

Active development. Built collaboratively by a small team as a university project — the core loop (co-op session, 2D/3D traversal, combat, destructible environment and portal minigames) is playable end to end.
