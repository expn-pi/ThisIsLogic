# This is Logic

Boolean logic puzzle game in Unreal Engine 5.8.3, built as a public portfolio project. Game logic in C++. No visual scripting, and as few Blueprints as possible (see Data design). Platforms: Windows.

## Working with the author (read first)

- Reply in Brazilian Portuguese. Code, identifiers and commit messages in English. Use Unreal Engine terms in English (Content Browser, Details panel, ...), never translated.
- The author leads the conversation. Answer what was asked and do not propose next steps unless asked. Scope warnings (below) are the exception.
- Explain and instruct in small steps, one at a time. Avoid long answers that cover many topics.
- This project is also a way to learn Unreal C++: explain rather than do. Show code one file at a time, in the conversation (no files to download). The author implements or pastes it, reviews it and reports back.
- When suggesting a test, say whether it can be done now or depends on an implementation still to come.
- When showing only part of a file, say whether its includes change.
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
- Stateless helpers, such as factories, are static classes: static methods and a deleted constructor.
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
  - `this->` on every member access, methods and fields alike; static members go through the class name (`FBlockFactory::CreatePropositionBlock`), since static methods have no `this`.
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
- Markdown and text files (`CLAUDE.md`, `README.md`, `LICENSE`): one line per paragraph or list item, never wrapped by hand.

## Author's environment

- Windows 10, JetBrains Rider opening `ThisIsLogic.uproject` directly, Development Editor configuration. Rider replaced Visual Studio, whose IntelliSense showed false errors in headers with UE 5.8; Visual Studio 2022 17.14 stays installed only for the MSVC toolchain and the Windows SDK.
- The .NET 10 SDK is installed, since UE 5.8's tools need it. RiderLink is installed in the engine, not in the project, so it stays out of the repository.
- The editor is launched from Rider with Run, without the debugger. Debug with logs (`UE_LOGFMT`); do not suggest the debugger.
- Live Coding handles small changes; reinstancing and automatic compilation of new classes are off. Structural changes (new classes, new or changed `UPROPERTY`/`UFUNCTION`, other reflection changes) need the editor closed and a rebuild from Rider. Changed defaults in constructors need the same, because Live Coding does not rebuild class default objects. Flag a structural suggestion in a few words, without repeating these steps, and group structural changes when possible.
- The author creates new source files in Rider, in the right folder: always state the exact folder.
- Git through TortoiseGit, committing directly to `main`. Keep git simple: no branch workflows. `.gitignore` in place, no Git LFS (the repository stays minimal).
- An IDE's formatter may move `public:` and `private:` back to the class's indentation. When reviewing pasted code, point out this lost indentation.

## Game design

- In the style of logic course exercises: the player simplifies "complex" boolean expressions.
- Each symbol of the expression is a block in a row. Proposition blocks hold a sentence (e.g. "John eats tofu") and a letter, given in order of first appearance (same sentence, same letter); each can be minimized (letter) or maximized (whole sentence). Operator blocks: the constants ⊤ and ⊥, ¬, ∧, ∨, ⊕, NAND (↑), NOR (↓), →, ← and ↔, and the parentheses.
- A level has three texts: a sentence in natural language shown to the player, the same sentence written as a formula (the starting formula) and the solution, also as a formula. In these formulas, propositions go in brackets (`[John eats tofu]`) and operators are symbols or ASCII aliases (`&`, `~`, `->`; `|` is OR, so NAND needs another alias). Levels will live in a DataTable imported from CSV; more columns (difficulty, move or time limits) may come later.
- The player moves, adds and removes blocks freely. The game does almost nothing for the player (a choice for simplicity, open to revision).
- Two indicators:
  - Validity: is the sequence a well-formed formula?
  - Equivalence with the level's starting expression: cyan if equivalent, red if not, gray when the expression is invalid.
- Visuals: simple 2D made with basic 3D. Top-down orthographic camera; blocks are meshes, not textures: capsules, or circles when the text is short.

## Project layout

