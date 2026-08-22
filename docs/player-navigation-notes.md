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

Parts workshops (engine / aero / suspension shops, etc.) are on the **Parts Shop course**, usually reached via **Select Destination → PARTS SHOP** / **Back to PA Menu → Go to Shops**, not by finding an OVERHAUL-style door on the Car Shop loop.

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
