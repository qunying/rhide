/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifndef __IDE_H__
#define __IDE_H__

#include <tv.h>
#include <stdio.h>
#include "rhutils.h"

#define MODE_INPUT ios::in
#define MODE_OUTPUT ios::out
#define MODE_TRUNCATE ios::trunc
#define MODE_NOCREATE ios::nocreate
#define MODE_BINARY ios::bin

#ifndef PROJECT_EXT
#define PROJECT_EXT ".gpr"
#endif
#ifndef DESKTOP_EXT
#define DESKTOP_EXT ".gdt"
#endif
#ifndef STANDARD_OBJECT_SUFFIX
#define STANDARD_OBJECT_SUFFIX ".o"
#endif
#ifndef STANDARD_EXE_SUFFIX
#ifdef __DJGPP__
#define STANDARD_EXE_SUFFIX ".exe"
#else
#define STANDARD_EXE_SUFFIX ""
#endif
#endif

#ifdef __DJGPP__
#define RHIDE_OPTIONS_NAME "rh_opt"
#else
#define RHIDE_OPTIONS_NAME ".rh_opt"
#endif


#define VERY_OLD_PROJECT_IDENT "__gcc_project-file__"
#define OLD_PROJECT_IDENT "__RHIDE_project-file__\x1A"
#define PROJECT_IDENT "__RHIDE_project-file\x1A"
#define DESKTOP_IDENT "__RHIDE_desktop-file__\x1A"

#define Options (*(Project.options))

#define AINSERT(s) \
do {\
  char *__tmp__;\
  string_dup(__tmp__,(s));\
  args.insert(__tmp__);\
} while (0)

class TSCollection;

/* module idecheck.cc */
int CheckIDE();

/* module ideapp.cc */
void ShowCalculator();

/* module idemain.cc */
class IDE;
extern IDE *App;
extern int global_argc;
extern char **global_argv;
extern char *RHIDE_DIR;
extern char *RHIDE_NAME;
extern char *RHIDE_EXT;
extern int external_program_executed;
void Repaint();
class TProject;
void SetMainTargetName(const char *, TProject * _prj = NULL);
int SaveAll();
extern unsigned long hasmodified;
extern int c_words_changed;
extern int gpc_words_changed;
extern int fpc_words_changed;
extern int user_words_changed;
char *WUC();
void check_for_external_modifications();

/* module ideopt.cc */
class TDirList;
class TParamList;
class TDependency;
class TParamList;

void InitOptions();
void EditDirList(TDirList * dirlist, const char *title, int hist_id);
ushort EditParamList(TParamList * paramlist, const char *title, int hist_id);
void AddArg(const char *option);

void OptimizationFlags();
void DebugFlags();
void WarningFlags();
void CFlags();
void CXXFlags();
void PascalFlags();
void FortranFlags();
void AdaFlags();

void Libraries(void);
void AddLibraries(char *&);
void LinkerOptions(void);
void CompilerOptions(void);
void Preferences();

void EditReserved();
void EditGPCReserved();
void EditCFlags();
void EditCXXFlags();
void EditDebugFlags();
void EditWarnFlags();
void EditOptFlags();
void EditPascalFlags();
void EditUserWords();
void EditFortranFlags();
void EditAdaFlags();

/* module idegcc.cc */
long TimeOfFile(const char *fname, Boolean do_update,

                Boolean is_full_name = False);
Boolean Make(Boolean force_messages = True);
Boolean Build(void);
Boolean RunMainTarget(void);
class TDependency;
Boolean Compile(TDependency * _dep = NULL);
int RunProgram(const char *cmd,
               Boolean redir_stderr, Boolean redir_stdout,
               Boolean SwitchToUser = False);
void ClearFileHash();
void CheckStderr(bool erase = true);
void CheckStdout(bool erase = true);
void RemoveStdout(void);
void RemoveStderr(void);

/* module idecomp.cc */
class TDependency;
Boolean compile_dep(TDependency *);
extern int already_maked;
extern char *cpp_outname;
extern char *cpp_errname;

