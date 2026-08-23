# SBOL player navigation notes (v2.03 / private server)

Field notes from live Main / Car Shop / Parts exploration. Useful for operators and future docs.

## In-course HUD strip

While driving, the bottom menu is:

| Tab | Role |
|-----|------|
| **MAP** | Road Map Normal/Wide, Course Guide, EXIT COURSE (leave to Locations / main menu) |
| **NAVI** | Mini-map on/off style options |
| **ASSIST** | Shift Assist on/off |
| **ITEM** | Inventory (valet tickets, Omamori, etc.) |
| **DATA** | Personal / team data views |
| **HOME** | Temporary garage still tied to current course (DRIVE often greyed; Back = return to course) |

There is **no separate Esc options menu** while driving. `GAME_QUIT` (even if bound to Escape) is **quit game** (“Exit game?”), not settings and not “Go to PA”.

Menu **OK** = Enter, **Cancel/Back** = **Backspace** (`KB_BACK`), not Escape.

## Course Guide (bottom center)

With Course Guide enabled, junction overlays show **real destination splits**, not just flavor text.

Example from Main:

- Junction: **Tatsumi JCT**
- Left: **TIME ATTACK A** / 湾岸線 (Wangan)
- Right: 湾岸線 (Wangan)

So course transfers (TA / Wangan / shops) can appear as **lane-choice signs** while driving. Follow the labeled branch and take the exit when the guide shows the destination you want.

`B` / MAP → Road Map only toggles **[NORMAL MAP]** vs **[WIDE MAP]** HUD zoom.

## Debug / status HUD (left)

Typical fields when the debug-style overlay is visible:

- **POSITION** — `junction:distance:extra` as hex, e.g. `0018:01EB:38C6`
- **COURSE NAME** — e.g. `Main`, Car Shop, Parts
- **PERSONS IN COURSE**, **NETWORK**, **CURRENT TRACK** (BGM)
- **LV** / **CP**

Server junction IDs of note:

- `0x0024` — Main start
- `0x01CD` — Outer C1 start (commented)
- `0x00DC` — Team Center (commented spawn; often blocked by `0x0182` feature byte `0`)
- `0x06BE` — **Parts Shop course** roadside shop marker (“power-up” / green wrench shield). Live HUD example: `POSITION: 06BE:…:0203` near 浜崎橋 on the mini-map; Course Guide shows **PARTS SHOP COURSE** with the wrench badge. Drive into this marker to open Parts workshops (same idea as OVERHAUL on Car Shop).
- `0x0848` — **BODY** tune shop on Parts course. Live HUD: `POSITION: 0848:012A:0099` at **Ichinohashi JCT** (一ノ橋); Course Guide shows a **BODY** oval. Split here: left → Meguro / 目黒線 (Route 2), right → C1 Outer.
- `0x075A` — **DRIVE TRAIN** shop on Parts course. Live HUD: `POSITION: 075A:01C7:07F3` at **Tanimachi JCT** (谷町); Course Guide shows **DRIVE TRAIN** on the left branch (Shibuya Route 3 / 渋谷線), right → C1 Outer.

## Courses vs roadside buildings

| Place | How you get there |
|-------|-------------------|
| **Main** | Default graduated course |
| **Car Shop course** | Locations → Go to PA → Car Shop, or valet **MOVE CAR SHOP COURSE**, or PA destination **CAR SHOP** |
| **Parts Shop course** | Same hub → **PARTS SHOP** / valet **MOVE PARTS SHOP COURSE** (separate course from Car Shop) |
| **Time Attack A/B** | Course Guide splits (e.g. Tatsumi JCT → TIME ATTACK A) or Locations / tickets |
| **Team Center** | World Map / Locations destination; often **blocked** by server flag |

On **Car Shop** course, roadside exits are Car Shop services, e.g.:

- **OVERHAUL** — engine refresh / overhaul UI (CP cost). Not the Parts course.
- Car dealer buildings — buy / sell / tickets.

Parts workshops (engine / aero / **BODY** / suspension shops, etc.) are **roadside markers on the Parts Shop course**, not one shared door. Field pins so far:

