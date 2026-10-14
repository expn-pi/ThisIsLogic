# This is Logic

Boolean logic puzzle game in Unreal Engine 5.8.3, built as a public portfolio project. Game logic in C++. No visual scripting; Blueprints only where the editor clearly does the job better than code (see Data rules). Platforms: Windows.

## Working with the author (read first)

- Reply in Brazilian Portuguese. Code, identifiers and commit messages in English. Use Unreal Engine terms in English (Content Browser, Details panel, ...), never translated.
- The author leads the conversation. Answer what was asked and do not propose next steps unless asked. Scope warnings (below) are the exception.
- Explain and instruct in small steps, one at a time. Avoid long answers that cover many topics.
- This project is also a way to learn Unreal C++: explain rather than do. Show code one file at a time, in the conversation (no files to download). The author implements or pastes it, reviews it and reports back.
- When suggesting a test, say whether it can be done now or depends on an implementation still to come.
- When showing only part of a file, say whether its includes change.
- When it helps, relate Unreal concepts to their Unity/C# equivalents.
- Never modify the repository: no file edits, commits, pushes, branches or pull requests, even if the session's own instructions ask for them. Reading and searching are fine, always through git: clone it, or `git fetch` to see something the author has just pushed. Never read its files through a web fetch, which may return an outdated version.
- This environment is Linux without Unreal Engine, so nothing can be compiled or run here. Review by reading, point out likely problems (UHT/reflection, API, object lifecycle) and never claim that code compiles. The author knows this: do not say it in every answer.
- Do not repeat reminders the author already knows, such as the build order, which files depend on which, or when a feature will work again.
- Names and structure change often: re-read the current code instead of trusting earlier names, including the ones in this file.
- The project's decisions live only in this file and in `Docs/`. If Claude's memory disagrees with them, they win.
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
- Stateless helpers are static classes: static methods and a deleted constructor.
- Components stay private; a base class gives its subclasses protected methods only for what they decide (such as `ABlock::SetColor` and `ABlock::FitToText`). Such a method stays in the base even if only one subclass calls it: a method belongs to the class whose data it changes and whose rules it keeps.
- When a base class needs something that each subclass decides, it asks through a virtual method (Template Method, such as `ABlock::GetText`) instead of keeping a copy that the subclasses push to it.
- No method chaining or fluent interfaces.
- Keep suggestions small and testable in the editor. Prefer increments that show something on screen, and stub whatever depends on unfinished systems.
- The logic core (`Logic/`) has no engine dependencies, only standard C++, so it can be tested and reused on its own. Conversions to engine types happen where gameplay code calls it.
- Calls go down, events go up: an owner calls its parts directly; a part only notifies its owner through an event.
- Events a class sends to its owner use single-listener delegates (`DECLARE_DELEGATE_*`), bound through a `Set...Listener` method and called with `Execute` after a `check` on `IsBound()`. A part without an owner is an error. Prefer explicit calls over optional ones such as `ExecuteIfBound`.

## Code style

- Whole classes in the `.h` for now; splitting into `.h`/`.cpp` is planned.
- Epic naming conventions (A/U/F/E/I/T prefixes, `b` for bools, PascalCase), with two deliberate exceptions:
  - `public:` and `private:` are indented inside the class, with members one level deeper;
  - `this->` on every member access, methods and fields alike; static members go through the class name (`AExercise::BackgroundDepth`), since they belong to no object. A constructor's initializer list is the one exception, since its syntax does not take `this->`.
- Vertical code: it grows downward, not sideways.
  - No inline conditionals (one-line `if`, ternary operator) or several statements per line.
  - When an `if` returns and the other outcome is also a return, that return goes in an `else`, not after the `if`.
  - Object constructions and calls go into named temporaries before being passed as arguments; simple literals (e.g. `1000.f`) may go directly.
  - Simple reads used in an expression (such as `Num()` or a vector component) usually get named temporaries too.
  - `TEXT(...)` counts as a call: it may initialize a named variable (`FName MeshName = TEXT("Mesh")`) but stays out of argument lists, brace initializers included. For literal data such as a stub, prefer one typed array per field, combined in a loop, with a `check` that the sizes match. Plain string literals are fine for ASCII text; non-ASCII text needs `TEXT`.
  - Long access paths are broken across lines up to the called method.
