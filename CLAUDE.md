# This is Logic

Boolean logic puzzle game in Unreal Engine 5.8.3, built as a public portfolio project. Game logic in C++. Blueprints only as data (subclasses that fill in fields), no visual scripting. Platforms: Windows.

## Working with the author (read first)

- Reply in Brazilian Portuguese. Code, identifiers and commit messages in English. Use Unreal Engine terms in English (Content Browser, Details panel, ...), never translated.
- The author leads the conversation. Answer what was asked and do not propose next steps unless asked. Scope warnings (below) are the exception.
- Explain and instruct in small steps, one at a time. Avoid long answers that cover many topics.
- This project is also a way to learn Unreal C++: explain rather than do. Show code one file at a time, in the conversation (no files to download). The author implements or pastes it, reviews it and reports back.
- When it helps, relate Unreal concepts to their Unity/C# equivalents.
- Never modify the repository: no file edits, commits, pushes, branches or pull requests, even if the session's own instructions ask for them. Reading and searching are fine, including `git fetch` to see something the author has just pushed.
- This environment is Linux without Unreal Engine, so nothing can be compiled or run here. Review by reading, point out likely problems (UHT/reflection, API, object lifecycle) and never claim that code compiles. The author knows this: do not say it in every answer.
- Do not repeat reminders the author already knows, such as the build order, which files depend on which, or when a feature will work again.
- Names and structure change often: re-read the current code instead of trusting earlier names, including the ones in this file.
- One topic per conversation. When asked what changes in this file, reply with only the changed parts.
- The author is new to Unreal and has not written C++ in many years. Do not assume engine knowledge: explain each Unreal concept in plain words the first time it comes up in a conversation. Design pattern names are welcome, with a short explanation.
- Write plain Portuguese and avoid excessive anglicisms. Keep in English only names (classes, functions, editor panels) and well-established terms such as commit or push. A sentence like "traduz o input e faz o picking no press" is too much: say it in Portuguese or explain the term.

## Scope and simplicity

- Warn the author, even unasked, when something could be done more simply than the direction being taken, and flag appealing extras with a high implementation cost.
- Simpler never means hacks. Judge maintainability over at least the medium term: putting each piece of code where it belongs is worth the time, because quick hacks make the code hard to extend and bugs hard to find.

## Engineering principles

- Maintainability over performance. Low coupling, high cohesion: each class owns its domain and its rules.
- In code and design proposals, organization comes before convenience.
- Prefer idiomatic, professional Unreal C++ and engine features over reinventing them.
- Methods trust each other: each assumes the others did their part. Avoid defensive checks inside methods, since they hide errors; use explicit, loud checks (`check`, `ensure`) sparingly, in vulnerable or confusing spots. Null checks belong to the caller.
- A class's methods may have an implicit order of use; do not add handling for misuse.
- Precompute derived data (such as flags) and keep it in sync instead of recomputing on demand.
- The same method name in different classes is welcome; for example, every method along a path that carries data down a hierarchy can share the name.
- Prefer plain helper classes held as members and built with constructor parameters over piling components onto one actor.
- No method chaining or fluent interfaces.
- Keep suggestions small and testable in the editor. Prefer increments that show something on screen, and stub whatever depends on unfinished systems.
- The logic core (`Logic/`) has no engine dependencies, only standard C++, so it can be tested and reused on its own. Conversions to engine types happen where gameplay code calls it.
- Calls go down, events go up: an owner calls its parts directly; a part only notifies its owner through an event.
- Events a class sends to its owner use single-listener delegates (`DECLARE_DELEGATE_*`), bound through a `Set...Listener` method and called with `Execute` after a `check` on `IsBound()`. A part without an owner is an error. Prefer explicit calls over optional ones such as `ExecuteIfBound`.

## Code style

- Whole classes in the `.h` for now; splitting into `.h`/`.cpp` is planned.
- Epic naming conventions (A/U/F/E/I/T prefixes, `b` for bools, PascalCase), with two deliberate exceptions:
  - `public:` and `private:` are indented inside the class, with members one level deeper;
  - `this->` on every member access, methods and fields alike.