| Shop | Junction | Landmark / HUD |
|------|----------|----------------|
| Parts course entrance (wrench) | `0x06BE` | near 浜崎橋; **PARTS SHOP COURSE** badge |
| **BODY** tune | `0x0848` | **Ichinohashi JCT**; Course Guide **BODY** oval; `0848:012A:0099` |
| **DRIVE TRAIN** | `0x075A` | **Tanimachi JCT**; left → Shibuya線; `075A:01C7:07F3` |

Drive into the labeled marker to open that shop UI. You can also reach the course via **Select Destination → PARTS SHOP** / **Back to PA Menu → Go to Shops** (not via an OVERHAUL-style door on the Car Shop loop).

PA destination list labels include: **CAR SHOP**, **PARTS SHOP**, **MERCHANDISE SHOP**, **TUNED CAR EXCHANGE**, **Return to PA**, **Go to Shops**.

## Safe Mode

- Blue **SAFE MODE** tag above the car = cannot be challenged for PvP.
- Beginner / Omamori-related enforcement possible.
- NPC rivals are still ghost visuals on Main until challenged with **PASSING** (default `C`).
- Exploring shops in Safe Mode is fine.

## Rivalry quick ref

- Ghost NPCs = normal (no solid collision).
- Flash lights alone does nothing; **PASSING** (`C`) starts challenge (`0x0504` NPC / `0x0500` PvP).
- Disable spawn: uncomment `DISABLE FOR LIVE` returns in `getRivals()` / `SendRivalPosition()`, or `#define DISABLE_BATTLE`.

## Full map data (dev)

- C1 HUD tiles in `TEX/mm_new.MIA` are **not** the full course.
- Full mesh: `game_client/data/COURSE/SHUTO.DPP` (~2276 nodes).
- EXIT assets under `COURSE/EXIT/` (Yaesu, Edobashi, Ariake, Shiba, Hamasaki, Ueno, etc.).

## Web research (首都高バトルOnline vs 2025)

Search Japanese title **首都高バトルOnline** (Genki PC, service ~2003–2005; official was `sb-online.genki.co.jp`).  
Do **not** confuse with Steam/PS **首都高バトル (2025)** / Tokyo Xtreme Racer remake — most current JP “PA解放” guides are for that remake (drive into 辰巳PA / 平和島PA / 箱崎PA, etc.).

### Period Online sources (relevant to this client)

From [4Gamer 2004-07-07](https://www.4gamer.net/games/007/G000798/20040707211303/):

- **運転代行 (valet) tickets**
  - `MOVE MAIN COURSE` → **三宅坂JCT** on Main
  - `MOVE CAR SHOP` → Car Shop course
  - `MOVE PARTS SHOP` → Parts Shop course
- **お守り** → permanent Safe Mode until course-out
- **ライバルマップ** → show NPC names on map
- **CPカード** → trade CP via team garage, etc.

From [4Gamer 2004-10-06](https://www.4gamer.net/games/007/G000798/20041006174622/):

- Fixed **グッズショップ** at **C1外回り・竹橋JCT** was closed
- Replaced by **ショップカー** (mobile shops): pass them with **パッシング** to buy
  - A-ZONE ×2, B-ZONE ×4, 湾岸/横羽 up/down ×2 each

From [4Gamer 2003-08-28](https://www.4gamer.net/games/007/G000798/20030828202934/):

- **B-ZONE** added: 深川線〜湾岸線〜台場線〜環状 (Fukagawa / Wangan / Daiba / loop), longer/straighter than A-ZONE; Rainbow Bridge

From [4Gamer 2003-05-27](https://www.4gamer.net/games/007/G000798/20030527211442/):

- **C1外回り** added after **C1内回り**

### Live Course Guide match

Player screenshot on Main at **Tatsumi JCT** with splits to **TIME ATTACK A** / **湾岸線** fits the Online-era Wangan / B-ZONE layout (辰巳 is a real Wangan area). Follow Course Guide branches for course transfers when labeled.

### Remake PA names (geography only; unlock rules differ)

Useful as real Shutoko landmarks, not as Online menu rules: 平和島PA, 辰巳PA, 代々木PA, 芝浦PA, 箱崎PA (and direction-restricted approaches). Online shops were **separate courses** + later **shop cars**, not the 2025 “enter PA ramp to unlock map icon” system.
