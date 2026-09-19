# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios

lineindentf()
{
    for textf ; do
        indent -bad -bap -br -ce -cdw -npcs -ncs -nbc -npsl -brs -blf -ut -l100 -ngts -fca -fnc -nlps -ncdw -i4 -ts4 -nlp -pal $textf
        rm $textf~
    done
}

lineindent()
{
    lineindentf $1/*.c
    lineindentf $1/*.h
}

lineindent i386
lineindent x86
# lineindent x86_64 # No C or H files in x86_64
lineindent riscv64
lineindent kernel
lineindentf tools/*.c
