# Integration edits (Battle Server)

After copying `TimeAttackRewards.h` / `.cpp` into `SBOL Battle Server/`, apply these hooks.

## 1. `Client Packets/ClientPacketCourseDetails.cpp`

Near the top:

```cpp
#include "..\TimeAttackRewards.h"
```

Inside `case 0x301:` after a successful `joinCourse()` (when `currentCourse` is known):

```cpp
TimeAttack_OnEnter(client, client->currentCourse);
```

Inside `case 0x302:` **before** `client->currentCourse` is overwritten, capture and pay:

```cpp
uint32_t leavingCourse = client->currentCourse;
// ... existing parse / removeClient / stash ...
// After you know the transfer is valid, and before/after stash:
TimeAttack_TryGrantBaseFare(client, leavingCourse);
```

Suggested placement: after `saveClientData` for the transfer, using the **old** course id (`leavingCourse`), not the destination.

## 2. `SBOL Battle Server.vcxproj`

Add:

```xml
<ClCompile Include="TimeAttackRewards.cpp" />
```

and

```xml
<ClInclude Include="TimeAttackRewards.h" />
```

(next to the other Client Packet compile items is fine).

## 3. Finish packet (later)

When you find the TA finish opcode, call:

```cpp
TimeAttack_GrantFinishReward(client, client->currentCourse, timeMs);
```

Then either remove the `0x0302` base-fare call or gate base fare so finish + leave do not double-pay (e.g. only pay on leave if `!s.entered` was cleared by finish).

## Rebuild

Rebuild **Battle Server only** → restart Battle → run Time Attack A or B → EXIT COURSE / transfer out → expect announce `TIME ATTACK: +1500 CP / +200 XP` and a log line.
