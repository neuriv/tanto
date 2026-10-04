# Repository migration

`neuriv/tanto` replaces `neuriv/tanto-engine`. Clone the new repository or update an existing checkout's origin:

```powershell
git remote set-url origin https://github.com/neuriv/tanto.git
```

The migration combines adjacent changes on dense days into varying daily totals. Sparse days retain their original granularity. Squashed commits retain the endpoint's exact source tree, author and dates; their bodies preserve original commit IDs, authors, dates and messages. Historic tags retain their exact source trees and annotations. Commit and tag IDs change because their ancestry changes. Existing checkouts with unpublished changes should keep their branches and transplant those changes onto a fresh clone.

A Git bundle preserves the complete original history, including intermediate snapshots and merged branches. The migration archive also contains the commit mapping, original issues/comments, pull-request metadata and release metadata. No timestamps were invented or shifted to another day.

All 31 issues were transferred through GitHub with their authors, content, comments and states. Issue references in historical commit messages and changelog sections refer to the former repository. References in the 0.5.0 notes refer to the new repository.

| Former issue | New issue |
| --- | --- |
| 1–21 | 1–21 |
| 24 | 22 |
| 25 | 23 |
| 26 | 24 |
| 27 | 25 |
| 28 | 26 |
| 29 | 27 |
| 30 | 28 |
| 31 | 29 |
| 32 | 30 |
| 33 | 31 |

Original entries 22 and 23 were pull requests; their metadata is archived rather than converted into issues. The existing `mwm/` source directory, application ID and saved-settings location remain compatible. The distributed editor and its logo are now named Tanto.
