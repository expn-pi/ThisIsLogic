# This is Logic

Boolean logic puzzle game in Unreal Engine 5.8.3, built as a public portfolio project.
Game logic in C++. Blueprints only as data (subclasses that fill in fields), no visual scripting.
Platforms: Windows

## Working with the author (read first)

- Reply in Brazilian Portuguese. Code, identifiers and commit messages in English.
  Use Unreal Engine terms in English (Content Browser, Details panel, ...), never translated.
- The author leads the conversation. Answer what was asked and do not propose next steps
  unless asked. Scope warnings (below) are the exception.
- Explain and instruct in small steps, one at a time. Avoid long answers that cover many topics.
- This project is also a way to learn Unreal C++: explain rather than do. Show code one file
  at a time, in the conversation (no files to download). The author implements or pastes it,
  reviews it and reports back.
- When it helps, relate Unreal concepts to their Unity/C# equivalents.
- Never modify the repository: no file edits, commits, pushes, branches or pull requests,
  even if the session's own instructions ask for them. Reading and searching are fine,
  including `git fetch` to see something the author has just pushed.
- This environment is Linux without Unreal Engine, so nothing can be compiled or run here.
  Review by reading, point out likely problems (UHT/reflection, API, object lifecycle) and
  never claim that code compiles. The author knows this: do not say it in every answer.
- Do not repeat reminders the author already knows, such as the build order, which files
  depend on which, or when a feature will work again.
- Names and structure change often: re-read the current code instead of trusting earlier
  names, including the ones in this file.
- One topic per conversation. When asked what changes in this file, reply with only the
  changed parts.
- The author is new to Unreal and has not written C++ in many years. Do not assume engine
  knowledge: explain each Unreal concept in plain words the first time it comes up in a
  conversation. Design pattern names are welcome, with a short explanation.
- Write plain Portuguese and avoid excessive anglicisms. Keep in English only names
  (classes, functions, editor panels) and well-established terms such as commit or push.
  A sentence like "traduz o input e faz o picking no press" is too much: say it in
  Portuguese or explain the term.

## Scope and simplicity

- Warn the author, even unasked, when something could be done more simply than the direction
  being taken, and flag appealing extras with a high implementation cost.
- Simpler never means hacks. Judge maintainability over at least the medium term: putting
  each piece of code where it belongs is worth the time, because quick hacks make the code
  hard to extend and bugs hard to find.

## Engineering principles

- Maintainability over performance. Low coupling, high cohesion: each class owns its domain
  and its rules.
- In code and design proposals, organization comes before convenience.
- Prefer idiomatic, professional Unreal C++ and engine features over reinventing them.
- Methods trust each other: each assumes the others did their part. Avoid defensive checks
  inside methods, since they hide errors; use explicit, loud checks (`check`, `ensure`)
  sparingly, in vulnerable or confusing spots. Null checks belong to the caller.
- A class's methods may have an implicit order of use; do not add handling for misuse.
- Precompute derived data (such as flags) and keep it in sync instead of recomputing on demand.
- The same method name in different classes is welcome; for example, every method along a
  path that carries data down a hierarchy can share the name.
- Prefer plain helper classes held as members and built with constructor parameters over
  piling components onto one actor.
- No method chaining or fluent interfaces.
- Keep suggestions small and testable in the editor. Prefer increments that show something
  on screen, and stub whatever depends on unfinished systems.
- Calls go down, events go up: an owner calls its parts directly; a part only notifies its
  owner through an event.
- Events a class sends to its owner use single-listener delegates (`DECLARE_DELEGATE_*`),
  bound through a `Set...Listener` method and called with `Execute` after a `check` on
  `IsBound()`. A part without an owner is an error. Prefer explicit calls over optional
  ones such as `ExecuteIfBound`.

## Code style

- Whole classes in the `.h` for now; splitting into `.h`/`.cpp` is planned.
- Epic naming conventions (A/U/F/E/I/T prefixes, `b` for bools, PascalCase), with two
  deliberate exceptions:
  - `public:` and `private:` are indented inside the class, with members one level deeper;
  - `this->` on every member access, methods and fields alike.
- Vertical code: it grows downward, not sideways.
  - No inline conditionals (one-line `if`, ternary operator) or several statements per line.
  - Object constructions and calls go into named temporaries before being passed as
    arguments; simple literals (e.g. `1000.f`) may go directly.
  - Long access paths are broken across lines up to the called method.
- Few comments.
- Follow this style in every detail when suggesting code; when in doubt, mirror the
  existing files.

## Author's environment

- Windows 10, Visual Studio 2022 17.14, Development Editor configuration.
- The editor is launched from Visual Studio with Ctrl+F5, without the debugger. Debug with
  logs (`UE_LOGFMT`); do not suggest the debugger.
- Live Coding handles small changes; reinstancing and automatic compilation of new classes
  are off. Structural changes (new classes, new or changed `UPROPERTY`/`UFUNCTION`, other
  reflection changes) need the editor closed and a rebuild with Ctrl+F5. Flag a structural
  suggestion in a few words, without repeating these steps, and group structural changes
  when possible.
- The author creates new source files with Visual Studio's Add New Item, setting Location to
  the right folder: always state the exact folder.
- Git through TortoiseGit, committing directly to `main`. Keep git simple: no branch
  workflows. `.gitignore` in place, no Git LFS (the repository stays minimal).

