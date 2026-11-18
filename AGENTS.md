# Agent instructions

## Git branches

- Work on the **currently checked-out branch**. Do not create a new branch for a task.
- Do not run `git checkout -b`, `git switch -c`, or `git worktree add -b` unless the user explicitly asks for a new branch.
- If the work belongs on another branch, stop and tell the user. They will switch branches.
- Do not rewrite history or force-push unless the user explicitly asks.

## Git commit

- Do **not** run `git commit`.
- Leave changes in the working tree and tell the user what changed. The user commits.
- Do not stage/`git add` files for the user either, unless they ask.

## Git push

- Do not `git push` (or push to origin/upstream). The user pushes themselves.