```
README.md            project overview (GitHub page)
LICENSE              MIT
Config/
  DefaultGame.ini    values of UThisIsLogicSettings (asset paths)
Source/ThisIsLogic/
  BasicComponents/   CameraPawn.h, ThisIsLogicGameMode.h, ThisIsLogicPlayerController.h, ThisIsLogicSettings.h
  Gameplay/Block/    Block.h, PropositionBlock.h, OperatorBlock.h, BlockFactory.h
  Gameplay/Exercise/ Exercise.h
  Gameplay/Formula/  Formula.h
  Input/             PointerTarget.h, BackgroundTarget.h
  Logic/             LogicTypes.h, FormulaStub.h (logic core, no engine dependencies)
  Visuals/           RoundedBackgroundComponent.h
Content/             folders by feature
  Levels/            Main (startup map)
  Gameplay/Block/    M_Block (Unlit, Color parameter)
  Input/             IA_Click, IMC_Gameplay
  Core/              BP_ThisIsLogicGameMode (default GameMode), BP_ThisIsLogicPlayerController
```

`ThisIsLogic.Build.cs` depends on `EnhancedInput`, `ProceduralMeshComponent` and `DeveloperSettings`.

Classes (summary only; read the code for details):

- `ACameraPawn`: top-down orthographic camera, set up in `BeginPlay`.
- `AThisIsLogicGameMode`: uses `ACameraPawn` as the default pawn.
- `AThisIsLogicPlayerController`: adds the mapping context and binds `IA_Click` (Started, Triggered, Completed). Tells a tap from a drag: a press released before the cursor moves more than `DragThreshold` (10 pixels on screen) from where it was pressed is a tap; moving past it starts a drag. Sends the gesture through `IPointerTarget` to whatever was under the cursor at the press; knows no gameplay classes (see Input design).
- `IPointerTarget`: C++-only interface with `Tapped`, `DragStarted` (with the press point), `Dragged` (every frame during a drag) and `DragEnded`. A tap sends only `Tapped`; a drag sends `DragStarted`, then `Dragged` each frame, then `DragEnded`. Points are in world space, on the horizontal plane through the press point.
- `ABackgroundTarget`: an invisible box (`Area`), much larger than the view, that blocks only the `Visibility` channel; its owner places it below the blocks, so a press that hits no block hits it. Implements `IPointerTarget`: a tap notifies the owner (`SetTappedListener`); drags do nothing.
- `ABlock`: abstract. Its root component is a `URoundedBackgroundComponent` (`Background`), fully round (a capsule, or a circle when `Width` equals `Height`); its text is shown by a text render component (`Label`). It stores no text of its own: each subclass says which text to show by overriding `GetText` (Template Method). `OnConstruction` gives the background the block material from `UThisIsLogicSettings`, builds it once with a provisional size (the default `Width`) and calls the private `ApplyText`, which puts the text from `GetText` on the `Label`, computes `Width` (the text plus `TextMargin` on each side, never less than `Height`) and resizes the background. Subclasses call the protected `FitToText` when their text changes: it calls `ApplyText` and then notifies the owner that the width changed. Subclasses choose their color through the protected `SetColor`. The selection is shown by `Outline`, a second rounded background, yellow and `OutlineThickness` larger on each side, just below the block and without collision; it is built and resized along with the background, and `SetSelected`, called by the owner, shows or hides it. Implements `IPointerTarget`: a tap or the start of a drag asks its owner to select it; during a drag it keeps its grab offset and asks its owner to move it, and at the end it asks to be dropped. Owner events: width changed (`SetWidthChangedListener`), move requested (`SetMoveRequestedListener`), drop requested (`SetDropRequestedListener`) and select requested (`SetSelectRequestedListener`).
- `APropositionBlock`: sets its color in its constructor and keeps its sentence (`SetSentence`, called by the factory), a letter (a stub, always "P") and whether it is minimized. `GetText` returns the letter when minimized and the sentence otherwise; the private `ToggleMinimized` switches and calls `FitToText`. Temporary trigger: it overrides `Tapped` and toggles after the base asks for the selection.
- `AOperatorBlock`: sets its color in its constructor and keeps its symbol (`SetSymbol`, called by the factory; for now the token's text), which `GetText` returns.
- `URoundedBackgroundComponent`: a `UProceduralMeshComponent` that draws a rounded rectangle in one color (dynamic material instance with a `Color` parameter), usable by any actor. Its owner calls `SetBaseMaterial`, `SetColor` and `SetRoundness`, which only store values, before `Build(Width, Height)`, which creates the mesh and the material; `Resize(Width, Height)` moves the vertices and recreates the mesh section, which also rebuilds the collision. `Roundness` (0 to 1, hidden from the editor) sets the corner radius as a fraction of half the smaller side: 0 is a plain rectangle, 1 a capsule, or a circle when both sides are equal. Editable: `Color` and `CornerSegments` (edges that cut each corner; 0 is a plain rectangle). See Background design.
- `AFormula`: owns its blocks and lays them out as a row (see Layout design). Its owner gives it the blocks through `AddBlock` or `AddBlocks`, which register each one (`RegisterBlock` sets the owner and listens to the block's events) and then update the row (`UpdateRow`). On a move request it keeps only Y, clamps it to the row, reorders the row and lifts the block above it; on a drop request it lays the row out again, which puts the block into its slot; on a width change it updates the row. Keeps the selected block (`SelectedBlock`): a select request calls `SelectBlock`, which unmarks the previous block through the private `DeselectBlock` and marks the new one; its owner calls `ClearSelection` when the background is tapped. Both notify the owner that the selection changed (`SetSelectionChangedListener`), once and with no data (see Exercise design).
- `AExercise`: the exercise being played, placed in the `Main` map (see Exercise design). In `BeginPlay` it creates an `ABackgroundTarget` `BackgroundDepth` below itself and listens to its tap, which clears the formula's selection (`ClearSelection`); spawns an `AFormula` at its own location and listens to its selection changes (for now it only logs them); then gets tokens from `FFormulaStub`, creates one block per token through `FBlockFactory` and hands them to the formula with `AddBlocks`.
- `FBlockFactory`: a static class. `CreateBlock(World, Token)` sends a proposition token to `CreatePropositionBlock` and any other token (operators, constants and parentheses) to `CreateOperatorBlock`. Each private creation method spawns its class from C++ with a deferred spawn and sets the block's data from the token's text (`SetSentence` or `SetSymbol`) before `FinishSpawning`, so `OnConstruction` already has it.
- `UThisIsLogicSettings`: a `UDeveloperSettings` shown in Project Settings, with its values in `Config/DefaultGame.ini`; holds soft references to assets. Read through `GetDefault<UThisIsLogicSettings>()`; `GetBlockMaterial` loads the block material and `check`s it (see Data design).
- `FFormulaStub`: plain C++; returns the tokens of a fixed example formula, in place of the level data and the analyzer. Temporary: its formula changes freely, and it may be deleted or reused in a test.
- `ETokenKind` and `FToken` (`LogicTypes.h`): plain C++, no reflection. A token is a kind and its text. One kind per concept; notation variants belong to the analyzer.

## Input design

- The player controller is a thin router (Mediator). It tells a tap from a drag and speaks only in those terms (tapped; drag started, dragged, ended); each element decides what they mean (select, move, insert, ...). The rule is the same for every element, and its distance is measured in screen pixels, which only the controller knows, as in Unity's EventSystem.
- It remembers which element was under the cursor at the press (capture), so the whole gesture reaches it even after the cursor has left it.
- Devices, keys and combinations (the left mouse button, a touch, a keyboard shortcut) are Enhanced Input data: Input Actions, Mapping Contexts and Triggers. Add one Input Action per intention, not code per device.
- Input that points (mouse) goes to the element under the cursor. Input that does not point (keyboard) should go to a current selection or to the game, when that is needed.
- Empty space is an element too: `ABackgroundTarget` lies below everything, so every press has a target and the controller has no special case for a press that hits nothing.
- Rejected: engine click events (no capture, no drag event) and `EnableInput` on each actor (every actor would check every click); telling a tap from a drag in each element (the same rule repeated, and only the controller knows screen positions); handling a press that hits nothing in the controller (the selection would clear on the press rather than on a tap, and gameplay would have to find the controller to listen to it); the formula as the background (it would fill the screen, mixing roles and clashing with the formula's area for drops).

## Command design

- Every command works with a single pointer: press, move and release, with one finger or the left mouse button. No hover, right click, modifier keys, wheel, multi-finger or timed gestures (double tap, long press), so the same commands serve mouse and touch and carry over to another engine. Keyboard shortcuts may come later as desktop extras, never as the only way to a command. The one planned exception is a long press on a proposition, a shortcut for Minimize/Maximize once the panel exists; like keyboard shortcuts, it is an extra, never the only way to the command.
- A press released before the pointer moves past a small distance is a tap; moving past it starts a drag. A tap on a block selects it, and so does starting to drag it: the block the player works with is the selected one. A tap on the background clears the selection.
- Besides the formula, the screen has a palette, always visible, that is the source of every new block (the operators, the level's propositions and New letter, which creates the next free letter), and a panel whose buttons change with the selection (Remove and Minimize/Maximize for a proposition; Minimize all and Maximize all with nothing selected). Undo and Restart are always visible. Every command has a button or a palette item; dragging moves blocks and is a shortcut for inserting and removing.
- Inserting: a tap on a palette item inserts the block after the selected one, or at the end of the row with nothing selected; dragging a palette item inserts it where it is dropped.
- Removing: the Remove button, or dropping a block on the palette, which is highlighted while the block is over it. Dropping inside the formula's area moves the block (to any of its rows, if it has several); dropping anywhere else puts it back.
- No confirmations: every command can be undone. Undo restores a copy of the state saved before each command and rebuilds the blocks from it (Memento pattern).
- Each command exists once, as a method of the class that owns the data; buttons and drags only call it, so undo, logs and tests see a single path.
- Planned, once the tree exists: tapping the selected block again widens the selection to the smallest sub-formula around it, then to the next one out. A selected sub-formula can become a new proposition (Define as proposition): it takes the next free letter, the legend shows its definition, and the indicators expand it before the truth table.
- Laws: first a legend of the laws, for reference only; later, only if play calls for it, the panel lists the laws that match the selected sub-formula, each with its result. A tap on the formula never applies a law directly.
- In Unreal, the palette and the panel are planned as actors in the world, like the blocks, implementing `IPointerTarget`, which reuses the input routing; UMG stays for menus and the HUD. Rejected for the palette and the panel: UMG (dragging from it into the world mixes two input systems in one gesture; without Blueprints it cannot use `BindWidget`; Slate's declarative syntax is method chaining).
- Rejected: icons or a context menu on the block (hidden commands, too small for a finger without covering the neighbors, and a context menu must repeat the main interface anyway); radial menus (no engine support, they compete with dragging on touch, and they pay off only with heavy repeated use); removing by dropping anywhere outside the row (accidental removals, as in the macOS Dock, and ambiguous with several rows); insert options only after a tap on empty space (blocks touch, so there is no empty space between them); a "+" button between each pair of blocks (many tiny targets); a Duplicate command (the palette already holds every operator and the level's propositions); confirmation dialogs (people confirm by habit); laws built into gestures, as in DragonBox (special cases, a tutorial for each gesture, and a conflict with reordering); a quick tap to minimize or maximize and a long press to select (selection underlies most commands, and a long press is hidden, slower and harder for some players).

## Exercise design

- `AExercise` is the top of the gameplay classes: it creates and owns the background target and the formula, and drives the creation of the blocks (see Block creation design). Later it will create the formula from the level data, own the panel and the palette, and decide the result of the exercise. The selection stays in `AFormula`, which owns the blocks (and later the tree); the exercise learns about its changes through an event.
- The selection event carries no data: whoever needs the selection asks the formula, so no copy of it has to be kept in sync. Each change sends one notification, once the state is final (`SelectBlock` unmarks the previous block through `DeselectBlock`, which does not notify). It may also come when nothing changed (tapping the selected block, or the background with nothing selected), which is harmless, since listeners only ask the formula.
- The name comes from the exercises of a logic course, which the game imitates: each has a statement, a formula to simplify, tools to work on it and a result.
- Rejected names: `AAdministrator` and `ATeacher` (role names, like `Manager`, fit any task and attract unrelated ones; `Administrator` also reads as system administration); `APuzzle` (generic).
- Rejected: the selection event carrying the selected block (the exercise would keep a copy of the selection); `SelectBlock` calling a notifying `ClearSelection` (switching blocks would send two notifications, the first with nothing selected).

## Layout design

- `Blocks` is the only source of the order. Positions are always derived from it by `LayoutBlocks` (left to right from the row's left edge, adding widths), never adjusted one block at a time. The row is centered on the formula; its width is the sum of the block widths (`TotalWidth`, kept in sync).
- Blocks enter the row through `AddBlock` (one) or `AddBlocks` (several at once, such as the initial ones). Both register each block (`RegisterBlock`) and then call `UpdateRow` once, which updates `TotalWidth` and lays the row out; a width change also calls `UpdateRow`. Every public method leaves the row laid out.
- While dragging, the block swaps with a neighbor when its edge facing that neighbor passes the neighbor's center (it covers half of it). Only the neighbors are checked each frame; the row is laid out again after each swap, and the dragged block is then put back under the cursor in the same frame.
- Rejected: attaching each block to its left neighbor (dragging would carry the blocks on its right, and the order would live in two places); swapping when the center passes the neighbor's center (a wide block cannot pass a narrow one at the end of the row) or the neighbor's edge (blocks of different widths swap back and forth every frame); a public `AddBlock` that leaves the layout to its caller (called alone, it leaves the block out of place); only `AddBlock`, called once per block (the initial blocks arrive together); `AddBlock` calling `AddBlocks` with a one-block list (harder to read than two methods that share private ones).

## Block creation design

- Data flow: level texts (DataTable) → analyzer → tokens → factory → blocks → formula, driven by `AExercise`. For now `FFormulaStub` stands in for the level data and the analyzer and returns the same tokens the analyzer will.
- The factory is the only place that knows which block class to create (Simple Factory); `AFormula` knows only `ABlock`. Actors cannot take constructor parameters (the engine calls the default constructor, also for the class default object), so the factory uses a deferred spawn to fill in a block before `OnConstruction`.
- `ABlock` is abstract and asks each subclass for its text (`GetText`): the subclass decides which text and when, and the base knows how to show it (label, width, background and owner notification). `APropositionBlock` keeps its sentence, its letter and whether it is minimized; `AOperatorBlock` keeps its symbol (planned: its operator kind, with the symbol derived from it). Letters will come from a small level-wide table (sentence → letter), which can also feed a legend like the "Define:" lists of textbook exercises; until then the letter is a stub.
- Shared types are grouped by domain (`Logic/LogicTypes.h`), not by kind of type.
- Rejected: parentheses as proposition markers (they also group); splitting the formula text only at spaces (propositions contain spaces); global `Structs`/`Enumerators` files in a `SharedData` folder (unrelated types side by side, included everywhere); a factory holding block classes set in the Details panel (Blueprint subclasses that only held the material; see Data design); text handling moved down to the subclasses (it would open `Label` and `Background` to them, or duplicate them in each); the base keeping the text, with one method to store it before construction and another to show it later (two paths for the same text); a `SetText` overridden in `APropositionBlock` (it was not virtual, and the name would lie, since the block may show the letter); one appearance method shared by `OnConstruction` and the subclasses that also notifies the owner (during construction the block has no owner yet).

## Background design

- The mesh is a triangle fan: vertex 0 is the center, vertices 1 to 4(n+1) are the outline, and the triangles are (0, i, i+1) plus the closing (0, last, 1). This works because the shape is convex.
- The outline goes bottom-left, bottom-right, top-right, top-left: counter-clockwise on screen, where +X points up and +Y right. Only that order faces the camera (the material is single-sided); the reverse is culled. It looks clockwise only when X and Y are drawn as on paper.
- Each corner has n+1 vertices and every edge turns 90/(n+1) degrees. The vertices sit at half steps around the corner's arc center (r inward from the rectangle corner), at r / cos(half step) from it, so the edges are tangent to the circle of radius r. With n = 0 this is the exact rectangle, and the straight sides always lie on the rectangle's edges, so the size never changes.
- The radius r is `Roundness` times half the smaller side, so it depends on the size: `Build` and `Resize` both recompute the offset of each outline vertex from its rectangle corner (it depends on n and r). `Build` creates the vertex array and the material once; `Resize` places the four corners, adds the offsets in place and recreates the mesh section (`BuildMesh`, with `CreateMeshSection_LinearColor`), which also rebuilds the collision used by the click. The vertex array keeps its size: changing n needs `Build`.
- `check`s: n ≥ 0 (also `ClampMin` in the Details panel) and 0 ≤ `Roundness` ≤ 1, which keeps r within half the smaller side.
- Rejected: an actor for the block's background (the click would find the background, not the block); rounding drawn by the material (collision stays rectangular, and it needs UVs); vertices on the circle at the tangent points (no n = 0, duplicate vertices in capsules); one corner list mirrored for the others (mirroring reverses the order, rotation keeps it); scaling the component to resize (stretches the corners and the label); a fixed radius (it had to be kept at half the block's `Height` by hand); computing the radius when it is set (the size only arrives in `Build` and `Resize`); `UpdateMeshSection_LinearColor` in `Resize` (in UE5 it updates only the drawing, and the collision keeps the old shape); a separate collision shape on the block, such as a box or a capsule (one more component, an approximate shape, and the background would no longer guarantee that its collision matches its drawing).

## Data design

- No Blueprints, not even as data; the remaining ones are removed as their data moves to code (see Pending). Values that tune a class (numbers, colors, texts) are defaults in its C++ constructor; an owner configures its components through setters, as `ABlock` does with `Label` and `Background`. Fields that only the owner sets stay out of the editor: plain fields, or `UPROPERTY()` for pointers to engine objects, which the garbage collector must see.
- Asset references (materials, meshes, input assets, tables) live in `UThisIsLogicSettings`: soft references whose paths are saved in `Config/DefaultGame.ini`, editable with the editor closed (or in Project Settings). Classes read them through `GetDefault<UThisIsLogicSettings>()`, outside constructors, since loading an asset in a constructor is risky; one getter per asset loads it and `check`s it.
- Gameplay classes are spawned from their C++ classes (`StaticClass()`), never from Blueprint subclasses.
- Moving or renaming an asset means updating its path in `DefaultGame.ini` by hand; Copy Reference in the Content Browser gives the path, between the single quotes.
- Rejected: Blueprint subclasses as data (edited only in the editor, their saved values silently win over the code, and each class needs its own asset); asset paths written in C++ (unusual in Unreal, and every change needs a rebuild); a central Data Asset (edited only with the editor open, and still found through a path); a hand-made singleton holding engine objects (the garbage collector does not see it).

## Roadmap

Items are numbered in order. A new item takes its place in the sequence, and the items after it are renumbered. `[x]` done, `[~]` in progress, `[ ]` to do.

**1 Blocks**
- [x] 1.1 Drag and drop blocks with the mouse: pick the block under the cursor, move it while the button is held, drop it on release.
- [x] 1.2 Reorganize the input (see Input design).
- [x] 1.3 Lay out the blocks as a row and reorder them by dragging (see Layout design).
- [x] 1.4 Show a text on each block; the width comes from the text.
- [x] 1.5 Create the formula's blocks from tokens, through a factory and a stub (see Block creation design).
- [x] 1.6 Rounded block corners: capsules and circles (see Background design).
- [x] 1.7 Split `ABlock` into an abstract class with `APropositionBlock` and `AOperatorBlock`; the factory chooses by token kind.
- [x] 1.8 Minimize and maximize proposition blocks: the block switches between sentence and letter, and the row follows (temporary trigger: every release toggles; letter stub: "P").
- [x] 1.9 Select a block with a tap or a drag, telling a tap from a drag; a tap on the background clears the selection (see Command design).
- [x] 1.10 `AExercise`, a class above `AFormula` that creates the background target and the formula's blocks, taking those tasks out of `AFormula` (see Exercise design).
- [~] 1.11 The panel, with buttons that follow the selection: Minimize/Maximize for a proposition, Minimize all and Maximize all with nothing selected. It replaces the temporary trigger.
- [ ] 1.12 A long press on a proposition minimizes or maximizes it, as a shortcut for the panel button (see Command design).
- [ ] 1.13 Letters from a level-wide table (sentence → letter, in order of first appearance), replacing the stub "P".
- [ ] 1.14 Levels in a DataTable imported from CSV, starting with stubs.
- [ ] 1.15 The analyzer: text → tokens, plain C++, checked with logs. First piece of the logic core.
- [ ] 1.16 Arrange the analyzer and the formula so that neither takes on unrelated tasks.
- [ ] 1.17 Decide whether precomputed levels are worth saving.
- [ ] 1.18 Remove blocks, with Undo and Restart.
- [ ] 1.19 The palette: a tap inserts an operator, one of the level's propositions or a new letter after the selected block, or at the end of the row.
- [ ] 1.20 Drag from the palette to insert, and drop a block on the palette to remove it.
- [ ] 1.21 Equivalence indicator (cyan / red / gray), with a stub.
- [ ] 1.22 Validity indicator, with a stub.

**2 Logic core (plain C++)**
- [ ] 2.1 Represent boolean expressions as a tree: propositions, constants and the operators of `ETokenKind`.
- [ ] 2.2 Parse the tokens into a tree, with precedence and parentheses; this also decides validity.
- [ ] 2.3 Evaluate expressions and check equivalence with a truth table.
- [ ] 2.4 Measure the size of an expression (basis for goals and scoring).
- [ ] 2.5 Compute the minimal form of an expression (goal of each level). Cost under review: the solution in the level data may be enough.
- [ ] 2.6 Automated tests for the core.

**3 Game rules**
- [ ] 3.1 Win condition.
- [ ] 3.2 Select a sub-formula: tapping the selected block again widens the selection.
- [ ] 3.3 Define a selected sub-formula as a new proposition.
- [ ] 3.4 A legend of the laws (De Morgan and similar), for reference.
- [ ] 3.5 Progression between levels.

**4 Interface and polish**
- [ ] 4.1 Menu, level select and HUD (in C++).
- [ ] 4.2 Visual feedback (simple animations).
- [ ] 4.3 Save progress.

**Ideas, not scheduled**
- A mode to prove equivalence.
- A mode like textbook exercises: pick the formula that matches a sentence.
- A formula in several rows, if long propositions call for it (decide by feel).
- Keyboard shortcuts on desktop, as extras (see Command design).
- The panel lists the laws that match the selected sub-formula, each with its result.
- Recognize the law the player has just applied by hand ("De Morgan").
- Laws as molds to fit blocks into. High cost: blocks inside blocks, against `Blocks` as the only source of the order.

## Pending

- Remove the engine dependencies of the logic core: `LogicTypes.h` and `FormulaStub.h` include `CoreMinimal.h` (`FString`, `TArray`, `check`). Acceptable while the stub stands in for the analyzer. Tokens reach gameplay code (`FBlockFactory`), so their texts will need a conversion at that border; details to be decided with the analyzer.
- Remove the remaining Blueprints (see Data design): `BP_ThisIsLogicPlayerController` only holds `IMC_Gameplay` and `IA_Click`, which can move to `UThisIsLogicSettings`; `BP_ThisIsLogicGameMode` can then go too if it only points to that controller (the C++ GameMode would set the controller class, and the project's default GameMode would be the C++ class).