/* module ideprj.cc */
class TProject;
class TProjectWindow;
class TListBox;
Boolean OpenProject(const char *);
void SaveProject();
bool CloseProject();
void SaveOptions();
void LoadOptions();
void ShowProject(void);
void HideProjectWindow(void);
void AddProjectItem(void);
ccIndex AddProjectItem(const char *name);
void DelProjectItem(void);
extern TProjectWindow *project_window;
void LocalOptions();
void ShowIncludes();
Boolean PushProject(const char *directory, const char *prjname);
void PopProject(Boolean Save = True);
void ResetProjectStack();
void ClearDependencies();
void CreateDependencies();
void MakeClear();
void AddToStack();
int OpenFromStack();
void RemoveFromStack();
extern int PRJSTACKCOUNT;
extern char *project_directory;
class TCEditWindow;
void SetGlobalEditorOptions(TCEditWindow *);
void SetGlobalEditorOptions();
void SetProjectScreenMode();
void EnableShadows();
void DisableShadows();
class TFileDialog;
char *select_project(char *title);
TFileDialog *FileOpenDialog(char *init_val, char *title, char *input_label,
                            ushort aOptions, int hist, char *init_dir);
extern char *default_directory;

/* module ideutil.cc */
Boolean FindFile(const char *name, TDirList * list, char *&full_name,
                 Boolean uselist = True);
Boolean FindFile(const char *name, char *&rel_name, TDirList * list);
Boolean FindFile(const char *name, char *&full_name);
void ClearFindCache();
extern int debug_dependencies;
extern int debug_commands;
extern int debug_tempfiles;
extern int debug_files;
class ifpstream;
ifpstream *open_ifpstream(const char *name);
void close_ifpstream(ifpstream * is);
void ExpandFileNames(TProject * prj);

/* module idemsg.cc */
class TMsgCollection;
class TDialog;
TCEditWindow *is_on_desktop(const char *, Boolean is_full_name = False,
                            Boolean check_inode = True);
void ShowMessages(TMsgCollection * msgs, Boolean remove_old,
                  int select_first_err = -1, Boolean select_it = False);
void ClearMessages(int what = -1);

#define ClearMessage() ClearMessages(0)
extern TDialog *msg_window;
class TMsgListBox;
extern TMsgListBox *msg_list;
void show_message(const char *msg, const char *file,

                  int line, int column, int msg_type);

/* module idedefault.cc */
class TProject;
void DefaultOptFlags();
void DefaultDebugFlags();
void DefaultWarnFlags();
void DefaultCXXFlags();
void DefaultCFlags();
void DefaultPascalFlags();
void DefaultFpcFlags();
void DefaultReservedWords(TProject * p = NULL);
void DefaultGPCReservedWords(TProject * p = NULL);
void DefaultFPCReservedWords(TProject * p = NULL);
void DefaultUserWords(TProject * p = NULL);
void DefaultFortranFlags();
void DefaultAdaFlags();

/* module idecolors.cc */
void Colors();

/* module idehints.cc */
const char *IDEHint(unsigned short);

/*module ideformatline.cc */
class TCEditor;
void SyntaxFormatLine_C(TCEditor *, void *, uint32, int, ushort, uint32);
void SyntaxFormatLine_Pascal(TCEditor *, void *, uint32, int, ushort, uint32);

/* module IDEExtern.cc */
void FSDB();
void GREP();
void GDB();
void RunExternalProgram(const char *program, int show_stderr,

                        int show_stdout);

/* module IDEHelp.cc */
void Help();
void Help(ushort);
void HelpIndex();
void ContextHelp(ushort);
void SyntaxHelp(const char *, const char *fname = "libc.inf");
class TRect;
extern TRect InfRec;
class opstream;
class ipstream;
void SaveHelpDesktop(opstream &);
void LoadHelpDesktop(ipstream &);
void ClearHelpDesktop();
void LastHelp();