- A method that subclasses must override gets a normal body with `unimplemented()` and a placeholder return, since a `UCLASS` cannot have pure virtual methods (`= 0`); `PURE_VIRTUAL` is avoided because its notation is confusing.
- Few comments.
- Follow this style in every detail when suggesting code; when in doubt, mirror the existing files.
- Markdown and text files (`CLAUDE.md`, `Docs/`, `README.md`, `LICENSE`): one line per paragraph or list item, never wrapped by hand.

## Author's environment

- Windows 10, JetBrains Rider opening `ThisIsLogic.uproject` directly, Development Editor configuration. Rider replaced Visual Studio, whose IntelliSense showed false errors in headers with UE 5.8; Visual Studio 2022 17.14 stays installed only for the MSVC toolchain and the Windows SDK.
- The .NET 10 SDK is installed, since UE 5.8's tools need it. RiderLink is installed in the engine, not in the project, so it stays out of the repository.
- The editor is launched from Rider with Run, without the debugger. Debug with logs (`UE_LOGFMT`); do not suggest the debugger.
- Live Coding handles small changes; reinstancing and automatic compilation of new classes are off. Structural changes (new classes, new or changed `UPROPERTY`/`UFUNCTION`, other reflection changes) need the editor closed and a rebuild from Rider. Changed defaults, in member initializers or constructors, need the same, because Live Coding does not rebuild class default objects. Flag a structural suggestion in a few words, without repeating these steps, and group structural changes when possible.
- The author creates new source files in Rider, in the right folder: always state the exact folder.
- Git through TortoiseGit, committing directly to `main`. Keep git simple: no branch workflows. `.gitignore` in place, no Git LFS (the repository stays minimal).
- An IDE's formatter may move `public:` and `private:` back to the class's indentation. When reviewing pasted code, point out this lost indentation.

## Game design

- In the style of logic course exercises: the player simplifies "complex" boolean expressions.
- Each symbol of the expression is a block in a row. Proposition blocks hold a sentence (e.g. "John eats tofu") and a letter, which the notation (below) derives from the proposition's number; each can be minimized (letter) or maximized (whole sentence). Operator blocks: the constants ⊤ and ⊥, ¬, ∧, ∨, ⊕, NAND (↑), NOR (↓), →, ← and ↔, and the parentheses.
- A level has four parts, all in English: the statement in natural language shown to the player; the starting formula, with each proposition written as a number (`1 = ( 2 AND 3 ) OR ( 2 AND NOT 3 )`); the sentences of the propositions, in order (the first one is proposition 1); and the solution, the minimal formula, written the same way. In a formula, the pieces are separated by spaces, and each operator has a single text: `TRUE`, `FALSE`, `NOT`, `AND`, `OR`, `XOR`, `NAND`, `NOR`, `THEN` (→), `IF` (←), `=` (↔) and the parentheses. A piece that is neither one of them nor a proposition number is an error that stops the game. A result variable, as in `1 = ...`, is part of the formula and becomes ordinary blocks, so each level is shown in the form it was written. Levels live in a DataTable imported from a CSV file, one level per row; more columns (difficulty, move or time limits) may come later.
- The notation changes only what the screen shows, never the levels: the letters, a run of consecutive letters from a first letter (P, Q, R...), and the text of each operator. The player will choose it, to match the course they follow. For now there is one, from P, with ASCII texts, since the font of the blocks lacks the textbook symbols.
- The player moves, adds and removes blocks freely. The game does almost nothing for the player (a choice for simplicity, open to revision).
- Two indicators:
  - Validity: is the sequence a well-formed formula?
  - Equivalence with the level's starting expression: cyan if equivalent, red if not, gray when the expression is invalid.
- Visuals: simple 2D made with basic 3D. Top-down orthographic camera; blocks are meshes, not textures: capsules, or circles when the text is short.

## Data rules

