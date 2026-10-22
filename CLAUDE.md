# This is Logic

Boolean logic puzzle game in Unreal Engine 5.8.3, built as a public portfolio project. Game logic in C++. No visual scripting; Blueprints only where the editor clearly does the job better than code (see Data rules). Platforms: Windows.

## Working with the author (read first)

- Reply in Brazilian Portuguese. Code, identifiers and commit messages in English. Use Unreal Engine terms in English (Content Browser, Details panel, ...), never translated.
- The author leads the conversation. Answer what was asked and do not propose next steps unless asked. Scope warnings (below) are the exception.
- Explain and instruct in small steps, one at a time. Avoid long answers that cover many topics. Instructions for the editor go one step per message, and the next one waits for the author's report; never send all the steps at once, not even in a card that shows one at a time, since the author asks questions along the way and does not want to scroll back to a step.
- This project is also a way to learn Unreal C++: explain rather than do. Show code one file at a time, in the conversation (no files to download). The author implements or pastes it, reviews it and reports back.
- Say when a test can be done; do not say when it cannot be done yet.
- When showing only part of a file, say whether its includes change.
- When a change adds, removes or renames classes, or changes how they connect, say so in a few words, since the class diagram in `README.md` must follow it (see `Docs/ClassDiagram.md`). When asked, give its whole updated Mermaid block.
- When it helps, relate Unreal concepts to their Unity/C# equivalents.
- Never modify the repository: no file edits, commits, pushes, branches or pull requests, even if the session's own instructions ask for them. Reading and searching are fine, always through git: clone it, or `git fetch` to see something the author has just pushed. Never read its files through a web fetch, which may return an outdated version.
- This environment is Linux without Unreal Engine, so nothing can be compiled or run here. Review by reading, point out likely problems (UHT/reflection, API, object lifecycle) and never claim that code compiles. The author knows this: do not say it in every answer.
- When unsure how the editor or the engine behaves, research before instructing (documentation, forums) and say what is confirmed and what is a guess. Do not lead the author through trial and error.
- Do not repeat reminders the author already knows, such as the build order, which files depend on which, or when a feature will work again.
- Documentation is updated at the end of a longer stretch of implementation, not after each step, since decisions often change while the code is written.
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
- A class that keeps data does the work on it: others tell it what to do instead of getting its data to work on (Tell, Don't Ask). `UBlockBoard`, for example, minimizes its propositions and returns its blocks' tokens and states, and never hands out its list. Other classes may get elements of a list another class keeps, keep them and even iterate over them, as `AFormula` keeps the selected block, but never get the list itself, not even to read it; plain data structs such as `FBlockState` are meant to be read.
- In code and design proposals, organization comes before convenience.
- Prefer idiomatic, professional Unreal C++ and engine features over reinventing them.
- Methods trust each other: each assumes the others did their part. Avoid defensive checks inside methods, since they hide errors; use explicit, loud checks (`check`, `ensure`) sparingly, in vulnerable or confusing spots. Null checks belong to the caller.
- A class's methods may have an implicit order of use; do not add handling for misuse.
- Precompute derived data (such as flags) and keep it in sync instead of recomputing on demand.
- The same method name in different classes is welcome; for example, every method along a path that carries data down a hierarchy can share the name.
- Prefer plain helper classes held as members and built with constructor parameters over piling components onto one actor.
- Stateless helpers are static classes: static methods and a deleted constructor.
- No attribute is public. A subclass may use a protected attribute of its base, as the formula and the palettes use the board of `AAbstractBlocksManager`. Better still, a base keeps an attribute private and gives its subclasses protected methods for what they decide, such as `ABlock::SetColor` for its background, so that they do not even know the attribute's class, when every subclass uses those methods (see the next item); it does not wrap an object whose operations only some subclasses use, which would only pass the calls on (Middle Man).
- Inheritance: a base class has only what all its subclasses use, with no method, field or other code that some subclass does not use; a subclass that inherits what it does not use (the Refused Bequest smell) shows that the code belongs elsewhere. What only some subclasses need goes to an object they contain (see the next item). The cases still in the code are listed in Pending.
- Composition: an object of a class that others use, held as a member, may offer features that not every holder uses, as `UBlockBoard` minimizes the propositions only for the formula.
- When a base class needs something that each subclass decides, it asks through a virtual method (Template Method, such as `ABlock::GetText`) instead of keeping a copy that the subclasses push to it.
- No method chaining or fluent interfaces.
- Keep suggestions small and testable in the editor. Prefer increments that show something on screen, and stub whatever depends on unfinished systems.
- The logic core (`Logic/`) has no engine dependencies, only standard C++, so it can be tested and reused on its own. Conversions to engine types happen where gameplay code calls it.
- Calls go down, events go up: a class calls the objects it holds directly; a part only notifies through an event and never knows who listens. The listener is bound by whoever connects them: the part's owner, or the GameMode, which assembles the table (see `Docs/Assembly.md`).
- Two levels of abstraction: the problem, a sequence of tokens (the exercise and the logic core), and the blocks, the actors the player handles (the formula, the palettes and their parts). The exercise talks to the formula in tokens and block states and never touches a block; the formula and the palettes share a base, `AAbstractBlocksManager`, which keeps their board, binds its events and gives every block the same height (see `Docs/BlocksManager.md`).
- Only the factory and the board know the concrete block classes (`APropositionBlock`, `AOperatorBlock`); the other classes know at most `ABlock`, the abstraction, and plain data (`FBlockState`, `FToken`), so they depend on abstractions rather than on concrete classes (Dependency Inversion). In the class diagram, many arrows to `ABlock` are fine, while arrows to the concrete classes from outside the factory and the board are not.
- The GameMode assembles the table (Composition Root): it creates the pieces, decides where they go and connects their events, and does nothing after that. Pieces receive what they need from it instead of looking for it; hand-made singletons, and subsystems used to look pieces up, are avoided (see `Docs/Assembly.md`).
- "Owner", in design discussions, has its object-oriented meaning: the object that keeps another, in a field or a list, and ends its lifetime. Unreal's actor `Owner` (`SetOwner`) is a different thing: a link between actors that the engine uses mainly for networking and visibility, which neither encapsulates nor destroys anything. Here, each `UBlockBoard` owns its blocks, since it creates, keeps and destroys them, and it listens to their events, while the actor that holds the board, a subclass of `AAbstractBlocksManager` (the formula or a palette), is only their engine `Owner`, set by the board (see `Docs/Layout.md`). So names that call that actor an owner, such as `ABlockOwner`, are avoided.
- Events use single-listener delegates (`DECLARE_DELEGATE_*`), bound through a `Set...Listener` method and called with `Execute` after a `check` on `IsBound()`. An event without a listener is an error. Prefer explicit calls over optional ones such as `ExecuteIfBound`.

## Code style

- Whole classes in the `.h` for now; splitting into `.h`/`.cpp` is planned.
- Plain data structs live in files of their own, apart from the classes that use them, such as `FBlockState` in `BlockData.h`.
- Epic naming conventions (A/U/F/E/I/T prefixes, `b` for bools, PascalCase), with two deliberate exceptions:
  - `public:` and `private:` are indented inside the class, with members one level deeper;
  - `this->` on every member access, methods and fields alike; static members go through the class name (`AFormula::BlockHeight`), since they belong to no object. A constructor's initializer list is the one exception, since its syntax does not take `this->`.
- Vertical code: it grows downward, not sideways. Lines stay within about 68 columns, counting a tab as 4: the author splits the IDE into two panes and uses a vertical monitor, and does not want to scroll sideways.
  - When a line must be broken, the breaks come as early in it as possible, such as after the `=` of an assignment or after a return type, rather than near its end.
  - No inline conditionals (one-line `if`, ternary operator) or several statements per line.
  - When an `if` returns and the other outcome is also a return, that return goes in an `else`, not after the `if`.
  - Object constructions and calls go into named temporaries before being passed as arguments; simple literals (e.g. `1000.f`) may go directly.
  - Simple reads used in an expression (such as `Num()` or a vector component) usually get named temporaries too.
  - `TEXT(...)` counts as a call: it may initialize a named variable (`FName MeshName = TEXT("Mesh")`) but stays out of argument lists, brace initializers included. For literal data such as a stub, prefer one typed array per field, combined in a loop, with a `check` that the sizes match. Plain string literals are fine for ASCII text; non-ASCII text needs `TEXT`.
  - Long access paths are broken across lines up to the called method.
- Includes come in groups separated by a blank line, with the `.generated.h` last; within each group, from the shortest line to the longest.
- Large classes may group their members in `#pragma region` blocks, as a navigation aid in the IDE, indented one level inside the class, like `public:` and `private:`; only sections big enough to be worth folding get one. Their names and order are not fixed: follow the regions a class already has, as its file shows, and do not suggest removing them; spreading them to other classes is not a priority, and refactoring a class is often better.
- A method that subclasses must override gets a normal body with `unimplemented()` and a placeholder return, since a `UCLASS` cannot have pure virtual methods (`= 0`); `PURE_VIRTUAL` is avoided because its notation is confusing.
- A subclass never declares a method with the name of one of its base's, except to override a virtual one: in C++ it hides the base's, usually without a warning (C# warns and asks for `new`), and a call by that name inside the subclass reaches its own method. Choose another name.
- Few comments.
- Follow this style in every detail when suggesting code; when in doubt, mirror the existing files.
- Markdown and text files (`CLAUDE.md`, `Docs/`, `README.md`, `LICENSE`): one line per paragraph or list item, never wrapped by hand.

## Author's environment

- Windows 10, JetBrains Rider opening `ThisIsLogic.uproject` directly, Development Editor configuration. Rider replaced Visual Studio, whose IntelliSense showed false errors in headers with UE 5.8; Visual Studio 2022 17.14 stays installed only for the MSVC toolchain and the Windows SDK.
- The .NET 10 SDK is installed, since UE 5.8's tools need it. RiderLink is installed in the engine, not in the project, so it stays out of the repository.
- DejaVu Sans 2.37 is installed in Windows, since the editor generates the blocks' offline font from an installed font (see `Docs/Font.md`).
- The editor is launched from Rider with Run, without the debugger. Debug with logs (`UE_LOGFMT`); do not suggest the debugger.
- The author keeps the editor closed while writing code and opens it only to test, after a build from Rider, so structural changes (reflection, new classes, changed defaults) need no warning.
- The author creates new source files in Rider, in the right folder: always state the exact folder.
- Git through TortoiseGit, committing directly to `main`. Keep git simple: no branch workflows. `.gitignore` in place, no Git LFS (the repository stays minimal).
- An IDE's formatter may move `public:` and `private:` back to the class's indentation. When reviewing pasted code, point out this lost indentation.

## Game design

- In the style of logic course exercises: the player simplifies "complex" boolean expressions.
- Each symbol of the expression is a block, and the blocks are laid out in rows, like the lines of a text. Proposition blocks hold a sentence (e.g. "John eats tofu") and a letter, which the notation (below) derives from the proposition's number; each can be minimized (the letter) or maximized (the letter and the sentence, as in "P: John eats tofu"). Operator blocks: the constants ⊤ and ⊥, ¬, ∧, ∨, ⊕, NAND (↑), NOR (↓), →, ← and ↔, and the parentheses.
- A level has four parts, all in English: the statement in natural language shown to the player; the starting formula, with each proposition written as a number (`1 = ( 2 AND 3 ) OR ( 2 AND NOT 3 )`); the sentences of the propositions, in order (the first one is proposition 1); and the solution, the minimal formula, written the same way. In a formula, the pieces are separated by spaces, and each operator has a single text: `TRUE`, `FALSE`, `NOT`, `AND`, `OR`, `XOR`, `NAND`, `NOR`, `THEN` (→), `IF` (←), `=` (↔) and the parentheses. A piece that is neither one of them nor a proposition number is an error that stops the game. A result variable, as in `1 = ...`, is part of the formula and becomes ordinary blocks, so each level is shown in the form it was written. Levels live in a DataTable imported from a CSV file, one level per row; more columns (difficulty, move or time limits) may come later.
- The notation changes only what the screen shows, never the levels: the letters, a run of consecutive letters from a first letter (P, Q, R...), and the text of each operator, in a symbol style: the textbook symbols (∧, ∨, ↔...) or ASCII texts. The player will choose it, to match the course they follow. For now there is one, from P, with the textbook symbols.
- The player moves, adds and removes blocks freely. New blocks come from two palettes: the operators, in a column on the left of the table, and the level's propositions, at the top, laid out in rows like the formula and doubling as the legend of their sentences (see `Docs/Palette.md`). The game does almost nothing for the player (a choice for simplicity, open to revision).
- Two indicators:
  - Validity: is the sequence a well-formed formula?
  - Equivalence with the level's starting expression: cyan if equivalent, red if not, gray when the expression is invalid.
- Visuals: simple 2D made with basic 3D, at the real size of a board game (see `Docs/Scale.md`). Top-down orthographic camera, which keeps a minimum area of the table in view on windows of any shape; blocks are meshes, not textures: capsules, or circles when the text is short.

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
- `Docs/Assembly.md`: the GameMode as the Composition Root (what it creates, in which order, how it connects the pieces, and the selection event that reaches the panel).
- `Docs/Exercise.md`: `AExercise`, the problem level: its level, starting tokens and undo history, Restart and Undo.
- `Docs/BlocksManager.md`: the abstract base of the formula and the palettes (`AAbstractBlocksManager`): the board it keeps as a protected attribute, the events it binds, what it asks of its subclasses (Template Method and hooks), the single block height, and the requests that live in the subclasses.
- `Docs/Layout.md`: the board, the blocks' facade (`UBlockBoard`), and its rows (`FBlockBoardRows` and `FBlockRow`), shared by the formula and the palettes: creating, registering and removing the blocks, their order, spacing and line breaks, and dragging within and between rows.
- `Docs/Palette.md`: the palettes (`APalette` and its subclasses, the operator column, the proposition rows, and why they are actors in the world).
- `Docs/Levels.md`: the level format (statement, formula with numbered propositions and operator words, sentences, solution), the result variable, and the levels' file and DataTable.
- `Docs/BlockCreation.md`: from tokens to blocks (the flow, the factory in the board, deferred spawn, the notation and the text of each block, the formula stub).
- `Docs/Font.md`: the blocks' font (why an offline distance field font, DejaVu Sans, its import options and how to regenerate it).
- `Docs/Logic.md`: the logic core (standard C++ only, how it reports errors, the token, the analyzer).
- `Docs/Background.md`: the rounded background mesh (triangle fan, corners, radius, collision).
- `Docs/Scale.md`: the real-world scale (the unit, the blocks' single height and the sizes derived from it, the table's arrangement in the GameMode, the background's depth and the camera's framing).
- `Docs/Data.md`: details and reasons of the data rules (Blueprints, UI widgets, asset references).
- `Docs/ClassDiagram.md`: the class diagram in its two forms (the Visual Paradigm drawing, shown first in the README, and the Mermaid text, kept up to date with the code), why the drawing matters, the `Images/` folder and the notation.

## Project layout

```
README.md            project overview (GitHub page), with the class diagram in Mermaid
LICENSE              MIT
Docs/                design decisions by area (see Design docs)
Images/              pictures shown in the README (the class diagram drawn in Visual Paradigm)
Data/
  Levels.csv         the levels, source of DT_Levels
  Fonts/DejaVuSans/  DejaVuSans.ttf and its LICENSE, source of Font_DejaVuSans
Config/
  DefaultGame.ini    values of UThisIsLogicSettings (asset paths)
Source/ThisIsLogic/
  BasicComponents/   camera pawn, game mode (which assembles the table), player controller, settings
  Gameplay/Block/    blocks, their data, their factory, the board that creates and keeps them, its rows, and the abstract base of the formula and the palettes
  Gameplay/Exercise/ the exercise being played, and its undo history
  Gameplay/Formula/  the formula, its blocks laid out in rows
  Gameplay/Level/    the row of the levels' DataTable, and the formula stub for quick tests
  Gameplay/Notation/ the notation: letters and operator texts shown to the player
  Gameplay/Palette/  the palettes: a base that shows blocks from tokens, and the operator palette
  Input/             pointer interface and background target
  Logic/             logic core (no engine dependencies)
  UI/                UMG widgets (C++ classes) and their helpers
  Visuals/           reusable visual components
Content/             folders by feature
  Levels/            Main (startup map; the GameMode creates every piece, so it holds none)
  Gameplay/Block/    M_Block (Unlit, Color parameter), Font_DejaVuSans (offline font)
  Gameplay/Level/    DT_Levels (imported from Data/Levels.csv)
  Input/             IA_Click, IMC_Gameplay
  Core/              BP_ThisIsLogicGameMode (default GameMode), BP_ThisIsLogicPlayerController
  UI/                WBP_CommandPanel (layout of UCommandPanel)
```

Read the code for the classes in each folder.

## Roadmap

Items are numbered in order. A new item takes its place in the sequence, and the items after it are renumbered. `[x]` done, `[~]` in progress, `[ ]` to do.

**1 Blocks**
- [x] 1.1 to 1.24, summarized (the git history has the details): drag and drop, input routing, the row and its reordering, text and width, blocks from tokens through a factory, rounded corners, proposition and operator blocks, minimize and maximize, selection, `AExercise`, the panel (a UMG widget with Minimize all and Maximize all, always visible, and Minimize/Maximize for a selected proposition, replacing the temporary trigger), the long press (a third gesture, recognized by the controller; on a proposition, a shortcut for Minimize/Maximize), propositions by number, with the letters and the operator texts from the notation, the levels in a DataTable imported from CSV, the analyzer, the first piece of the logic core, which turns the level's formula into tokens and stays apart from the formula, the rows (the blocks wrap like the lines of a text, and a dragged block can move to another row), the textbook symbols (an offline distance field font made in the editor from DejaVu Sans, and a symbol style passed to the notation, like its first letter), the decision not to save precomputed levels (see `Docs/Levels.md`), removing blocks, with Restart and Undo (a history of the blocks' states; see `Docs/Commands.md`), the camera's framing, which keeps a minimum area of the table in view on windows of any shape (see `Docs/Scale.md`), the GameMode assembling the table, with the exercise speaking only in tokens (see `Docs/Assembly.md`), the operator palette, a column on the left of the table whose tap inserts the operator before the selected block, or at the end of the formula with nothing selected, keeping the selection like a text cursor (see `Docs/Palette.md` and `Docs/Commands.md`), the abstract base of the formula and the palettes, `AAbstractBlocksManager` (see `Docs/BlocksManager.md`), the formula telling a selected proposition by its token's kind, instead of a cast to `APropositionBlock`, and the board as the blocks' facade: a `UObject` that keeps the factory and the sentences, creates the blocks, registers their events and relays their requests to the manager, with the rows and dragging in `FBlockBoardRows`, while the base keeps only what every subclass uses and the subclasses use the board directly (see `Docs/Layout.md` and `Docs/BlocksManager.md`).
- [ ] 1.25 Gestures reported by the blocks and relayed by the board, with each subclass of the manager deciding what they mean, so the propositions' own long-press event goes away; then a long press on a palette item inserts after the selected block, as a shortcut.
- [ ] 1.26 The proposition palette: the level's propositions at the top of the table, laid out in rows like the formula and shown with their letters and sentences ("P: Alarm on"), as the maximized propositions of the formula will be; it doubles as the legend.
- [ ] 1.27 Equivalence indicator (cyan / red / gray), with a stub.
- [ ] 1.28 Validity indicator, with a stub.

**2 Logic core (plain C++)**
- [ ] 2.1 Represent boolean expressions as a tree: propositions, constants and the operators of `ETokenKind`.
- [ ] 2.2 Parse the tokens into a tree, with precedence and parentheses; this also decides validity.
- [ ] 2.3 Evaluate expressions and check equivalence with a truth table.
- [ ] 2.4 Measure the size of an expression (basis for goals and scoring).
- [ ] 2.5 Automated tests for the core.

**3 Game rules**
- [ ] 3.1 Win condition.
- [ ] 3.2 Select a sub-formula: tapping the selected block again widens the selection.
- [ ] 3.3 Define a selected sub-formula as a new proposition: it takes the next free letter, replaces the sub-formula and joins the proposition palette with its definition, so it can be inserted elsewhere. This is how a new letter enters the formula; the palette has no New letter item.
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
- Drag from a palette to insert, and drop a block on a palette to remove it (see `Docs/Commands.md`).

## Pending

- Remove the Blueprints that only hold references (see Data rules): `BP_ThisIsLogicPlayerController` only holds `IMC_Gameplay` and `IA_Click`, which can move to `UThisIsLogicSettings`; `BP_ThisIsLogicGameMode` can then go too if it only points to that controller (the C++ GameMode would set the controller class, and the project's default GameMode would be the C++ class).
- A block wider than the formula's area (only a very long sentence, maximized) jumps between two positions when dragged across, since its horizontal limits cross. No level has such a sentence yet.
- `UCommandPanel` binds its buttons by name, written as text, through `FAuxUI::AssignButtonClick`, which uses `FScriptDelegate::BindUFunction`: a typo or a renamed handler still compiles and leaves its button silent, where `AddDynamic` or a name from `GET_FUNCTION_NAME_CHECKED` would break the build. Kept for now, to be investigated.
- The positions on the table (the formula, the operator palette) are first values set by eye in the GameMode; they are revisited with the proposition palette.
- `AExercise` reads the levels' table from `UThisIsLogicSettings` itself; the GameMode, as the Composition Root, should read the configuration and give the exercise what it needs (see `Docs/Assembly.md`), which also removes the exercise's arrow to the settings in the class diagram.
- Code against the inheritance rule (see Engineering principles): `ABlock::FitToText` is used only by `APropositionBlock`.

## Maintaining this file

- This file holds only what every conversation needs and cannot learn from the code: how to work with the author, rules, environment, game design, data rules and the roadmap.
- An area's decisions, with their reasons and rejected alternatives, go in its file in `Docs/`. A new area gets a new file, listed in Design docs.
- What the code shows (class roles, members, call order) is described neither here nor in `Docs/`: read the code.
- When a decision changes, rewrite or remove the text it replaces instead of adding to it.