/* module IDEDebug.cc */
void STEP(int _switch_to_user = 1);
void TRACE(int _switch_to_user = 1);
void GOTO(int _switch_to_user = 1);
void RESET();
void FINISH();
void CONTINUE();
int DEBUGGER_STARTED();
void CLEARSYMBOLS();
void InitDebuggerInterface();
void ToggleBreak();
void OpenDisWin(int force_open = 0);

/* module idemak.cc */
class TProject;
TProject *ReadProject(const char *, bool from_ide = true);
void SaveProject(TProject * _project, const char *_project_name);
void WriteMake(char *outname = NULL, int argc = 0, char *arg[] = NULL,
               bool default_vars = false);
void put_breakline(FILE * f, int start_len, int max_len, const char *s);
extern TProject *project;

#define Project (*project)
extern char *project_directory;
extern char *project_name;
extern int recursive_make;
extern unsigned short ProjectVersion;

typedef struct
{
  TProject *project;
  char *pname;
  char *dname;
}
project_stack;

extern project_stack *PROJECT_STACK;
extern int stack_count;
Boolean _PushProject(const char *directory, const char *prjname);
class TDependency;
Boolean _PushProject(TDependency * dep);
void _PopProject();
class TStringCollection;
void _AbsToRelPath(char *&dname, TStringCollection * vars,

                   bool use_rel = true);
void check_vars(TStringCollection * vars, TDirList * dirs);

/* module idespec.cc */
class TDependency;
char *BuildCompiler(TDependency * dep, const char *spec);
char *GetCompilerSpec(TDependency * dep, Boolean & is_user);
void WriteSpecData(FILE *);
void dump_rhide_environment(FILE * f);
char *expand_rhide_spec(const char *spec);


/* module ideversi.cc */
extern char IDEVersion[];
extern char build_date[];
extern char build_time[];

/* idebug.cc */
extern char *create_bug_report(int);

/* openedit.cc */
void OpenFileFromEditor(char *name);
TCEditWindow *OpenEditor(const char *name, Boolean search);
void OpenEditor(const char *name, Boolean search, TCEditWindow * &ew);

/* idesynta.cc */
void SyntaxOptions();
void syntaxhelp(const char *, int create_index = 0);
void SyntaxIndex();

/* ideenv.cc */
void rhide_load_environment_file(char *appname, char *basename,
                                 int only_current = 0, int unload = 0);
void push_environment();
void pop_environment();

/* idercs.cc */
Boolean FindRCSFile(const char *name, char *&fname, char *&sname);
void CheckoutRCSFile(const char *sname, const char *fname, int rw = 0);

/* idedail.cc */
unsigned IDEdoEditDialog(int, ...);


#ifdef Uses_IDEConst
#include <libtvuti.h>
#include <ideconst.h>
#endif

typedef struct
{
  unsigned all_deps:1;
  unsigned create_backup_files:1;
  unsigned show_syntax:1;
  unsigned use_dual_display:1;
  unsigned autoindent:1;
  unsigned overwrite:1;
  unsigned show_stderr:1;
  unsigned show_stdout:1;
  unsigned show_doing:1;
  unsigned show_mem:1;
  unsigned no_file_cache:1;
  unsigned c_autoindent:1;
  unsigned use_tabs:1;
  unsigned no_stdinc:1;
  unsigned no_stdlib:1;
  unsigned for_profile:1;
}
ide_options;

#define IDE_OPTIONS (*((ide_options*)&(Options.debug_level)))
#define AllDeps (IDE_OPTIONS.all_deps)
#define CreateBackupFiles (IDE_OPTIONS.create_backup_files)
#define ShowSyntax (IDE_OPTIONS.show_syntax)
#define UseDualDisplay (IDE_OPTIONS.use_dual_display)
#define AutoIndent (IDE_OPTIONS.autoindent)
#define OverWrite (IDE_OPTIONS.overwrite)
#define ShowStderr (IDE_OPTIONS.show_stderr)
#define ShowStdout (IDE_OPTIONS.show_stdout)
#define ShowWhatDoing (IDE_OPTIONS.show_doing)
#define ShowMem (IDE_OPTIONS.show_mem)
#define NoFileCache (IDE_OPTIONS.no_file_cache)
#define CAutoIndent (IDE_OPTIONS.c_autoindent)
#define UseRealTabs (IDE_OPTIONS.use_tabs)

