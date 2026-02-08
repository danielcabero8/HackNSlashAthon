# HackNSlashAthon

🎮 HackNSlashAthon — AI-Powered Procedural Dungeon Crawler
Supercell Hackathon Feb 2026 - Hack N Slack game using GenAI made by @salvagm &amp; @danielcabero8

  🎯 Project Goal

  HackNSlashAthon is a top-down hack-and-slash roguelike built in Unreal Engine 5.5 (C++) where every level is generated in real-time by Anthropic's Claude AI. The core idea: an infinite dungeon       
  crawler where the AI acts as a live game designer, crafting each floor based on how the player performed on the last one. You start with 3 lives. There is no final level. You play until you fall.    

  ---
  🧠 Generative AI — The Core of the Experience

  GenAI isn't a side feature — it is the level designer. Every dungeon floor is authored by Claude through a structured prompt conversation sent via HTTP at runtime.

  📡 Integration

  The game communicates with Claude Haiku 4.5 (switchable to Sonnet 4.5) via a direct HTTP POST to the Anthropic Messages API. The API key is loaded from a local file at startup. Requests are fully    
  asynchronous — the game fires the request and continues rendering while awaiting a callback, preventing any frame drops during generation.

  📜 The System Prompt — Teaching Claude to Design Levels

  A detailed system prompt is embedded in the source code at initialization. It serves as Claude's permanent instruction manual and covers four key areas:

  1. Role & Context — Claude is told it's a tile-based level generator for a top-down hack-and-slash game. It understands the MxN grid layout and that its job is to decide what spawns on each tile.    
  2. Entity Vocabulary — Claude learns every placeable entity, organized by category:
    - 🕳️ General: Hole (deadly fall), Floor (safe), PlayerStart (spawn point)
    - 👾 Enemies: EnemyType1 — with configurable MovementSpeed, AttackCadence, and BulletSpeed
    - 🔥 Traps: Spikes (1-hit damage on contact)
    - 🪨 Environment: Column (blocks movement and projectiles)
  3. Rules & Constraints — Exactly one PlayerStart per level. Tile indices are 0-based row-major. Floor is the default for any unspecified tile. Difficulty must increase progressively, not be random.  
  4. Output Format — Claude must return sparse JSON only: a single "M" object mapping tile indices to entity data. Non-enemy tiles are a simple integer ("7": 3), while enemies are a tuple ("3": [11,   
  300.0, 1.5, 800.0]). Omitted indices default to Floor. This sparse format is a deliberate optimization — it minimizes token usage, reducing latency and API cost without losing any information.       

  💬 The User Prompt — The Adaptive Feedback Loop

  Each time a level is completed, the game constructs a JSON user prompt containing five performance metrics:

  {"CompletedLevel": 3, "Time": 45.2, "HitsTaken": 1, "Rows": 10, "Columns": 10}

  - ⏱️ Time — how long the player took (seconds)
  -  HitsTaken — damage absorbed (0 = flawless, 2 = one hit from death)
  - 📊 CompletedLevel — current progression depth
  - 📐 Rows/Columns — grid dimensions for the next floor

  Claude uses these signals to calibrate difficulty dynamically. A player who cleared the last floor quickly and unscathed will face more enemies, tighter corridors, and more holes. A player who barely
   survived gets a more forgiving layout. The system prompt provides a balancing baseline (level 1 = 2 enemies, 1 obstacle, 5 holes) that Claude scales upward from there.

  🔄 Response Processing Pipeline

  When Claude's response arrives, the game:
  1. Extracts the text content from the API response
  2. Strips any markdown code fences ( json ...  ) Claude may add
  3. Parses the sparse "M" JSON object
  4. Initializes a full grid defaulting every tile to Floor
  5. Overlays the sparse entries — simple integers for traps/environment, 4-element arrays for enemies with their individual combat stats
  6. Broadcasts an OnLevelGenerated event, triggering the spawn system

  🛡️ Resilience

  If the API is unavailable, the game falls back to a local debug generator with hardcoded enemy and trap placements, ensuring the game never breaks due to network issues.

  ---
  🏗️ Game Systems at a Glance
  ┌───────────────────┬───────────────────────────────────────────────────────────────────┐
  │      System       │                               Role                                │
  ├───────────────────┼───────────────────────────────────────────────────────────────────┤
  │ 🔄 Game Loop      │ FSM managing level lifecycle, tracking lives/kills/hits/time      │
  ├───────────────────┼───────────────────────────────────────────────────────────────────┤
  │ 🏭 Spawn System   │ Data-driven pipeline mapping entity IDs → Unreal actors           │
  ├───────────────────┼───────────────────────────────────────────────────────────────────┤
  │ 🤖 Enemy AI       │ Behavior Trees                                                   │
  ├───────────────────┼───────────────────────────────────────────────────────────────────┤
  │ 🎮 Player Control │ WASD + spacebar to jump                                           │
  ├───────────────────┼───────────────────────────────────────────────────────────────────┤
  │ 🎬 Demo Mode      │ Batch-generates and showcases multiple AI-created levels          │
  ─────────────────────────────────────────────────────────────────────────────────────────
