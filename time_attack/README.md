# Time Attack rewards (drop-in)

Self-contained CP/XP reward module for **Shutokou Battle Online** private Battle Server.  
Download **this folder only** from branch `cursor/time-attack-rewards-365a`.

## Why a base fare first?

If you have **no recorded times yet**, time-tier bonuses (gold/silver/bronze vs a ghost) have nothing to score against. A **base completion fare** still makes TA worth running:

- Flat **CP + XP** when you finish / leave a TA course after entering it
- Optional later: bonus tiers once finish-time packets and personal bests exist
- Short **cooldown** so exit-farming does not print money

Yes — start with base fare. Add thresholds after you have times.

## Where it hooks in the server

All of this is **Battle Server** (`server_client/SBOL Battle Server/`), not the DB process.

| Piece | File / API |
|-------|------------|
| Course enums | `client.h` → `COURSE_TIMEATTACKA` / `COURSE_TIMEATTACKB` |
| Grant currency / XP | `Client::giveCP()`, `Client::addExp()` |
| Persist | `Server::saveClientData(client)` |
| **Base fare (now)** | `ClientPacketCourseDetails.cpp` case **`0x0302`** — when leaving a TA course |
| Finish-time packet | Still unmapped (`0x0800` / `0x0D00` / `0x1300` / `0x1400` are `DoNothing` in `packets.h`) |
| Pattern to copy | `Client::processBattleWin()` / `SendBattleNPCFinish()` |

Client already can show strings like `* Rank settled at %d (Official) *`; the private server just never grants rewards on that path yet.

## Default base fare (editable)

In `TimeAttackRewards.h`:

| Constant | Default | Meaning |
|----------|---------|---------|
| `TA_BASE_CP` | `1500` | CP per completed TA run |
| `TA_BASE_XP` | `200` | XP per completed TA run |
| `TA_COOLDOWN_SEC` | `120` | Min seconds between payouts (per license) |

Tune freely for your economy.

## Install (copy into Battle Server)

1. Copy `TimeAttackRewards.h` and `TimeAttackRewards.cpp` into  
   `server_client/SBOL Battle Server/`
2. Add `TimeAttackRewards.cpp` to the VS project (`SBOL Battle Server.vcxproj`) if it does not pick up new `.cpp` files automatically.
3. Apply the three integration edits in `INTEGRATION.md`.
4. Rebuild **Battle Server only**, restart Battle.

## Later: time-tier bonuses

Once you capture the real finish packet (log unknown subtypes while clearing TA A/B):

1. Parse finish time (ms) from the packet.
2. Call `TimeAttack_GrantFinishReward(client, courseId, timeMs)` (stub ready in the `.cpp`).
3. Compare against a threshold table (or the player’s PB) and add bonus CP/XP on top of the base fare — or replace the leave-course payout with finish-only payout to avoid double-pay.
