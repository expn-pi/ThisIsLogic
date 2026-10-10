# This is Logic

A puzzle game about boolean logic, made with Unreal Engine 5 and C++.

The player gets a sentence in natural language, written as a "complex" boolean formula, and must rewrite it into a simpler equivalent one, in the style of the exercises of an introductory logic course.

> **Status:** early development. The game is not playable yet.

## How it works

Each symbol of the formula is a block in a row, and the player moves, adds and removes blocks freely. Two indicators help along the way:

- **Validity:** is the sequence of blocks a well-formed formula?
- **Equivalence:** is it still equivalent to the level's starting formula? Cyan if it is, red if it is not, gray while the formula is invalid.

Proposition blocks hold a sentence and a letter, and can be minimized (letter) or maximized (whole sentence). Operator blocks cover the constants ⊤ and ⊥, ¬, ∧, ∨, ⊕, NAND (↑), NOR (↓), →, ←, ↔ and the parentheses.

### Example

| | |
|---|---|
| **Sentence** | It is not true that John eats neither tofu nor rice. |
| **Starting formula** | `¬(¬[John eats tofu] ∧ ¬[John eats rice])` |
| **Solution** | `[John eats tofu] ∨ [John eats rice]` |

## Features

- [x] Drag and drop blocks with the mouse
- [x] Blocks laid out as a row, reordered by dragging
- [x] Block width fitted to its text
- [x] Proposition and operator blocks
- [ ] Levels loaded from a data table
- [ ] Validity and equivalence indicators
- [ ] Logic core: parser, evaluation and equivalence by truth table
- [ ] Win condition and progression between levels
- [ ] Menu, level select and saved progress

## Built with

- Unreal Engine 5.8
- C++ for all game logic, with no visual scripting; Blueprints only where the editor does the job better than code, such as the UI layout
- Enhanced Input and Procedural Mesh Component

## Building

Requirements: Windows, Unreal Engine 5.8 and Visual Studio 2022 with the "Game development with C++" workload.

1. Clone the repository.
2. Right-click `ThisIsLogic.uproject` and choose **Generate Visual Studio project files**.
3. Open `ThisIsLogic.sln` in Visual Studio.
4. Select the **Development Editor** configuration and the **Win64** platform.
5. Press **Ctrl+F5** to build and open the editor.

## Project structure

```
Source/ThisIsLogic/
  BasicComponents/   camera, game mode and player controller
  Gameplay/          blocks and formulas
  Input/             mouse routing interface
  Logic/             logic core, plain C++ with no engine dependencies
Content/             assets, in folders by feature
```

## License

This project is licensed under the MIT License; see [LICENSE](LICENSE) for details.

Unreal Engine is a trademark of Epic Games, Inc., and its use is governed by the [Unreal Engine EULA](https://www.unrealengine.com/eula). This repository contains no engine code.

## Author

Elias Ximenes do Prado Neto (EXPN)