- Vertical code: it grows downward, not sideways.
  - No inline conditionals (one-line `if`, ternary operator) or several statements per line.
  - Object constructions and calls go into named temporaries before being passed as arguments; simple literals (e.g. `1000.f`) may go directly.
  - Simple reads used in an expression (such as `Num()` or a vector component) usually get named temporaries too.
  - `TEXT(...)` counts as a call: it may initialize a named variable (`FName MeshName = TEXT("Mesh")`) but stays out of argument lists, brace initializers included. For literal data such as a stub, prefer one typed array per field, combined in a loop, with a `check` that the sizes match. Plain string literals are fine for ASCII text; non-ASCII text needs `TEXT`.
  - Long access paths are broken across lines up to the called method.
- Few comments.
- Follow this style in every detail when suggesting code; when in doubt, mirror the existing files.
- Markdown and text files (`CLAUDE.md`, `README.md`, `LICENSE`): one line per paragraph or list item, never wrapped by hand.

## Author's environment

- Windows 10, Visual Studio 2022 17.14, Development Editor configuration.
- The editor is launched from Visual Studio with Ctrl+F5, without the debugger. Debug with logs (`UE_LOGFMT`); do not suggest the debugger.
- Live Coding handles small changes; reinstancing and automatic compilation of new classes are off. Structural changes (new classes, new or changed `UPROPERTY`/`UFUNCTION`, other reflection changes) need the editor closed and a rebuild with Ctrl+F5. Flag a structural suggestion in a few words, without repeating these steps, and group structural changes when possible.
- The author creates new source files with Visual Studio's Add New Item, setting Location to the right folder: always state the exact folder.
- Git through TortoiseGit, committing directly to `main`. Keep git simple: no branch workflows. `.gitignore` in place, no Git LFS (the repository stays minimal).
- Visual Studio's formatter moves `public:` and `private:` back to the class's indentation. When reviewing pasted code, point out this lost indentation.

## Game design

- In the style of logic course exercises: the player simplifies "complex" boolean expressions.
- Each symbol of the expression is a block in a row. Proposition blocks hold a sentence (e.g. "John eats tofu") and a letter, given in order of first appearance (same sentence, same letter); each can be minimized (letter) or maximized (whole sentence). Operator blocks: the constants ⊤ and ⊥, ¬, ∧, ∨, ⊕, NAND (↑), NOR (↓), →, ← and ↔, and the parentheses.
- A level has three texts: a sentence in natural language shown to the player, the same sentence written as a formula (the starting formula) and the solution, also as a formula. In these formulas, propositions go in brackets (`[John eats tofu]`) and operators are symbols or ASCII aliases (`&`, `~`, `->`; `|` is OR, so NAND needs another alias). Levels will live in a DataTable imported from CSV; more columns (difficulty, move or time limits) may come later.
- The player moves, adds and removes blocks freely. The game does almost nothing for the player (a choice for simplicity, open to revision).
- Two indicators:
  - Validity: is the sequence a well-formed formula?
  - Equivalence with the level's starting expression: cyan if equivalent, red if not, gray when the expression is invalid.
- Visuals: simple 2D made with basic 3D. Top-down orthographic camera; blocks are meshes, not textures.

## Project layout

```
README.md            project overview (GitHub page)
LICENSE              MIT
Source/ThisIsLogic/
  BasicComponents/   CameraPawn.h, ThisIsLogicGameMode.h, ThisIsLogicPlayerController.h
  Gameplay/Block/    Block.h, BlockFactory.h
  Gameplay/Formula/  Formula.h
  Input/             PointerTarget.h
  Logic/             LogicTypes.h, FormulaStub.h (logic core, no engine dependencies)
Content/             folders by feature
  Levels/            Main (startup map)
  Gameplay/Block/    M_Block (Unlit, Color parameter), BP_Block
  Input/             IA_Click, IMC_Gameplay
  Core/              BP_ThisIsLogicGameMode (default GameMode), BP_ThisIsLogicPlayerController
```

