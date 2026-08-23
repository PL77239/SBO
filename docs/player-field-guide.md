# SBOL player field guide

Consolidated notes from live play on the private server (client ~v2.03, handle examples like `janko_admin`).  
Use this as the main operator / player reference. Maps live in this folder.

---

## 1. Controls & in-course HUD

### Bottom tabs (while driving)

| Tab | Role |
|-----|------|
| **MAP** | Road Map Normal/Wide, Course Guide, **EXIT COURSE** (leave to Locations / main menu) |
| **NAVI** | Mini-map options |
| **ASSIST** | Shift Assist on/off |
| **ITEM** | Inventory (valet tickets, Omamori, etc.) |
| **DATA** | Personal / team data |
| **HOME** | Temporary garage on the **current** course (`DRIVE` / settings; Back returns to course) |

There is **no Esc options menu** while driving. `GAME_QUIT` (even if bound to Escape) is **quit game**, not settings and not “Go to PA”.

| Action | Default |
|--------|---------|
| OK / confirm | **Enter** |
| Cancel / Back | **Backspace** (`KB_BACK`), not Escape |
| Challenge NPC / player | **PASSING** (default **`C`**) — headlights alone do nothing |

### Course Guide (bottom center)

Junction overlays show **real destination splits**, not flavor text only.

Example on **Main** at **Tatsumi JCT**:

- Left: **TIME ATTACK A** / 湾岸線 (Wangan)
- Right: 湾岸線 (Wangan)

Follow the labeled branch to transfer.  
`B` / MAP → Road Map toggles **[NORMAL MAP]** vs **[WIDE MAP]**.

### Debug / status strip (left)

When visible:

| Field | Meaning |
|-------|---------|
| **POSITION** | `junction:distance:extra` as hex (e.g. `075A:01C7:07F3`) |
| **COURSE NAME** | `Main`, `Car Shop`, `Parts`, … |
| **CURRENT TRACK** | BGM title (Vorbis `TITLE`, e.g. `Metropolis As`) |
| **LV** / **CP** | Level / currency |
| **PERSONS IN COURSE** / **NETWORK** | Lobby occupancy / net |

Junction ID in `POSITION` ≈ node index in `game_client/data/COURSE/SHUTO.DPP`.

---

## 2. Courses (IDs)

| ID | Name | Notes |
|----|------|--------|
| `0` | **Main** | Default after beginner; rivals spawn here |
| `1` | **Car Shop** | Buy/sell cars, OVERHAUL, tickets |
| `2` | **Parts** | Aero / drive train / body workshops |
| `3`–`4` | Freeway A/B | |
| `5` | Survival | |
| `6`–`7` | **Time Attack A/B** | e.g. Tatsumi → TIME ATTACK A |

Server names: `coursenames[]` in Battle Server (`Main`, `Car Shop`, `Parts`, …).

---

## 3. PA hub & shop course access

Car Shop / Parts are **separate courses**, **not** Main C1 off-ramps (unlike Tatsumi → Time Attack).

### Enter

| Destination | How |
|-------------|-----|
| **Car Shop** | Locations → Go to PA → **Select Destination → CAR SHOP**, or valet **MOVE CAR SHOP COURSE** (item `2110`) |
| **Parts Shop** | Same → **PARTS SHOP**, or valet **MOVE PARTS SHOP COURSE** (`2111`) |

PA labels also include: **MERCHANDISE SHOP**, **TUNED CAR EXCHANGE**, **Return to PA**, **Go to Shops**.

### Leave

| Action | How |
|--------|-----|
| Leave shop course | **MAP → EXIT COURSE** → Main Menu / Locations (**menu**, not a roadside ramp) |
| Valet back to Main | **MOVE MAIN COURSE** → lands at **Miyakezaka JCT** (三宅坂) |

![Shop course enter/leave](map_txr_shop_course_exits.png)

