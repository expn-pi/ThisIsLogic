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
  never claim that code compiles.
- Names and structure change often: re-read the current code instead of trusting earlier
  names, including the ones in this file.
- One topic per conversation. When asked what changes in this file, reply with only the
  changed parts.

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
  reflection changes) need the editor closed and a rebuild with Ctrl+F5. Say so whenever a
  suggestion is structural, and group structural changes when possible.
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
- `AThisIsLogicPlayerController`: adds the mapping context, binds `IA_Click`
  (Started, Triggered, Completed) and currently drives the drag.
- `ABlock`: procedural rectangle mesh (`Width`, `Height`), color through a dynamic material
  instance, width-changed event (`AddWidthChangedListener`).
- `AFormula`: owns its blocks (`AddBlock` sets the owner and subscribes to their events)
  and clamps their movement to its `Length` (`MoveBlock`). Relayout on width change is a stub.

## Open design discussion: input and drag

Today the player controller finds the block under the cursor, keeps the grabbed block and
drives the drag, asking the block's formula to move it. It knows game rules that are not its
own (high coupling, low cohesion), so it will be reorganized.

The author's current direction (not final):

- Each interactive element finds out by itself that it was clicked; no central dispatcher.
- The block tells its formula, through a callback, where it wants to go. The formula applies
  its limits and places the block where it can be.
- `AFormula` keeps an auxiliary structure that summarizes the layout of its blocks. It updates
  it from block events (move, width change) and repositions and notifies the affected blocks.

Options considered so far:

- The controller only translates input and passes world points through a drag `UINTERFACE`.
- Engine click events (`bEnableClickEvents`): the release only reaches the actor if the
  cursor is still over it.
- `EnableInput` on each actor: every enabled actor receives every click and must check
  whether it was the target.

## Roadmap

Numbers follow the author's task list. `[x]` done, `[~]` in progress, `[ ]` to do.

**3 Blocks**
- [x] 3.2 Drag and drop blocks with the mouse: pick the block under the cursor, move it
  while the button is held, drop it on release.
- [~] Reorganize the input (see the open design discussion).
- [ ] 3.3 Add and remove blocks.
- [ ] 3.4 Lay out an expression as a row of blocks.
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