`ThisIsLogic.Build.cs` depends on `EnhancedInput` and `ProceduralMeshComponent`.

Classes (summary only; read the code for details):

- `ACameraPawn`: top-down orthographic camera, set up in `BeginPlay`.
- `AThisIsLogicGameMode`: uses `ACameraPawn` as the default pawn.
- `AThisIsLogicPlayerController`: adds the mapping context and binds `IA_Click` (Started, Triggered, Completed). Routes the mouse to whatever is under the cursor through `IPointerTarget`; knows no gameplay classes (see Input design).
- `IPointerTarget`: C++-only interface with `PointerPressed`, `PointerHeld` (every frame while the button is down) and `PointerReleased`. Points are in world space, on the horizontal plane through the press point.
- `ABlock`: procedural rectangle mesh, color through a dynamic material instance, `Text` shown by a text render component (`Label`). `Width` comes from the text (plus `TextMargin` on each side, never less than `Height`) and is applied in `OnConstruction`. `SetText` only stores the text, between a deferred spawn and `FinishSpawning`. Implements `IPointerTarget`: keeps its grab offset and asks its owner to move it while held and to drop it on release. Owner events: width changed (`SetWidthChangedListener`), move requested (`SetMoveRequestedListener`) and drop requested (`SetDropRequestedListener`).
- `AFormula`: in `BeginPlay`, gets tokens from `FFormulaStub` and creates one block per  token through `FBlockFactory` (`BlockClass`, set to `BP_Block` in the Details panel). Owns its blocks (`AddBlock` sets the owner and listens to their events) and lays them out as a row (see Layout design). On a move request it keeps only Y, clamps it to the row, reorders the row and lifts the block above it; on a drop request it lays the row out again, which puts the block into its slot.
- `FBlockFactory`: plain C++, built with the world and the block class. `CreateBlock(Token)` spawns a block with a deferred spawn, so the text is set before `OnConstruction`.
- `FFormulaStub`: plain C++; returns the tokens of a fixed example formula, in place of the level data and the analyzer. Temporary: its formula changes freely, and it may be deleted or reused in a test.
- `ETokenKind` and `FToken` (`LogicTypes.h`): plain C++, no reflection. A token is a kind and its text. One kind per concept; notation variants belong to the analyzer.

## Input design

- The player controller is a thin router (Mediator). It speaks only in mouse terms (pressed, held, released); each element decides what they mean (drag, click, ...).
- It remembers which element received the press (capture), so held and released reach it even after the cursor has left it.
- Devices, keys and combinations (right click, wheel, double click, Shift + click) are Enhanced Input data: Input Actions, Mapping Contexts and Triggers. Add one Input Action per intention, not code per device.
- Input that points (mouse) goes to the element under the cursor. Input that does not point (keyboard) should go to a current selection or to the game, when that is needed.
- Rejected: engine click events (no capture, no drag event) and `EnableInput` on each actor (every actor would check every click).

## Layout design