Period reference: [4Gamer 2004-07-07](https://www.4gamer.net/games/007/G000798/20040707211303/) — valet tickets to Main (三宅坂), Car Shop, Parts Shop.

---

## 4. Car Shop course

Roadside / Course Guide services on **Car Shop** (not Parts):

| Marker / UI | What it does |
|-------------|--------------|
| **OVERHAUL** | Engine refresh / overhaul (CP). Found during live play on the Car Shop loop |
| Dealer buildings | Buy / sell cars, tickets |

Do **not** expect Parts workshops (BODY / DRIVE TRAIN / …) on this course — those are on **Parts**.

Chat helpers used in testing: `!give item 2110` / `2111` for valet tickets (ITEM BOX → use). Safer than raw SQL `item_data` edits.

---

## 5. Parts Shop course (live pins)

Workshops are **separate Course Guide ovals**. Drive into the label to open that shop.

| Shop | Junction | Landmark / live POSITION |
|------|----------|---------------------------|
| Course entrance (green wrench / “power-up”) | `0x06BE` | Near 浜崎橋; badge **PARTS SHOP COURSE**; `06BE:…:0203` |
| **BODY** | `0x0848` | **Ichinohashi JCT** (一ノ橋); `0848:012A:0099`; left → Meguro / 目黒線, right → C1 Outer |
| **DRIVE TRAIN** | `0x075A` | **Tanimachi JCT** (谷町); `075A:01C7:07F3`; left → Shibuya / 渋谷線, right → C1 Outer |
| **EXTERIOR** | `0x086D` | **Edobashi JCT** (江戸橋); `086D:01D4:08F3`; left → Mukojima / 向島線, right → C1 Outer |

### DRIVE TRAIN submenu

Opens **PARTS SHOP DRIVE TRAIN**:

| Menu | Meaning |
|------|---------|
| **TRANS MISSION** | Transmission |
| **DIF** | Differential |
| **SUS** | Suspension |
| **TIRE & BRAKE** | Tires & Brakes |

### Exterior / unlocks

Stock cars often only show base exterior slots until unlocked. Chat **`!unlock all`** unlocks higher aero/engine slots for the active car (used successfully for RX-7 front bumper Types 1–5). Aero is largely per-car mesh; prices by class A/B/C; wheels use a separate path.

### HOME / MAIN GARAGE (on Parts)

Temporary course garage — **DRIVE**, or **SETTING 1/2/3** (e.g. spring rate, damper, turbo boost). This is **setup**, not a roadside Parts marker.

![TXR-style Parts pins](map_txr_parts_shops.png)

![Wider Shutoko + mesh](map_preview_parts_shops.png)

---

## 6. Safe Mode & Team Center

- Blue **SAFE MODE** tag above the car → cannot be **PvP**-challenged; exploring shops is fine.
- Omamori / beginner rules can enforce Safe Mode until course-out.
- **TEAM CENTER** / **TUNED CAR EXCHANGE** may show *“access currently blocked”* when server feature byte `0x0182` is `0`.
- Team Center junction (commented spawn): `0x00DC`.

---

## 7. Rivals (NPC bots)

| Topic | Detail |
|-------|--------|
| Where they spawn | **Main** only, and **not** Beginner (`notBeginner` must be true) |
| Look | Ghost visuals until challenged (no solid collision) |
| Challenge | **PASSING** (`C`) → client `0x0504` NPC / `0x0500` PvP |
| Safe Mode | Blocks **being** PvP-challenged; does **not** block you challenging NPCs |
| Spawn count | `Client::getRivals()` — loop `for (i < 3)` with DB rival IDs `800`, `1`, `2` (Keisuke + Rolling Guy #2/#3). Course entity IDs sent to client are `0`, `1`, `2` |
| Data | `server_client/SBOL Battle Server/data/rivals/*.json` (~241 members). Cap: `COURSE_NPC_LIMIT` = 100 |
| Increase bots | Edit that loop in **your tree**, rebuild **Battle Server only**, restart Battle. Do **not** delete `lib\Debug` / `lib\win32-debug` |
| Disable | Uncomment `DISABLE FOR LIVE` returns, or `#define DISABLE_BATTLE` |

### If PASSING (`C`) does nothing

1. Confirm **COURSE NAME = Main** and you are past Beginner (rivals never spawn on shop / TA / beginner).
2. Confirm ghosts appear (join packet `0x0480`). No ghosts → empty `rivals` list → challenge always aborts.
3. Challenge uses the **course entity ID** (`0`–`2`), not DB id `800`. Server looks up `getRival(id)` from the local spawn list.
4. Shop / PA mode disables battle packets (`packetDisable(0x05)`). Must be on course with `enableCoursePackets()`.
5. Battle Server log lines to watch: `challenging NPC ID` / `challenging invalid NPC` / rival abort `0x0585`.
6. Known server bugs that break challenges: spawn with position still `(0,0,0)` until first `Tick()`; `clearBattle()` / failed challenge leaving `battle.isNPC` stuck — fixed on branch `cursor/rival-npc-challenge-365a`.

Rival wins already call `giveCP()` / `addExp()` (NPC `WinXP()` is still stubbed to `0`). Time Attack rewards: see §10 and the `time_attack/` folder branch.

---

## 8. Custom BGM

Files: `game_client/data/BGM/*.ogg` (Ogg Vorbis, stereo ~44.1 kHz).

| Slot | Examples |
|------|----------|
| Free run | `freerun_0.ogg` … `freerun_8.ogg` — HUD **CURRENT TRACK** = Vorbis `TITLE` (`Metropolis As` = `_0`) |
| Battle | `battle_0.ogg`, `battlewin` / `battlelose` / `battleend` |
| Menus | `mainmenu.ogg`, `garage.ogg`, `parking.ogg` |

Client matches patterns like `freerun*.ogg`. **Replace a file** (same name) → restart client. No server rebuild.  
Engine SE is separate (`SE/*.dls`) — see §8b.

---

## 8b. Engine SE swap / normalize (`AllEngine.dls`)

Do **not** drop mixed WAV banks straight into the game. Stock engines live in `game_client/data/SE/AllEngine.dls` (DirectMusic DLS):

| Fact | Stock bank |
|------|------------|
| Instruments / waves | ~46 / ~60 |
| Format | Mono **Microsoft ADPCM**, **22.05 kHz**, 4-bit |
| Loops | Short (~1–2 s), with loop metadata |
| RPM layers | Several **regions** per engine; **unity notes** ~MIDI 55–70 |
| Loudness | Per-region **attenuation** in the DLS, not only WAV peaks |

**Pipeline**

1. Backup `AllEngine.dls`. Open in a DLS editor (e.g. Awave Studio). Export each wave and note instrument, region, unity note, attenuation, loop length.
2. Source **steady RPM loops** (mono). Avoid stereo cinematic takes / gear-shift edits.
3. In Audacity/Reaper: HPF ~40–80 Hz → light EQ to a house curve → **match RMS (or short-loop LUFS) to the stock layer you replace** (not peak-normalize alone) → soft limit ≈ −1 dBTP → resample **22050 Hz mono**.
4. Set loop points on zero-crossings; keep lengths in the same ballpark as stock.
5. Re-import as Microsoft ADPCM; keep stock **region map / unity notes / attenuation** unless you are retuning the whole car.
6. Replace `AllEngine.dLS`, restart client. `RaceSE.dls` / `SystemSE.dls` are separate.

Peak-only “normalize” across packs still sounds uneven because RMS/timbre differ. Same rate/channels/codec + same RMS target + same region layout = usable swaps.

---

## 9. Resolution (dgVoodoo)

Client is **D3D8** (not Glide). Soft image after “1920×1080” is usually the **wrong tab**.

| Do | Don’t |
|----|--------|
| **dgVoodoo → DirectX** tab → Forced resolution (e.g. 1920×1080), optional MSAA / AF | **Glide** tab resolution (ignored by this game) |
| Keep `D3D8.dll` + `dgVoodoo.conf` beside the **actual** `SBClient.exe` you launch | Assume Glide settings apply |
| Apply → fully quit game → relaunch | Expect native widescreen HUD without EXE work |

dgVoodoo **upscales output**; it does not make a true native 1080p remaster.  
NVIDIA **NIS/DLSS GitHub SDKs** need DX11+/Vulkan integration — not a drop-in for D3D8. Driver **Image Scaling** or dgVoodoo is the practical path.

---

## 10. Time Attack rewards

**Where to code:** Battle Server only — not the DB service.

| Piece | Location |
|-------|----------|
| Course IDs | `Client::COURSE_TIMEATTACKA` / `B` (`client.h`) |
| Grant APIs | `Client::giveCP()`, `Client::addExp()`, then `Server::saveClientData()` |
| Leave / transfer | `ClientPacketCourseDetails` case `0x0302` (good hook for **base completion fare** until the official finish packet is mapped) |
| Finish time packet | Still unmapped (`0x0800` / `0x0D00` / `0x1300` / `0x1400` are currently `DoNothing`) — log unknown types while driving TA to find it |
| Rival-style pattern | Mirror `processBattleWin()` / `SendBattleNPCFinish()` |

**Base fare without personal records:** yes — that is the right first step. Pay a flat CP + XP for completing a TA run (leave course after entering A/B, or later: on verified finish). Add time-tier bonuses later once you have times and the finish packet. Keep a short cooldown so exit-farming does not print money.

Self-contained design + drop-in sources: repo folder **`time_attack/`** on branch `cursor/time-attack-rewards-365a` (download that one folder from the PR).

---

## 11. Useful chat / ops (testing)

| Command / note | Use |
|----------------|------|
| `!give item 2110` / `2111` | Car / Parts valet tickets |
| `!unlock all` | Unlock higher parts slots on active car |
| `SERVER.INI` | Both slots `TYPE=0` / `SIDE=0` for typical local setup |
| Course transfer | Client `0x0302` then reconnect; Battle Server stashes destination so shop/Main resume correctly |

---

## 12. Map / mesh (dev)

- C1 HUD tiles (`TEX/mm_new.MIA` → `mm_map*`) are **not** the full world.
- Full mesh: `SHUTO.DPP` (~2276 nodes, ~9250 links); junction ID ≈ node index.
- EXIT meshes under `COURSE/EXIT/` (Yaesu, Edobashi, Ariake, Shiba, Hamasaki, …).

Other server junctions of note: Main start `0x0024`, Outer C1 start `0x01CD` (commented).

---

## 13. Period Online vs 2025 remake

Search **首都高バトルOnline** (Genki PC, ~2003–2005).  
Do **not** use Steam/PS **首都高バトル (2025)** “PA unlock by driving in” guides as Online menu rules.

Useful period notes (4Gamer): valet tickets; later **ショップカー** instead of fixed goods shop at 竹橋; B-ZONE / Wangan layout.  
Remake PA names (平和島, 辰巳, 代々木, 芝浦, 箱崎) are geography only.

---

## Screenshot → doc map

| Live capture theme | Documented in |
|--------------------|---------------|
| Parts wrench / “power-up”, BODY, DRIVE TRAIN, EXTERIOR + POSITIONs | §5 + maps |
| MAIN GARAGE / SETTING 3 / DRIVE TRAIN UI | §5 |
| Safe Mode, blocked Team Center / Tuned Car Exchange | §6 |
| Shop enter via PA / EXIT COURSE / Miyakezaka return | §3 + `map_txr_shop_course_exits.png` |
| dgVoodoo Glide vs DirectX confusion | §9 |
| Rival ghosts / Passing / bot count | §7 |
| CURRENT TRACK / Metropolis As | §1, §8 |
| Engine SE / DLS normalize | §8b |
| Time Attack CP/XP | §10 + `time_attack/` branch |