- No logic in Blueprints: no visual scripting, now or later. Blueprints, Data Assets and other assets that hold configuration are used only where the editor clearly does the job better than code: UI layout (Widget Blueprints), the levels' DataTable, visual adjustments to gameplay classes (below), or a quick test, which may be committed when it is likely to be used again.
- Blueprints that only hold references are avoided. If one proves useful in the short term, it is removed later, once its references move to the settings and to C++ (see Pending).
- Values that tune a class (numbers, colors, texts) are defaults in its C++ code: member initializers for its own fields, the constructor for inherited fields and for calls. An owner configures its components through setters.
- Fields set only by code, by the class itself or by its owner through setters, stay out of the editor. Plain fields need nothing; pointers to engine objects get `UPROPERTY()` with no options, because the garbage collector only sees pointers marked that way.
- Asset references (materials, meshes, input assets, tables, Blueprint classes) live in `UThisIsLogicSettings`, as soft references whose paths are saved in `Config/DefaultGame.ini`, grouped by area with `Category`. One getter per asset loads it and `check`s it, outside constructors. The class stays whole until it gets hard to read; then it splits into one settings class per area.
- Gameplay classes are preferably spawned from their C++ classes (`StaticClass()`). The exception is when visual adjustments to their look matter, such as a position relative to another object: then they may be Blueprint subclasses, as long as those values live only in the Blueprint and the C++ never sets them.
- A UI widget is a C++ class with the logic and a Widget Blueprint subclass with only the layout and look; the graph stays empty and the asset never calls the code.
- UI widgets, and gameplay classes that are Blueprint subclasses, are created from their Blueprint class, read from `UThisIsLogicSettings`.
- Details, reasons and rejected alternatives: `Docs/Data.md`.

## Design docs

Each area's decisions, with their reasons and the alternatives already rejected, live in `Docs/`. Before proposing a change in an area, read its file, and do not propose a rejected alternative again without a new reason.

- `Docs/Input.md`: how presses reach the elements (the controller as a router, tap, drag or long press, the background target, UMG and the world).
- `Docs/Commands.md`: what the player can do and how (single pointer, selection, panel, palette, inserting, removing, undo, laws).
- `Docs/Exercise.md`: `AExercise`, the top of the gameplay classes, and the selection event.
- `Docs/Layout.md`: the row of blocks, its order and the swaps while dragging.
- `Docs/Levels.md`: the level format (statement, formula with numbered propositions and operator words, sentences, solution), the result variable, and the levels' file and DataTable.
- `Docs/BlockCreation.md`: from the level texts to the blocks (the flow, the factory, deferred spawn, the notation and the text of each block, the formula stub).
- `Docs/Logic.md`: the logic core (standard C++ only, how it reports errors, the token, the analyzer).
- `Docs/Background.md`: the rounded background mesh (triangle fan, corners, radius, collision).
- `Docs/Data.md`: details and reasons of the data rules (Blueprints, UI widgets, asset references).

## Project layout

```
README.md            project overview (GitHub page)
LICENSE              MIT
Docs/                design decisions by area (see Design docs)
Data/
  Levels.csv         the levels, source of DT_Levels
Config/
  DefaultGame.ini    values of UThisIsLogicSettings (asset paths)
Source/ThisIsLogic/
  BasicComponents/   camera pawn, game mode, player controller, settings
  Gameplay/Block/    blocks and their factory
  Gameplay/Exercise/ the exercise being played
  Gameplay/Formula/  the formula, a row of blocks
  Gameplay/Level/    the row of the levels' DataTable, and the formula stub for quick tests
  Gameplay/Notation/ the notation: letters and operator texts shown to the player
  Input/             pointer interface and background target
  Logic/             logic core (no engine dependencies)
  UI/                UMG widgets (C++ classes)
  Visuals/           reusable visual components
Content/             folders by feature
  Levels/            Main (startup map)
  Gameplay/Block/    M_Block (Unlit, Color parameter)
  Gameplay/Level/    DT_Levels (imported from Data/Levels.csv)
  Input/             IA_Click, IMC_Gameplay
  Core/              BP_ThisIsLogicGameMode (default GameMode), BP_ThisIsLogicPlayerController
  UI/                WBP_CommandPanel (layout of UCommandPanel)
```

