# Project Vestige play-testing

The one place for what to check by hand in the editor.

- **[Before you test](#1-before-you-test)**: rebuilding, the automated tests, starting the game and handling saves.
- **[Pending checks](#2-pending-checks)**: what recent changes still need you to look at. Work through it after pulling or building new code.
- **[Smoke test](#3-smoke-test)**: a five-minute pass over every system, before pushing and at the end of each milestone.
- **[What isn't manual](#4-what-isnt-manual)**: what the automated tests cover instead.

---

## 1. Before you test

- **New or moved source files:** regenerate the Visual Studio project files (right-click `Vestige.uproject` in Explorer > Generate Visual Studio project files), so the solution shows them.
- **C++ that adds classes, `UPROPERTY`s or `UFUNCTION`s:** close the editor and rebuild `VestigeEditor` (Development Editor, Win64) in Visual Studio. Live Coding can't add those; it only patches function bodies.
- **Run the automated tests first.** In the editor: Tools > Test Automation, filter on `Vestige`, tick it and press Start Tests. Or from a command prompt with the editor closed:

  ```
  "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "C:\Unreal Projects\Vestige\Vestige.uproject" -ExecCmds="Automation RunTests Vestige; Quit" -unattended -nopause -nullrhi -nosplash -testexit="Automation Test Queue Empty"
  ```

  Every test logs `Result={Success}`. Two `LogAutomationTest: Error: Condition failed` lines at startup come from the engine and can be ignored.
- **Start the game:** open `/Game/Maps/L_Farm` (Content Browser > `Content/Maps`, double-click), then Play (Alt+P) for Play in Editor. For what players get, and for any performance numbers, use the arrow next to Play > Standalone Game instead.
- **Scalability:** Play in Editor uses the viewport's own setting: the viewport toolbar's Settings menu (top left of the viewport) > Engine Scalability Settings. New players start at **High**; **Epic** adds hardware ray tracing. Standalone and packaged builds use the game's settings.
- **Saves** live in `Saved/SaveGames/`: `Farm.sav` and `Farm2.sav` are the two slots saves alternate between, and `Farm_Version0.sav` is the backup of a save from before save versions. Before testing anything that writes saves, copy the folder somewhere safe, and copy it back afterwards to get your farm back. F9 in the game deletes both slots and starts a new game on day 1; the backups stay.
- **Logs:** Window > Output Log. Our messages are under `LogVestige`; note any warning or error even if the game looks fine.
- **Console:** the key under Esc opens it, for `stat fps`, `stat unit` and similar commands.

## 2. Pending checks

Checks for recent changes that haven't been confirmed in the editor yet. Each names the commit to look at if something is wrong. When a check passes, delete its line (or tell the agent, who deletes it). When one fails, write what you saw under it; it is fixed before new work starts.

### Code clean-up, 2026-10-29

- [x] **Sleep and passing out** (758f81c, 9d77ee1): N starts the next day. Letting the clock reach 02:00 passes the farmer out into the next day.
- [ ] **Messages** (c87081e): after sleeping, "You slept. Day N begins." and "Game saved." show as two lines. Up to three messages show at once, each fading after about three seconds.
   - Problem: If you Save, the save message appears. Then you sleep, and the sleep message appears. The save message remains. You sleep again, the save message appears and the save message is still there. Separate messages do not seem to fade out. It looks more like a log of messages that appears whenever a new message is sent.
- [ ] **Key hints** (6805d86, 812ea57): the bottom line reads "1 Hoe   2 Seeds   3 Watering Can   4 Hand   |   E Use tool   |   N Sleep   |   F9 New game". The tool names come from the tool data assets.
   - This works, but we need a much better toolbar.
- [x] **Clock** (812ea57): the status line shows the day, a 24-hour clock in 10-minute steps and the tool in hand.
- [x] **Counts** (812ea57): planting lowers the seed count by one, and harvesting a ripe crop raises the harvest count by the crop's yield. Both update at once.
- [x] **Old save** (9cdf0ac, 8fff295): your existing save loads with its day, inventory and planted crops. A save from before 2026-10-29 is copied to `Farm_Version0.sav` first. Each new day saves to whichever of `Farm.sav` and `Farm2.sav` doesn't hold the newest save.
- [ ] **Lighting** (42f3a44): at High and at Epic, days look as before and nights are properly dark. Epic may differ slightly, since only Epic uses hardware ray tracing now.
   - I don't know how to test different graphic settings.
- [x] **Content validation** (139e88d): right-click `Content/Farm` > Validate Assets reports no errors, and the Output Log at editor start has no "is used by more than one asset" error.

### Tile targeting, 2026-10-30

Replaces the tile highlight check above: the frame now sits on a neighbouring tile chosen by the movement direction (GameDesign.md, section 6).

- [ ] **Tap to turn** (TARGETING): a quick tap of W, A, S or D turns the farmer to face that way without stepping, and the frame moves to the tile next to the farmer's in that direction. Tapping two keys at once (W and D) turns diagonally, and the frame sits on the diagonal neighbour.
- [ ] **Walking** (TARGETING): holding a key walks after a short moment, and the frame follows the direction of travel, one tile ahead of the farmer. When you let go, the frame stays where it was.
- [ ] **Field edge** (TARGETING): standing on an edge tile and turning to face outwards hides the frame; turning back shows it again.
- [ ] **Tools** (TARGETING): the tools act on the framed tile, never on the one the farmer stands on.
- [ ] **Feel** (TARGETING): the delay before walking (0.15 s) doesn't feel sluggish. If it does, change Tap To Turn Seconds on `BP_ThirdPersonCharacter` (Details panel, Farming > Input) and note the value that feels right.
- [ ] **Gamepad** (TARGETING), if you have one: a flick of the left stick turns, holding it walks, the same as the keys.

### Source folders, 2026-10-30

- [x] **Solution** (adef4a7): after regenerating the project files, the solution shows `Core`, `Content`, `Time`, `Farming`, `Save`, `Player`, `UI`, `Template` and `Tests` under `Source/Vestige`.
- [x] **Editor start** (adef4a7): the editor opens `L_Farm` with no errors about missing classes, and the farmer Blueprint (`Content/ThirdPerson/Blueprints/BP_ThirdPersonCharacter`), `BP_FarmTile`, the crops and the tools open normally.

### Editor chores

Changes only the editor can make safely. The steps are in [Roadmap.md, Housekeeping](Roadmap.md#3-housekeeping): re-saving the template Blueprints and `L_Farm`, moving `BP_FarmTile` and the materials with redirectors fixed, and regenerating the Android File Server token if it was ever used. After each one, run the [smoke test](#3-smoke-test).

## 3. Smoke test

About five minutes in `L_Farm`, in Play in Editor at High. Run it before pushing and at the end of each milestone, and after any editor chore. Back up `Saved/SaveGames/` first.

1. **Start:** a new game (F9) starts on day 1 at 06:00. The HUD shows the day, the clock, the tool, seeds and harvested crops, and the key hints; the Output Log has no `LogVestige` errors.
2. **Tools:** with each tool on the tile in front of you: the hoe (1) tills grass, the seeds (2) plant in tilled soil and use a seed, the watering can (3) darkens the soil, and the hand (4) does nothing on an unripe crop. The tool in hand changes with each key. Each action shows a short message; facing away from the field says "Stand facing a farm tile."
3. **Growing:** water a planted tile and sleep (N), a few times. The crop grows one stage per watered night, and a tile left dry doesn't grow. Harvest the ripe crop with the hand; the harvest count goes up.
4. **Day and night:** let the clock run through an evening (it moves about one hour every 40 seconds unless the level's day cycle is set faster). The sun sets with warm light, the night is properly dark with faint moonlight, and at 02:00 the farmer passes out into the next day.
5. **Saving:** sleep ("Game saved."), stop the game and play again. The day, seeds, harvest count, tool in hand and every tile, crops included, come back.
6. **Highlight:** walk along the field edge and turn. The frame follows the tile in front of you and disappears off the field.
7. **Performance:** in the console, `stat fps` and `stat unit`. In a busy part of the farm, by day and by night, the frame stays under 16.6 ms (60 fps), and sleeping shows no visible hitch ([Architecture.md, section 10](Architecture.md#10-performance-by-design)). Play in Editor adds its own cost, so check a doubtful number in Standalone Game. For a hitch on the new day, start the game with `-trace=default` and open the trace in Unreal Insights (`Engine\Binaries\Win64\UnrealInsights.exe`): `AFarmDayCycle::StartNextDay` shows the whole new day, with the tiles, the save and the load inside it.

Put the save folder back afterwards if you want your farm as it was.

## 4. What isn't manual

- **Game rules are tested automatically**, by the `Vestige` automation tests: tile states and growth, tools and seeds, the clock, sleeping and passing out, the save rules and upgrades, content IDs, the include direction between source folders and the rendering settings. A rule is never covered by a manual check alone. If a check here ever finds a rule broken, the fix comes with a test that would have caught it.
- **This document is for what tests can't judge:** how the game looks and feels, lighting, the HUD as you see it, input, saves on real files across sessions, performance, and steps that need the editor.
- **When a change needs a manual check**, the commit that makes it adds a line to [Pending checks](#2-pending-checks) under a dated heading, with the commit and what to look for. A system that gains lasting behavior also gets a line in the [smoke test](#3-smoke-test).