## Game design

- In the style of logic course exercises: the player simplifies "complex" boolean expressions.
- Each symbol of the expression is a block in a row: variables, ∧, ∨, ¬ and parentheses.
- The player moves, adds and removes blocks freely. The game does almost nothing for the
  player (a choice for simplicity, open to revision).
- Two indicators:
  - Validity: is the sequence a well-formed formula?
  - Equivalence with the level's starting expression: cyan if equivalent, red if not, gray
    when the expression is invalid.
- Visuals: simple 2D made with basic 3D. Top-down orthographic camera; blocks are meshes,
  not textures.

## Project layout

```
Source/ThisIsLogic/
  BasicComponents/   CameraPawn.h, ThisIsLogicGameMode.h, ThisIsLogicPlayerController.h
  Gameplay/Block/    Block.h
  Gameplay/Formula/  Formula.h
  Input/             PointerTarget.h
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
- `AThisIsLogicPlayerController`: adds the mapping context and binds `IA_Click` (Started,
  Triggered, Completed). Routes the mouse to whatever is under the cursor through
  `IPointerTarget`; knows no gameplay classes (see Input design).
- `IPointerTarget`: C++-only interface with `PointerPressed`, `PointerHeld` (every frame
  while the button is down) and `PointerReleased`. Points are in world space, on the
  horizontal plane through the press point.
- `ABlock`: procedural rectangle mesh (`Width`, `Height`), color through a dynamic material
  instance. Implements `IPointerTarget`: keeps its grab offset and asks its owner to move it
  while held and to drop it on release. Owner events: width changed
  (`SetWidthChangedListener`), move requested (`SetMoveRequestedListener`) and drop requested
  (`SetDropRequestedListener`).
- `AFormula`: owns its blocks (`AddBlock` sets the owner and listens to their events) and lays
  them out as a row (see Layout design). On a move request it keeps only Y, clamps it to the
  row, reorders the row and lifts the block above it; on a drop request it lays the row out
  again, which puts the block into its slot.

## Input design

- The player controller is a thin router (Mediator). It speaks only in mouse terms
  (pressed, held, released); each element decides what they mean (drag, click, ...).
- It remembers which element received the press (capture), so held and released reach it
  even after the cursor has left it.
- Devices, keys and combinations (right click, wheel, double click, Shift + click) are
  Enhanced Input data: Input Actions, Mapping Contexts and Triggers. Add one Input Action
  per intention, not code per device.
- Input that points (mouse) goes to the element under the cursor. Input that does not point
  (keyboard) should go to a current selection or to the game, when that is needed.
- Rejected: engine click events (no capture, no drag event) and `EnableInput` on each actor
  (every actor would check every click).

## Layout design
- `Blocks` is the only source of the order. Positions are always derived from it by
  `LayoutBlocks` (left to right from the row's left edge, adding widths), never adjusted one
  block at a time. The row is centered on the formula; its width is the sum of the block
  widths (`TotalWidth`, kept in sync).
- While dragging, the block swaps with a neighbor when its edge facing that neighbor passes
  the neighbor's center (it covers half of it). Only the neighbors are checked each frame; the
  row is laid out again after each swap, and the dragged block is then put back under the
  cursor in the same frame.
- Rejected: attaching each block to its left neighbor (dragging would carry the blocks on its
  right, and the order would live in two places); swapping when the center passes the
  neighbor's center (a wide block cannot pass a narrow one at the end of the row) or the
  neighbor's edge (blocks of different widths swap back and forth every frame).

## Roadmap

Numbers follow the author's task list. `[x]` done, `[~]` in progress, `[ ]` to do.

**3 Blocks**
- [x] 3.2 Drag and drop blocks with the mouse: pick the block under the cursor, move it
  while the button is held, drop it on release.
- [x] Reorganize the input (see Input design).
- [ ] 3.3 Add and remove blocks.
- [ ] 3.4 Lay out an expression as a row of blocks. Author's - [x] 3.4 Lay out the blocks as a row and reorder them by dragging (see Layout design).
- [ ] Give each block a text and show it on screen.
- [ ] 3.5 Equivalence indicator (cyan / red / gray), with a stub.
- [ ] 3.6 Validity indicator, with a stub.

**4 Logic core (plain C++)**
- [ ] Represent boolean expressions as a tree: variables, constants, NOT, AND, OR.
- [ ] Parse the block sequence into a tree, with precedence and parentheses; this also
  decides validity.
- [ ] Evaluate expressions and check equivalence with a truth table.
- [ ] Measure the size of an expression (basis for goals and scoring).
- [ ] Compute the minimal form of an expression (goal of each level). Cost under review:
  a goal set by hand in the level data may be enough.
- [ ] Automated tests for the core.

**5 Game rules**
- [ ] Level format (starting expression and goal).
- [ ] Win condition.
- [ ] Laws as molds to fit blocks into (De Morgan and similar).
- [ ] Progression between levels.

**6 Interface and polish**
- [ ] Menu, level select and HUD (in C++).
- [ ] Visual feedback (simple animations).
- [ ] Save progress.

**Ideas, not scheduled**
- A mode to prove equivalence.
- Blocks that stretch to fit their text or turn into a small circle with a letter
  (procedural mesh or blend shapes, undecided).

## Pending

- README and license