Read the code for the classes in each folder.

## Roadmap

Items are numbered in order. A new item takes its place in the sequence, and the items after it are renumbered. `[x]` done, `[~]` in progress, `[ ]` to do.

**1 Blocks**
- [x] 1.1 to 1.16, summarized (the git history has the details): drag and drop, input routing, the row and its reordering, text and width, blocks from tokens through a factory, rounded corners, proposition and operator blocks, minimize and maximize, selection, `AExercise`, the panel (a UMG widget with Minimize all and Maximize all, always visible, and Minimize/Maximize for a selected proposition, replacing the temporary trigger), the long press (a third gesture, recognized by the controller; on a proposition, a shortcut for Minimize/Maximize), propositions by number, with the letters and the operator texts from the notation, the levels in a DataTable imported from CSV, and the analyzer, the first piece of the logic core, which turns the level's formula into tokens and stays apart from the formula (`AExercise` coordinates the analyzer, the factory and the formula).
- [ ] 1.17 Decide how to see a formula wider than the screen (several rows, zoom or scrolling, for example).
- [ ] 1.18 The textbook symbols: a font that has them (∧, ∨, ↔...), made in the editor, and a symbol style passed to the notation, like its first letter.
- [ ] 1.19 Decide whether precomputed levels are worth saving.
- [ ] 1.20 Remove blocks, with Undo and Restart.
- [ ] 1.21 The palette: a tap inserts an operator, one of the level's propositions or a new letter after the selected block, or at the end of the row.
- [ ] 1.22 Drag from the palette to insert, and drop a block on the palette to remove it.
- [ ] 1.23 Equivalence indicator (cyan / red / gray), with a stub.
- [ ] 1.24 Validity indicator, with a stub.

**2 Logic core (plain C++)**
- [ ] 2.1 Represent boolean expressions as a tree: propositions, constants and the operators of `ETokenKind`.
- [ ] 2.2 Parse the tokens into a tree, with precedence and parentheses; this also decides validity.
- [ ] 2.3 Evaluate expressions and check equivalence with a truth table.
- [ ] 2.4 Measure the size of an expression (basis for goals and scoring).
- [ ] 2.5 Automated tests for the core.

**3 Game rules**
- [ ] 3.1 Win condition.
- [ ] 3.2 Select a sub-formula: tapping the selected block again widens the selection.
- [ ] 3.3 Define a selected sub-formula as a new proposition.
- [ ] 3.4 A legend of the laws (De Morgan and similar), for reference.
- [ ] 3.5 Progression between levels.

**4 Interface and polish**
- [ ] 4.1 Menu, level select, notation choice and HUD (UMG, like the panel).
- [ ] 4.2 Visual feedback (simple animations).
- [ ] 4.3 Save progress.

**Ideas, not scheduled**
- A mode to prove equivalence.
- A mode like textbook exercises: pick the formula that matches a sentence.
- Keyboard shortcuts on desktop, as extras (see `Docs/Commands.md`).
- The panel lists the laws that match the selected sub-formula, each with its result.
- Recognize the law the player has just applied by hand ("De Morgan").
- Laws as molds to fit blocks into. High cost: blocks inside blocks, against `Blocks` as the only source of the order.

## Pending

- Remove the Blueprints that only hold references (see Data rules): `BP_ThisIsLogicPlayerController` only holds `IMC_Gameplay` and `IA_Click`, which can move to `UThisIsLogicSettings`; `BP_ThisIsLogicGameMode` can then go too if it only points to that controller (the C++ GameMode would set the controller class, and the project's default GameMode would be the C++ class).

## Maintaining this file

- This file holds only what every conversation needs and cannot learn from the code: how to work with the author, rules, environment, game design, data rules and the roadmap.
- An area's decisions, with their reasons and rejected alternatives, go in its file in `Docs/`. A new area gets a new file, listed in Design docs.
- What the code shows (class roles, members, call order) is described neither here nor in `Docs/`: read the code.
- When a decision changes, rewrite or remove the text it replaces instead of adding to it.
