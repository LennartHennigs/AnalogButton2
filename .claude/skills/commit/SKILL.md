---
name: commit
description: Stage and commit current changes for AnalogButtons — checks for needed CHANGELOG/README/CLAUDE.md updates, writes a commit message, and commits
---

Commit the current working changes. Follow these steps:

## Step 1: Understand what changed

Run `git diff --stat` and `git status` to see what files are modified or untracked. Read the diffs for any changed source files (`src/`, `test/`, `examples/`) to understand the nature of the changes.

## Step 2: Check CHANGELOG

If any source files (`src/`, `test/`, `examples/`) changed, check whether `CHANGELOG.md`'s `## Unreleased` section already documents those changes. If not, add the missing entries following the established format:
- `**Fixed**: ...` for bug fixes
- `**Added**: ...` for new API or features
- `**Changed**: ...` for behavioral changes
- `**Internal**: ...` for refactors, test additions, infra with no user-visible effect

Skip changelog update for whitespace-only, comment, or docs-only changes.

## Step 3: Check README

If public API changed (new/removed/renamed functions in `src/AnalogButtons.h`), check whether `README.md` reflects the change. If not, update the relevant section. Skip for internal or test-only changes.

## Step 4: Check CLAUDE.md

If a non-obvious pattern, gotcha, or invariant was introduced that future sessions should know about, add a concise note to `CLAUDE.md`. Do not add obvious or one-off information.

## Step 5: Stage and commit

Stage all relevant files:
```bash
git add <changed source files> CHANGELOG.md  # plus README.md / CLAUDE.md if updated
```

Write a conventional commit message:
- `fix:` for bug fixes
- `feat:` for new features
- `test:` for test-only changes
- `docs:` for documentation only
- `chore:` for tooling, config, infra

Keep the subject line under 72 characters. Add a short body if the why isn't obvious from the subject.

Run `git commit -m "..."` and confirm success.
