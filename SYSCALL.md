# Syscalls
Remember, when adding syscalls, you need to modify:
- **[`syscall_ids.h`](include/LineKernel/syscall/syscall_ids.h)** (add your syscall number)
- **all `syscall.s` files** (change max syscall to yours + 1)
    Note: you can search across files for `REPLACEMAXSYSCALL` if you have trouble finding it.
- **[`syscall.c`](kernel/syscall.c)** (add your syscall function and add your syscall to `syscall_table`)

<!--
SPDX-License-Identifier: CC0-1.0
SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios
-->
