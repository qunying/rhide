/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
F("-a")
F("-b")
F("-B")
F("-Ca")
F("-Ce")
F("-Ch%d")
F("-Ci")
F("-Co")
F("-Cr")
F("-d%s")
F("-Dd%d")
F("-Do")
F("-Dw")
F("-e%s")
F("-g")
F("-Fe")
F("-Fg")
F("-Fr")
F("-L%s(DE)")
/*         write_line('  -Ixxx  adds xxx to include path'); */
F("-I%s")
/*         write_line('  -n     don''t read the default config file'); */
F("-n")
/*         write_line('  -Oxxx  adds xxx to object path'); */
F("-O%s")
/*         write_line('  -oxxx  change the name of the executable produced to xxx');*/
F("-o%s")
/*         write_line('  -q-    write information when compiling  (obsolete, see -v)'); */
/*         write_line('  -S     syntax options'); */
/*         write_line('             -S2 switch some Delphi 2 extension on');*/
F("-S2")
/*         write_line('             -Sa semantic check of expressions');
         write_line('                 a higher level includes the lower');
         write_line('                 -Sa0 only ANSI pascal expressions are allowed');
         write_line('                 -Sa1 functions results havn''t to be assigned to variables');
         write_line('                 -Sa2 @-operator returns typed pointer');
         write_line('                 -Sa4 assigment results are typed (allows a:=b:=0)');
         write_line('                 -Sa9 allows expressions with no side effect');*/
F("-S%s(01249)")
/*         write_line('             -Sc supports operators like C (*=,+=,/= and -=)');*/
F("-Sc")
/*         write_line('             -Sg allows LABEL and GOTO');*/
F("-Sg")
/*         write_line('             -Si support C++ stlyed INLINE');*/
F("-Si")
/*         write_line('             -Sm support macros like C (global)');*/
F("-Sm")
/*         write_line('             -So tries to be TP/BP 7.0 compatible');*/
F("-So")
/*         write_line('             -Ss the name of constructors must be init');*/
/*         write_line('                 the name of destructors must be done');*/
F("-Ss")
/*         write_line('             -St allows static keyword in objects');*/
F("-St")
/*         write_line('  -s     don''t call assembler and linker (only with -a)');*/
F("-s")
/*         write_line('  -T     target operating system');*/
/*         write_line('             -TDOS DOS extender by DJ Delorie');
         write_line('             -TOS2 OS/2 2.x');
         write_line('             -TLINUX Linux');
         write_line('          !  -TWin32 Windows 32 Bit');
         write_line('             -TGO32V2 version 2 of DJ Delorie DOS extender'); */
F("-T%s")
/*         write_line('  -uxxx  undefines the symbol xxx'); */
F("-u%s")
/*         write_line('  -U     unit options');
         write_line('             -Un don''t check the unit name');*/
/*         write_line('             -Us compile a system unit');*/
F("-Us")
/*         write_line('             -Upxxxx adds xxxx to the unit path');*/
F("-Up%s")
/*         write_line('  -vxxx  Be verbose. xxx is a combination of the following letters :');*/
/*         write_line('          e : Show errors (default)       i : Show general info');
         write_line('          w : Show warnings               l : Show linenumbers');
         write_line('          u : Show used files             t : Show tried files');
         write_line('          p : Show compiled procedures    c : Show conditionals');
         write_line('          d : Show debug info             m : Show defined macros');*/
F("-v%s(cdeilmptuw)")
/*         write_line('  -w-    turns warnings off (Obsolete, see -v) ');*/

/*{$ifdef linux}
         write_line('  -X     executable options (LINUX)');*/
/*         write_line('             -Xc link with the c library');*/
F("-Xc")
/*         write_line('             -Xe create ELF executable');*/
F("-Xe")
/*{$endif}*/

/*         write_line('  -A     output format');
         write_line('             -Aatt  AT&T assembler');
         write_line('             -Ao    coff file using GNU AS');
         write_line('             -Aobj  OMF file using nasm');
         write_line('             -Anasm coff  file using NASM');
         write_line('             -Awasm assembler for the Watcom assembler');*/
F("-A%s")
/*         write_line('  -R     assembler reading style');
         write_line('             -Ratt  read AT&T style assembler');
         write_line('             -Rintel read Intel style assembler');
         write_line('             -Rdirect copy assembler text directly to assembler file'); */
F("-R%s")
/*         write_line('  -O     optimizations');
         write_line('             -Oa simple optimizations');
         write_line('             -Og optimize for size');
         write_line('             -OG optimize for time');
         write_line('             -Ox optimize maximum');
         write_line('             -O3 optimize for i386');
         write_line('             -O4 optimize for i486');
         write_line('             -O5 optimize for Pentium (tm)');
         write_line('             -O6 optimizations for PentiumPro (tm)'); */
F("-O%s(agGx3456)")

