# Bitboard Chess Engine

---

This is a chess engine, can be played on a GUI that support UCI protocol.

This is my attempt to learn how to build a bitboard chess engine from [Code Monkey King's tutorial.](https://www.youtube.com/watch?v=QUNP-UjujBM&list=PLmN0neTso3Jxh8ZIylk74JpwfiWNI76Cs)

It is playable on a chess GUI like ChessArena or CuteChess, but this engine only support basic protocol like parse position and time management.

## Features

- Bitboard board representation.
- Pre-calculated attack mask.
- Magic bitboard for sliding piece.
- Make/Unmake move
- Negamax search with alpha-beta pruning.
- Killer/History move ordering.
- Transposition table.
- [PeSTO's evaluation](https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function). 
- Pawns/King's safety/Mobility evaluation.
- Basic UCI protocol.

## How to play

> **Note:** This engine has currently only been tested and verified on **Window 11**. Support for Linus and macOS is included.

### Using a chess GUI (Recommended)

1. Download the **chess_engine.exe** from the [release page.](https://github.com/NhatTien-1635/Bitboard-ChessEngine/releases/tag/v1.0.0)
2. Install it into your chess GUI like ChessArena or CuteChess.
3. Have fun! :D

### Playing on Lichess

I have configured a bridge to run this engine on a bot account.

- **Challenge the bot:** [PotatoBotNo13](https://lichess.org/@/PotatoBotNo13)
- **Technical note:** This is handled via the [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot) Python bridge. It will search the move on my machine and send it to lichess, so this option is available sometime.

## Further Improvement

There is a lot of improvement can be made for the engine:

- **Full UCI support:** Some protocol I didn't support like: pounder and option.
- **Better evaluation:** Further tweak on evaluation can be made. Embedding Stockfish's NNUE is an alternative option.
- **Opening book:** The engine play really recklessly at the beginning.
- **Parallelism:** Implement Lazy SMP for multi-core scaling.
- **Advanced Search:** Implement Multi-Cut and Singular Extensions.
- **Endgame:** Integrate Syzygy Tablebase probing.
