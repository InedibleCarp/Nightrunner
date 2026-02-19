# Nightrunner

Nightrunner is a C++ endless runner game built with the [Raylib](https://www.raylib.com/) library. Control the runner as he dashes through a neon cityscape, dodging hazardous nebulae. The game features parallax scrolling, progressive difficulty, and a persistent local leaderboard.

## 🎮 Gameplay

The goal is to survive as long as possible. Your score increases based on the distance traveled. As your score rises, the game becomes more difficult: obstacles move faster and the gaps between them shrink.

### Controls

| Key | Action |
| :--- | :--- |
| **SPACE** | **Jump** (Press again in mid-air to **Double Jump**) |
| **SPACE** | Start Game (from Menu) / Retry (from Game Over) |
| **L** | View Leaderboard |
| **M** | Return to Menu |

## ✨ Features

* **Parallax Backgrounds:** Three-layer scrolling background system (Foreground, Midground, Far Buildings) for visual depth.
* **Progressive Difficulty:** Obstacle speed and spawn rates scale logarithmically with your score.
* **Animation:** Sprite sheet animation for the player character and obstacles.
* **Leaderboard:** Saves the top 5 high scores locally to `scores.dat`.
* **Audio System:** Dynamic music switching between menu and gameplay states.

## 🛠️ Building the Game

### Prerequisites

* **C++ Compiler:** `clang++` (default) or `g++`.
* **Raylib:** This project requires the Raylib library.

**Installing Raylib on macOS (Homebrew):**
```bash
brew install raylib
```