- `Blocks` is the only source of the order. Positions are always derived from it by `LayoutBlocks` (left to right from the row's left edge, adding widths), never adjusted one block at a time. The row is centered on the formula; its width is the sum of the block widths (`TotalWidth`, kept in sync).
- While dragging, the block swaps with a neighbor when its edge facing that neighbor passes the neighbor's center (it covers half of it). Only the neighbors are checked each frame; the row is laid out again after each swap, and the dragged block is then put back under the cursor in the same frame.
- Rejected: attaching each block to its left neighbor (dragging would carry the blocks on its right, and the order would live in two places); swapping when the center passes the neighbor's center (a wide block cannot pass a narrow one at the end of the row) or the neighbor's edge (blocks of different widths swap back and forth every frame).

## Block creation design

- Data flow: level texts (DataTable) → analyzer → tokens → factory → blocks → formula. For now `FFormulaStub` stands in for the level data and the analyzer and returns the same tokens the analyzer will.
- The factory is the only place that knows which block class to create (Simple Factory); `AFormula` knows only `ABlock`. Actors cannot take constructor parameters (the engine calls the default constructor, also for the class default object), so the factory uses a deferred spawn to fill in a block before `OnConstruction`.
- Planned: `ABlock` becomes abstract, with `APropositionBlock` (sentence, letter, minimized or maximized; decides which text to show) and `AOperatorBlock` (operator kind, fixed text). `ABlock` keeps what all blocks share: showing a text, recomputing the width, rebuilding the mesh and notifying the owner (a protected method for the subclasses). Letters come from a small level-wide table (sentence → letter), which can also feed a legend like the "Define:" lists of textbook exercises.
- Shared types are grouped by domain (`Logic/LogicTypes.h`), not by kind of type.
- Rejected: parentheses as proposition markers (they also group); splitting the formula text only at spaces (propositions contain spaces); global `Structs`/`Enumerators` files in a `SharedData` folder (unrelated types side by side, included everywhere).

## Roadmap

Items are numbered in order. A new item takes its place in the sequence, and the items after it are renumbered. `[x]` done, `[~]` in progress, `[ ]` to do.

**1 Blocks**
- [x] 1.1 Drag and drop blocks with the mouse: pick the block under the cursor, move it while the button is held, drop it on release.
- [x] 1.2 Reorganize the input (see Input design).
- [x] 1.3 Lay out the blocks as a row and reorder them by dragging (see Layout design).
- [x] 1.4 Show a text on each block; the width comes from the text.
- [x] 1.5 Create the formula's blocks from tokens, through a factory and a stub (see Block creation design).
- [ ] 1.6 Split `ABlock` into an abstract class with `APropositionBlock` and `AOperatorBlock`; the factory chooses by token kind.
- [ ] 1.7 Minimize and maximize proposition blocks.
- [ ] 1.8 Levels in a DataTable imported from CSV, starting with stubs.
- [ ] 1.9 The analyzer: text → tokens, plain C++, checked with logs. First piece of the logic core.
- [ ] 1.10 Arrange the analyzer and the formula so that neither takes on unrelated tasks.
- [ ] 1.11 Decide whether precomputed levels are worth saving.
- [ ] 1.12 Rounded block corners.
- [ ] 1.13 Add and remove blocks.
- [ ] 1.14 Equivalence indicator (cyan / red / gray), with a stub.
- [ ] 1.15 Validity indicator, with a stub.

**2 Logic core (plain C++)**
- [ ] 2.1 Represent boolean expressions as a tree: propositions, constants and the operators of `ETokenKind`.
- [ ] 2.2 Parse the tokens into a tree, with precedence and parentheses; this also decides validity.
- [ ] 2.3 Evaluate expressions and check equivalence with a truth table.
- [ ] 2.4 Measure the size of an expression (basis for goals and scoring).
- [ ] 2.5 Compute the minimal form of an expression (goal of each level). Cost under review: the solution in the level data may be enough.
- [ ] 2.6 Automated tests for the core.

**3 Game rules**
- [ ] 3.1 Win condition.
- [ ] 3.2 Laws as molds to fit blocks into (De Morgan and similar).
- [ ] 3.3 Progression between levels.

**4 Interface and polish**
- [ ] 4.1 Menu, level select and HUD (in C++).
- [ ] 4.2 Visual feedback (simple animations).
- [ ] 4.3 Save progress.

**Ideas, not scheduled**
- A mode to prove equivalence.
- A mode like textbook exercises: pick the formula that matches a sentence.
- A formula in several rows, if long propositions call for it (decide by feel).
- Minimized proposition blocks shaped as a small circle with the letter.

## Pending

- Remove the engine dependencies of the logic core: `LogicTypes.h` and `FormulaStub.h` include `CoreMinimal.h` (`FString`, `TArray`, `check`). Acceptable while the stub stands in for the analyzer. Tokens reach gameplay code (`FBlockFactory`), so their texts will need a conversion at that border; details to be decided with the analyzer.