typedef struct
{
  unsigned no_persistent_blocks:1;
  unsigned column_cursor:1;
  unsigned row_cursor:1;
  unsigned intense_mode:1;
  unsigned verbose_gdb_commands:1;
  unsigned no_shadows:1;
  unsigned transparent_blocks:1;
  unsigned save_text_palette:1;
  unsigned save_project_only_when_closing:1;
  unsigned show_user_after_exit:1;
  unsigned only_user_includes:1;
  unsigned auto_save:1;
  unsigned auto_save_minutes:6;
  unsigned allow_directories:1;
  unsigned automatic_open_disass:1;
  unsigned use_rcs:1;
  unsigned use_fpc:1;
  unsigned save_messages:1;
  unsigned dont_show_exit_code:1;
  unsigned delete_recursive:1;
  unsigned dummy:7;
}
ide_options1;

#define IDE_OPTIONS1 (*((ide_options1*)&(Project.options1)))
#define NoPersistentBlocks (IDE_OPTIONS1.no_persistent_blocks)
#define ShowColumnCursor (IDE_OPTIONS1.column_cursor)
#define ShowRowCursor (IDE_OPTIONS1.row_cursor)
#define IntenseMode (IDE_OPTIONS1.intense_mode)
#define VerboseGDB (IDE_OPTIONS1.verbose_gdb_commands)
#define NoShadows (IDE_OPTIONS1.no_shadows)
#define TransparentBlocks (IDE_OPTIONS1.transparent_blocks)
#define SaveTextPalette (IDE_OPTIONS1.save_text_palette)
#define SaveProjectOnlyWhenClosing (IDE_OPTIONS1.save_project_only_when_closing)
#define ShowUserAfterExit (IDE_OPTIONS1.show_user_after_exit)
#define OnlyUserIncludes (IDE_OPTIONS1.only_user_includes)
#define AutoSave (IDE_OPTIONS1.auto_save)
#define AutoSaveMinutes (IDE_OPTIONS1.auto_save_minutes)
#define AllowDirectories (IDE_OPTIONS1.allow_directories)
#define AutomaticOpenDisass (IDE_OPTIONS1.automatic_open_disass)
#define UseRCS (IDE_OPTIONS1.use_rcs)
#define UseFPC (IDE_OPTIONS1.use_fpc)
#define SaveMessages (IDE_OPTIONS1.save_messages)
#define DontShowExitCode (IDE_OPTIONS1.dont_show_exit_code)
#define DeleteRecursive (IDE_OPTIONS1.delete_recursive)

#define NoStdInc (IDE_OPTIONS.no_stdinc)
#define NoStdLib (IDE_OPTIONS.no_stdlib)
#define ForProfile (IDE_OPTIONS.for_profile)

#define _CReservedWords(PROJECT) (PROJECT->options->reserved)
#define _GPCReservedWords(PROJECT) (PROJECT->options->gpc_reserved)
#define _FPCReservedWords(PROJECT) (PROJECT->options->fpc_reserved)
#define _RHIDEUserWords(PROJECT) (PROJECT->options->user_words)

#define CReservedWords (_CReservedWords(project))
#define GPCReservedWords (_GPCReservedWords(project))
#define FPCReservedWords (_FPCReservedWords(project))
#define RHIDEUserWords (_RHIDEUserWords(project))

#define IsRooted(x) (x[0] == '/' || (rh_isalpha(x[0]) && x[1] == ':'))

void setup_title(const char *);
void setup_main_title();

extern int link_strm;

#endif


