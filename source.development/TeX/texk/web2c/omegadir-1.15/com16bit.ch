% com16bit.ch: Omega version of file tex.ch
%
% tex.ch for C compilation with web2c, derived from various other change files.
% By Tim Morgan, UC Irvine ICS Department, and many others.
%
% (05/28/86) ETM Started with TeX 2.0
% (06/03/87) ETM Brought up to TeX 2.2
% (09/26/87) ETM Brought up to TeX 2.3
% (10/01/87) ETM Brought up to TeX 2.5
% (12/21/87) ETM Brought up to TeX 2.7
% (01/14/88) ETM Brought up to TeX 2.9
% (02/20/88) PAM Revised format and module numbers
% (03/01/88) ETM Eliminated some unused variables and unnecesary tests
% (05/09/88) ETM Added yet another casting bug fix
% (06/21/88) ETM Brought up to TeX version 2.93
% (12/11/88) ETM Brought up to TeX version 2.94
% (01/12/89) PAM Brought up to TeX version 2.95
% (02/14/89) ETM Brought up to TeX version 2.96
% (03/10/89) ETM Brought up to TeX version 2.98
% (07/06/89) ETM Brought up to TeX version 2.991
% (11/30/89) KB  To version 2.992 (8-bit).
% (01/10/90) SR  To version 2.993.
% (03/27/90) KY  To version 3.0.
% (more recent changes in ChangeLog)
%
% The TeX program is copyright (C) 1982 by D. E. Knuth.
% TeX is a trademark of the American Mathematical Society.
%
% The module numbers in this change file refer to TEX.WEB 3.141592 as
% of March, 1995 (published as Donald E. Knuth, TeX: The Program,
% Volume B of Computers & Typesetting).

@x [0.0] l.83 - WEAVE: print changes only.
  \def\?##1]{\hbox to 1in{\hfil##1.\ }}
  }
@y 83
  \def\?##1]{\hbox{Changes to \hbox to 1em{\hfil##1}.\ }}
  }
\let\maybe=\iffalse
@z

@x
@d banner=='This is Omega, Version 3.141592--1.15' {printed when \TeX\ starts}
@y
@d banner=='This is Omega, Version 3.141592--1.15' {printed when \TeX\ starts}
@d banner_k=='This is Omegak, Version 3.141592--1.15' {printed when \TeX\ starts}
@z

@x [1.4] l.233 - program header
Actually the heading shown here is not quite normal: The |program| line
does not mention any |output| file, because \ph\ would ask the \TeX\ user
to specify a file name if |output| were specified here.
@^system dependencies@>
@y
@z

@x [1.4] l.243 - labels in outer block not needed
program OMEGA; {all file names are defined dynamically}
label @<Labels in the outer block@>@/
@y
program OMEGA; {all file names are defined dynamically}
@z

@x [1.6] l.267 - labels in outer block not needed
@ Three labels must be declared in the main program, so we give them
symbolic names.

@d start_of_TEX=1 {go here when \TeX's variables are initialized}
@d end_of_TEX=9998 {go here to close files and terminate gracefully}
@d final_end=9999 {this label marks the ending of the program}

@<Labels in the out...@>=
start_of_TEX@t\hskip-2pt@>, end_of_TEX@t\hskip-2pt@>,@,final_end;
  {key control points}
@y
@ For Web2c, labels are not declared in the main program, but
we still have to declare the symbolic names.

@d start_of_TEX=1 {go here when \TeX's variables are initialized}
@d final_end=9999 {this label marks the ending of the program}
@z

% Here we change these WEB symbols, which are used much as #ifdef's
% are in C, into something which will get translated into actual #ifdef's.
@x [1.7] l.292 - debug..gubed, stat..tats
@d debug==@{ {change this to `$\\{debug}\equiv\null$' when debugging}
@d gubed==@t@>@} {change this to `$\\{gubed}\equiv\null$' when debugging}
@y
@d debug==ifdef('TEXMF_DEBUG')
@d gubed==endif('TEXMF_DEBUG')
@z
@x [1.7] l.297 - debug..gubed, stat..tats
@d stat==@{ {change this to `$\\{stat}\equiv\null$' when gathering
  usage statistics}
@d tats==@t@>@} {change this to `$\\{tats}\equiv\null$' when gathering
  usage statistics}
@y
@d stat==ifdef('STAT')
@d tats==endif('STAT')
@z

@x [1.8] l.312 - Same, for `init...tini'.
@d init== {change this to `$\\{init}\equiv\.{@@\{}$' in the production version}
@d tini== {change this to `$\\{tini}\equiv\.{@@\}}$' in the production version}
@y 312
@d init==ifdef('INITEX')
@d tini==endif('INITEX')
@z

@x [1.8] l.319 - init...tini is dynamic
@!init @<Initialize table entries (done by \.{INITEX} only)@>@;@+tini
@y  318
@!init
if ini_version then
  begin @<Initialize table entries (done by \.{INITEX} only)@>@;
  end;
@+tini
@z

@x [1.11] l.375 - Compile-time constants: most removed for dynamic allocation.
@<Constants...@>=
@!mem_max=30000; {greatest index in \TeX's internal |mem| array;
  must be strictly less than |max_halfword|;
  must be equal to |mem_top| in \.{INITEX}, otherwise |>=mem_top|}
@!mem_min=0; {smallest index in \TeX's internal |mem| array;
  must be |min_halfword| or more;
  must be equal to |mem_bot| in \.{INITEX}, otherwise |<=mem_bot|}
@!buf_size=500; {maximum number of characters simultaneously present in
  current lines of open files and in control sequences between
  \.{\\csname} and \.{\\endcsname}; must not exceed |max_halfword|}
@!error_line=72; {width of context lines on terminal error messages}
@!half_error_line=42; {width of first lines of contexts in terminal
  error messages; should be between 30 and |error_line-15|}
@!max_print_line=79; {width of longest text lines output; should be at least 60}
@!stack_size=200; {maximum number of simultaneous input sources}
@!max_in_open=6; {maximum number of input files and error insertions that
  can be going on simultaneously}
@y
@d file_name_size == maxint
@d ssup_error_line = 255
@d ssup_max_strings ==200000
{Larger values may be used, but then the arrays consume much more memory.}
@d ssup_trie_opcode == 65535
@d ssup_trie_size == 262143

@d ssup_hyph_size == 65535 {Changing this requires changing (un)dumping!}
@d iinf_hyphen_size == 610 {Must be not less than |hyph_prime|!}

@<Constants...@>=
@!mem_bot=0; {smallest index in the |mem| array dumped by \.{INITEX};
  must not be less than |mem_min|}
  {Use |mem_bot=0| for compilers which cannot decrement pointers.}
@z

@x
@!font_max=65535; {maximum internal font number; must be at most |font_biggest|}
@!param_size=60; {maximum number of simultaneous macro parameters}
@!nest_size=40; {maximum number of semantic levels simultaneously active}
@!max_strings=3000; {maximum number of strings; must not exceed |max_halfword|}
@!string_vacancies=8000; {the minimum number of characters that should be
  available for the user's control sequences and font names,
  after \TeX's own error messages are stored}
@!pool_size=32000; {maximum number of characters in strings, including all
  error messages and help texts, and the names of all fonts and
  control sequences; must exceed |string_vacancies| by the total
  length of \TeX's own strings, which is currently about 23000}
@!save_size=600; {space for saving values outside of current group; must be
  at most |max_halfword|}
@!trie_size=8000; {space for hyphenation patterns; should be larger for
  \.{INITEX} than it is in production versions of \TeX}
@!trie_op_size=500; {space for ``opcodes'' in the hyphenation patterns}
@!dvi_buf_size=800; {size of the output buffer; must be a multiple of 8}
@!file_name_size=40; {file names shouldn't be longer than this}
@!pool_name='TeXformats:OMEGA.POOL                   ';
  {string of length |file_name_size|; tells where the string pool appears}
@y
@!font_max=65535; {maximum internal font number; must be at most |font_biggest|}
@!hash_offset=514; {smallest index in hash array, i.e., |hash_base| }
  {Use |hash_offset=0| for compilers which cannot decrement pointers.}
@!trie_op_size=15011; {space for ``opcodes'' in the hyphenation patterns;
  best if relatively prime to 313, 361, and 1009.}
@!neg_trie_op_size=-15011; {for lower |trie_op_hash| array bound;
  must be equal to |-trie_op_size|.}
@!min_trie_op=0; {first possible trie op code for any language}
@!max_trie_op=ssup_trie_opcode; {largest possible trie opcode for any language}
@!pool_name=TEX_POOL_NAME; {this is configurable, for the sake of ML-\TeX}
  {string of length |file_name_size|; tells where the string pool appears}
@#
@!inf_main_memory = 2000000;
@!sup_main_memory = 16000000;

@!inf_trie_size = 80000;
@!sup_trie_size = ssup_trie_size;

@!inf_max_strings = 100000;
@!sup_max_strings = ssup_max_strings;

@!inf_strings_free = 100;
@!sup_strings_free = sup_max_strings;

@!inf_buf_size = 500;
@!sup_buf_size = 200000;

@!inf_nest_size = 40;
@!sup_nest_size = 400;

@!inf_max_in_open = 6;
@!sup_max_in_open = 127;

@!inf_param_size = 60;
@!sup_param_size = 600;

@!inf_save_size = 600;
@!sup_save_size = 40000;

@!inf_stack_size = 200;
@!sup_stack_size = 3000;

@!inf_dvi_buf_size = 800;
@!sup_dvi_buf_size = 65536;

@!inf_pool_size = 32000;
@!sup_pool_size = 10000000;
@!inf_pool_free = 1000;
@!sup_pool_free = sup_pool_size;
@!inf_string_vacancies = 8000;
@!sup_string_vacancies = sup_pool_size - 23000;

@!sup_hyph_size = ssup_hyph_size;
@!inf_hyph_size = iinf_hyphen_size; {Must be not less than |hyph_prime|!}
@z

@x [1.12] l.427 - Constants that are WEB numeric macros.
@d mem_bot=0 {smallest index in the |mem| array dumped by \.{INITEX};
  must not be less than |mem_min|}
@d mem_top==30000 {largest index in the |mem| array dumped by \.{INITEX};
  must be substantially larger than |mem_bot|
  and not greater than |mem_max|}
@d font_base=0 {smallest internal font number; must not be less
  than |min_quarterword|}
@d hash_size=65536 {maximum number of control sequences; it should be at most
  about |(mem_max-mem_min)/10|}
@d hash_prime=55711 {a prime number equal to about 85\pct! of |hash_size|}
@d hyph_size=307 {another prime; the number of \.{\\hyphenation} exceptions}
@y
@d font_base=0 {smallest internal font number; must not be less
  than |min_quarterword|}
@d hash_size=65536 {maximum number of control sequences; it should be at most
  about |(mem_max-mem_min)/10|}
@d hash_prime=55711 {a prime number equal to about 85\pct! of |hash_size|}
@d hyph_prime=607 {another prime for hashing \.{\\hyphenation} exceptions;
                if you change this, you should also change |iinf_hyphen_size|.}
@z

@x [1.16] l.498 - Use C macros for `incr' and `decr'.
@d incr(#) == #:=#+1 {increase a variable by unity}
@d decr(#) == #:=#-1 {decrease a variable by unity}
@y
@z

% The text_char type is used as an array index into xord.  The
% default type `char' produces signed integers, which are bad array
% indices in C.
@x [2.19] l.565 - data type text_char is 8-bit ASCII_code
@d text_char == char {the data type of characters in text files}
@y
@d text_char == ASCII_code {the data type of characters in text files}
@z

% [3.26] name_of_file is no longer an array.  And change the destination
% type to text_char, which fixes:
% 
% Date: 19 Sep 1994 10:38:24 +0200
% From: thorinn@diku.dk (Lars Mathiesen)
%	When echoed to the screen and in the log, character codes
%	above '177 in file names are shown wrongly (typically as ^@).
%
@x [3.26] l.789 - name_of_file is no longer an array
@!name_of_file:packed array[1..file_name_size] of char;@;@/
  {on some systems this may be a \&{record} variable}
@y
@!name_of_file:^char;
@z

@x [3.27] l.794 - Do file opening in C.
@ The \ph\ compiler with which the present version of \TeX\ was prepared has
extended the rules of \PASCAL\ in a very convenient way. To open file~|f|,
we can write
$$\vbox{\halign{#\hfil\qquad&#\hfil\cr
|reset(f,@t\\{name}@>,'/O')|&for input;\cr
|rewrite(f,@t\\{name}@>,'/O')|&for output.\cr}}$$
The `\\{name}' parameter, which is of type `{\bf packed array
$[\langle\\{any}\rangle]$ of \\{char}}', stands for the name of
the external file that is being opened for input or output.
Blank spaces that might appear in \\{name} are ignored.

The `\.{/O}' parameter tells the operating system not to issue its own
error messages if something goes wrong. If a file of the specified name
cannot be found, or if such a file cannot be opened for some other reason
(e.g., someone may already be trying to write the same file), we will have
|@!erstat(f)<>0| after an unsuccessful |reset| or |rewrite|.  This allows
\TeX\ to undertake appropriate corrective action.
@:PASCAL H}{\ph@>
@^system dependencies@>

\TeX's file-opening procedures return |false| if no file identified by
|name_of_file| could be opened.

@d reset_OK(#)==erstat(#)=0
@d rewrite_OK(#)==erstat(#)=0

@p function a_open_in(var f:alpha_file):boolean;
  {open a text file for input}
begin reset(f,name_of_file,'/O'); a_open_in:=reset_OK(f);
end;
@#
function a_open_out(var f:alpha_file):boolean;
  {open a text file for output}
begin rewrite(f,name_of_file,'/O'); a_open_out:=rewrite_OK(f);
end;
@#
function b_open_in(var f:byte_file):boolean;
  {open a binary file for input}
begin reset(f,name_of_file,'/O'); b_open_in:=reset_OK(f);
end;
@#
function b_open_out(var f:byte_file):boolean;
  {open a binary file for output}
begin rewrite(f,name_of_file,'/O'); b_open_out:=rewrite_OK(f);
end;
@#
function w_open_in(var f:word_file):boolean;
  {open a word file for input}
begin reset(f,name_of_file,'/O'); w_open_in:=reset_OK(f);
end;
@#
function w_open_out(var f:word_file):boolean;
  {open a word file for output}
begin rewrite(f,name_of_file,'/O'); w_open_out:=rewrite_OK(f);
end;
@y
@ All of the file opening functions are defined in C.
@z

@x [3.28] l.850 - Do file closing in C.
@ Files can be closed with the \ph\ routine `|close(f)|', which
@^system dependencies@>
should be used when all input or output with respect to |f| has been completed.
This makes |f| available to be opened again, if desired; and if |f| was used for
output, the |close| operation makes the corresponding external file appear
on the user's area, ready to be read.

These procedures should not generate error messages if a file is
being closed before it has been successfully opened.

@p procedure a_close(var f:alpha_file); {close a text file}
begin close(f);
end;
@#
procedure b_close(var f:byte_file); {close a binary file}
begin close(f);
end;
@#
procedure w_close(var f:word_file); {close a word file}
begin close(f);
end;
@y
@ And all the file closing routines as well.
@z

@x [3.30] l.888 - Array size of input buffer is determined at runtime.
@!buffer:array[0..buf_size] of ASCII_code; {lines of characters being read}
@y
@!buffer:^ASCII_code; {lines of characters being read}
@z

@x [3.31] l.933 - Do `input_ln' in C.
@p function input_ln(var f:alpha_file;@!bypass_eoln:boolean):boolean;
  {inputs the next line or returns |false|}
var last_nonblank:0..buf_size; {|last| with trailing blanks removed}
begin if bypass_eoln then if not eof(f) then get(f);
  {input the first character of the line into |f^|}
last:=first; {cf.\ Matthew 19\thinspace:\thinspace30}
if eof(f) then input_ln:=false
else  begin last_nonblank:=first;
  while not eoln(f) do
    begin if last>=max_buf_stack then
      begin max_buf_stack:=last+1;
      if max_buf_stack=buf_size then
        @<Report overflow of the input buffer, and abort@>;
      end;
    buffer[last]:=f^; get(f); incr(last);
    if buffer[last-1]<>" " then last_nonblank:=last;
    end;
  last:=last_nonblank; input_ln:=true;
  end;
end;
@y
We define |input_ln| in C, for efficiency.
@z

% [3.32] `term_in' and `term_out' are standard input and output.
% Declare the variables that used to be constants.
@x [3.32] l.961 - `term_in' and `term_out' are standard input and output.
@<Glob...@>=
@!term_in:alpha_file; {the terminal as an input file}
@!term_out:alpha_file; {the terminal as an output file}
@y
@d term_in==stdin {the terminal as an input file}
@d term_out==stdout {the terminal as an output file}

@<Glob...@>=
@!init
@!ini_version:boolean; {are we \.{INITEX}?}
@!dump_option:boolean; {was the dump name option used?}
@!dump_line:boolean; {was a \.{\%\AM format} line seen?}
tini@/
@#
@!bound_default:integer; {temporary for setup}
@!bound_name:^char; {temporary for setup}
@#
@!main_memory:integer; {total memory words allocated in initex}
@!extra_mem_bot:integer; {|mem_min:=mem_bot-extra_mem_bot| except in \.{INITEX}}
@!mem_min:integer; {smallest index in \TeX's internal |mem| array;
  must be |min_halfword| or more;
  must be equal to |mem_bot| in \.{INITEX}, otherwise |<=mem_bot|}
@!mem_top:integer; {largest index in the |mem| array dumped by \.{INITEX};
  must be substantially larger than |mem_bot|,
  equal to |mem_max| in \.{INITEX}, else not greater than |mem_max|}
@!extra_mem_top:integer; {|mem_max:=mem_top+extra_mem_top| except in \.{INITEX}}
@!mem_max:integer; {greatest index in \TeX's internal |mem| array;
  must be strictly less than |max_halfword|;
  must be equal to |mem_top| in \.{INITEX}, otherwise |>=mem_top|}
@!error_line:integer; {width of context lines on terminal error messages}
@!half_error_line:integer; {width of first lines of contexts in terminal
  error messages; should be between 30 and |error_line-15|}
@!max_print_line:integer;
  {width of longest text lines output; should be at least 60}
@!max_strings:integer; {maximum number of strings; must not exceed |max_halfword|}
@!strings_free:integer; {strings available after format loaded}
@!string_vacancies:integer; {the minimum number of characters that should be
  available for the user's control sequences and font names,
  after \TeX's own error messages are stored}
@!pool_size:integer; {maximum number of characters in strings, including all
  error messages and help texts, and the names of all fonts and
  control sequences; must exceed |string_vacancies| by the total
  length of \TeX's own strings, which is currently about 23000}
@!pool_free:integer;{pool space free after format loaded}
@!hyph_size:integer; {maximun number of hyphen exceptions}
@!trie_size:integer; {space for hyphenation patterns; should be larger for
  \.{INITEX} than it is in production versions of \TeX.  50000 is
  needed for English, German, and Portuguese.}
@!buf_size:integer; {maximum number of characters simultaneously present in
  current lines of open files and in control sequences between
  \.{\\csname} and \.{\\endcsname}; must not exceed |max_halfword|}
@!stack_size:integer; {maximum number of simultaneous input sources}
@!max_in_open:integer; {maximum number of input files and error insertions that
  can be going on simultaneously}
@!param_size:integer; {maximum number of simultaneous macro parameters}
@!nest_size:integer; {maximum number of semantic levels simultaneously active}
@!save_size:integer; {space for saving values outside of current group; must be
  at most |max_halfword|}
@!dvi_buf_size:integer; {size of the output buffer; must be a multiple of 8}
@z

@x [3.33] l.964 - We don't need to open terminal files.
@ Here is how to open the terminal files
in \ph. The `\.{/I}' switch suppresses the first |get|.
@^system dependencies@>

@d t_open_in==reset(term_in,'TTY:','/O/I') {open the terminal for text input}
@d t_open_out==rewrite(term_out,'TTY:','/O') {open the terminal for text output}
@y
@ Here is how to open the terminal files.  |t_open_out| does nothing.
|t_open_in|, on the other hand, does the work of ``rescanning,'' or getting
any command line arguments the user has provided.  It's defined in C.

@d t_open_out == {output already open for text output}
@z

@x [3.34] l.982 - Flushing output to terminal files.
these operations can be specified in \ph:
@^system dependencies@>

@d update_terminal == break(term_out) {empty the terminal output buffer}
@d clear_terminal == break_in(term_in,true) {clear the terminal input buffer}
@y
these operations can be specified with {\mc UNIX}.  |update_terminal|
does an |fflush|. |clear_terminal| is redefined
to do nothing, since the user should control the terminal.
@^system dependencies@>

@d update_terminal == fflush (term_out)
@d clear_terminal == do_nothing
@z

@x [3.35] l.1017 - Do `input_ln' in C => section not needed.
@<Report overflow of the input buffer, and abort@>=
if format_ident=0 then
  begin write_ln(term_out,'Buffer size exceeded!'); goto final_end;
@.Buffer size exceeded@>
  end
else begin cur_input.loc_field:=first; cur_input.limit_field:=last-1;
  overflow("buffer size",buf_size);
@:TeX capacity exceeded buffer size}{\quad buffer size@>
  end
@y
Routine is implemented in C, module is not needed.
@z

@x [3.37] l.1055 - |init_terminal|, reading the command line.
@ The following program does the required initialization
without retrieving a possible command line.
It should be clear how to modify this routine to deal with command lines,
if the system permits them.
@^system dependencies@>

@p function init_terminal:boolean; {gets the terminal input started}
label exit;
begin t_open_in;
@y
@ The following program does the required initialization.
Iff anything has been specified on the command line, then |t_open_in|
will return with |last > first|.
@^system dependencies@>

@p function init_terminal:boolean; {gets the terminal input started}
label exit;
begin t_open_in;
if last > first then
  begin loc := first;
  while (loc < last) and (buffer[loc]=' ') do incr(loc);
  if loc < last then
    begin init_terminal := true; goto exit;
    end;
  end;
@z

@x [3.37] l.1068 - |init_terminal|, output missing newline.
    write(term_out,'! End of file on the terminal... why?');
@y
    write_ln(term_out,'! End of file on the terminal... why?');
@z

@x [4.38] l.1126 - Array size for string pool is determined at runtime.
@!pool_pointer = 0..pool_size; {for variables that point into |str_pool|}
@!str_number = 0..max_strings; {for variables that point into |str_start|}
@y
@!pool_pointer = integer; {for variables that point into |str_pool|}
@!str_number = 0..ssup_max_strings; {for variables that point into |str_start|}
@z

@x [4.39] l.1131 - Dynamically size pool arrays.
@!str_pool:packed array[pool_pointer] of packed_ASCII_code; {the characters}
@!str_start_ar : array[str_number] of pool_pointer; {the starting pointers}
@y
@!str_pool: ^packed_ASCII_code; {the characters}
@!str_start_ar : ^pool_pointer; {the starting pointers}
@z

@x [4.47] l.1237 - string recycling
@p @!init function get_strings_started:boolean; {initializes the string pool,
@y
@p @t\4@>@<Declare additional routines for string recycling@>@/

@!init function get_strings_started:boolean; {initializes the string pool,
@z

% [4.51] Open the pool file using a path, and can't do string
% assignments directly.  (`strcpy' and `strlen' work here because
% `pool_name' is a constant string, and thus ends in a null and doesn't
% start with a space.)
@x [4.51] l.1314 - Open the pool file.
name_of_file:=pool_name; {we needn't set |name_length|}
if a_open_in(pool_file) then
@y
name_length := strlen (pool_name);
name_of_file := xmalloc_array (char, name_length + 1);
strcpy (name_of_file+1, pool_name); {copy the string}
if a_open_in (pool_file, kpse_texpool_format) then
@z

@x [4.51] l.1322 - Make `OMEGA.POOL' lowercase, and change how it's read.
else  bad_pool('! I can''t read OMEGA.POOL.')
@y
else  bad_pool('! I can''t read omega.pool; bad path?')
@z
@x [4.52] l.1326 - Make `OMEGA.POOL' lowercase, and change how it's read.
begin if eof(pool_file) then bad_pool('! OMEGA.POOL has no check sum.');
@.OMEGA.POOL has no check sum@>
read(pool_file,m,n); {read two digits of string length}
@y
begin if eof(pool_file) then bad_pool('! omega.pool has no check sum.');
@.OMEGA.POOL has no check sum@>
read(pool_file,m); read(pool_file,n); {read two digits of string length}
@z
@x [4.52] l.1332 - Make `OMEGA.POOL' lowercase, and change how it's read.
    bad_pool('! OMEGA.POOL line doesn''t begin with two digits.');
@y
    bad_pool('! omega.pool line doesn''t begin with two digits.');
@z
@x [4.53] l.1354 - Make `OMEGA.POOL' lowercase, and change how it's read.
  bad_pool('! OMEGA.POOL check sum doesn''t have nine digits.');
@y
  bad_pool('! omega.pool check sum doesn''t have nine digits.');
@z
@x [4.53] l.1360 - Make `OMEGA.POOL' lowercase, and change how it's read.
done: if a<>@$ then bad_pool('! OMEGA.POOL doesn''t match; OTANGLE me again.');
@y
done: if a<>@$ then
  bad_pool('! omega.pool doesn''t match; otangle me again (or fix the path).');
@z

@x [5.54] l.1422 - error_line
@!trick_buf:array[0..error_line] of ASCII_code; {circular buffer for
@y
@!trick_buf:array[0..ssup_error_line] of ASCII_code; {circular buffer for
@z

@x [5.61] l.1556 - Print rest of banner, eliminate misleading `(no format preloaded)'.
wterm(banner);
if format_ident=0 then wterm_ln(' (no format preloaded)')
else  begin slow_print(format_ident); print_ln;
  end;
@y
wterm(banner);
wterm(version_string);
if format_ident>0 then slow_print(format_ident);
print_ln;
@z

@x [6.73] l.1732 - Add unspecified_mode.
@d error_stop_mode=3 {stops at every opportunity to interact}
@y
@d error_stop_mode=3 {stops at every opportunity to interact}
@d unspecified_mode=4 {extra value for command-line switch}
@z

@x [6.73] l.1738 - Add interaction_option.
@!interaction:batch_mode..error_stop_mode; {current level of interaction}
@y
@!interaction:batch_mode..error_stop_mode; {current level of interaction}
@!interaction_option:batch_mode..unspecified_mode; {set from command line}
@z

@x [6.74] l.1740 - Allow override by command line switch.
@ @<Set init...@>=interaction:=error_stop_mode;
@y
@ @<Set init...@>=if interaction_option=unspecified_mode then
  interaction:=error_stop_mode
else
  interaction:=interaction_option;
@z

% [6.81] Eliminate nonlocal goto, since C doesn't have them.
% Plus, it's nicer just to do an exit with the appropriate status code
% under Unix.  We call it `uexit' because there's a WEB symbol called
% `exit' already.  We use a C macro to change `uexit' back to `exit'.
@x [6.81] l.1852 - Eliminate nonlocal goto, since C doesn't have them.
@<Error hand...@>=
procedure jump_out;
begin goto end_of_TEX;
end;
@y
@d do_final_end==begin
   update_terminal;
   ready_already:=0;
   if (history <> spotless) and (history <> warning_issued) then
       uexit(1)
   else
       uexit(0);
   end

@<Error hand...@>=
procedure jump_out;
begin
close_files_and_terminate;
do_final_end;
end;
@z

@x [6.84] l.1888 - Implement the switch-to-editor option.
line ready to be edited. But such an extension requires some system
wizardry, so the present implementation simply types out the name of the
file that should be
edited and the relevant line number.
@^system dependencies@>

There is a secret `\.D' option available when the debugging routines haven't
been commented~out.
@^debugging@>
@y
line ready to be edited.
We do this by calling the external procedure |call_edit| with a pointer to
the filename, its length, and the line number.
However, here we just set up the variables that will be used as arguments,
since we don't want to do the switch-to-editor until after TeX has closed
its files.
@^system dependencies@>

There is a secret `\.D' option available when the debugging routines haven't
been commented~out.
@^debugging@>
@d edit_file==input_stack[base_ptr]
@z

@x [6.84] l.1903 - Implement the switch-to-editor option.
"E": if base_ptr>0 then
  begin print_nl("You want to edit file ");
@.You want to edit file x@>
  slow_print(input_stack[base_ptr].name_field);
  print(" at line "); print_int(line);
  interaction:=scroll_mode; jump_out;
@y
"E": if base_ptr>0 then
    begin edit_name_start:=str_start(edit_file.name_field);
    edit_name_length:=str_start(edit_file.name_field+1) -
    		      str_start(edit_file.name_field);
    edit_line:=line;
    jump_out;
@z

% [7.104] `remainder' is a library routine on some systems, so change
% its name to avoid conflicts.
@x [7.104] l.2227 - avoid name conflicts with lib routine remainder()
|remainder|, holds the remainder after a division.

@<Glob...@>=
@y
|remainder|, holds the remainder after a division.

@d remainder==tex_remainder

@<Glob...@>=
@z

@x [7.109] l.2352 - Define glue_ratio in C.
@!glue_ratio=real; {one-word representation of a glue expansion factor}
@y
@z

@x [8.112] l.2435 - Efficiency.
macros are simplified in the obvious way when |min_quarterword=0|.
@^inner loop@>@^system dependencies@>

@d qi(#)==#+min_quarterword
  {to put an |eight_bits| item into a quarterword}
@d qo(#)==#-min_quarterword
  {to take an |eight_bits| item out of a quarterword}
@d hi(#)==#+min_halfword
  {to put a sixteen-bit item into a halfword}
@d ho(#)==#-min_halfword
  {to take a sixteen-bit item from a halfword}
@y
macros are simplified in the obvious way when |min_quarterword=0|.
So they have been simplified here in the obvious way.
@^inner loop@>@^system dependencies@>

@d qi(#)==# {to put an |eight_bits| item into a quarterword}
@d qo(#)==# {to take an |eight_bits| item from a quarterword}
@d hi(#)==# {to put a sixteen-bit item into a halfword}
@d ho(#)==# {to take a sixteen-bit item from a halfword}
@z

% [8.113] We've put the memory structure into the include file
% `texmf.h', since it's too hard to translate automatically.
@x [8.113] l.2453 - data structures for main memory
@!quarterword = min_quarterword..max_quarterword; {1/4 of a word}
@!halfword=min_halfword..max_halfword; {1/2 of a word}
@!two_choices = 1..2; {used when there are two variants in a record}
@!four_choices = 1..4; {used when there are four variants in a record}
@!two_halves = packed record@;@/
  @!rh:halfword;
  case two_choices of
  1: (@!lh:halfword);
  2: (@!b0:quarterword; @!b1:quarterword);
  end;
@!four_quarters = packed record@;@/
  @!b0:quarterword;
  @!b1:quarterword;
  @!b2:quarterword;
  @!b3:quarterword;
  end;
@!memory_word = record@;@/
  case four_choices of
  1: (@!int:integer);
  2: (@!gr:glue_ratio);
  3: (@!hh:two_halves);
  4: (@!qqqq:four_quarters);
  end;
@y
@!quarterword = min_quarterword..max_quarterword;
@!halfword = min_halfword..max_halfword;
@!two_choices = 1..2; {used when there are two variants in a record}
@!four_choices = 1..4; {used when there are four variants in a record}
@=#include "texmfmem.h";@>
@=#include "omegamem.h";@>
@z

% [9.116] Change `mem' to `zmem', so we can define mem to be a register
% pointer to the memory array for speed.
@x [9.116] l.2545 - definition of main memory array
@!mem : array[mem_min..mem_max] of memory_word; {the big dynamic storage area}
@y
@!yzmem : ^memory_word; {the big dynamic storage area}
@!zmem : ^memory_word; {the big dynamic storage area}
@z

% [9.127] Fix casting problem in C.
% There are several of these.  They come from the rules C uses for
% comparing signed and unsigned quantities.  Just doing the comparison
% can result in incorrect evaluation wrt the way Pascal would do it.
@x [9.127] l.2739 - Fix casting problem in C.
if r>p+1 then @<Allocate from the top of node |p| and |goto found|@>;
@y 2738
if r>intcast(p+1) then @<Allocate from the top of node |p| and |goto found|@>;
@z

% [11.165] Fix the word `free' so that it doesn't conflict with the
% standard C library routine of the same name.
@x [11.165] l.3364 - avoid conflict with lib function free()
are debugging.)
@y
are debugging.)

@d free==free_arr
@z

@x [11.165] l.3367 - dummy |free| and |was_free| arrays
@!debug @!free: packed array [mem_min..mem_max] of boolean; {free cells}
@t\hskip10pt@>@!was_free: packed array [mem_min..mem_max] of boolean;
@y
 {The debug memory arrays have not been mallocated yet.}
@!debug @!free: packed array [0..9] of boolean; {free cells}
@t\hskip10pt@>@!was_free: packed array [0..9] of boolean;
@z

@x [12.174] l.3526 - Eliminate unsigned comparisons to zero.
        begin if (font(p)<font_base)or(font(p)>font_max) then
          print_char("*")
@.*\relax@>
        else @<Print the font identifier for |font(p)|@>;
@y
        begin @<Print the font identifier for |font(p)|@>;
@z

@x [12.176] l.3563 - Eliminate unsigned comparisons to zero.
begin if p>mem_end then print_esc("CLOBBERED.")
else  begin if (font(p)<font_base)or(font(p)>font_max) then print_char("*")
@.*\relax@>
  else @<Print the font identifier for |font(p)|@>;
@y
begin if p>mem_end then print_esc("CLOBBERED.")
else  begin @<Print the font identifier for |font(p)|@>;
@z

@x [12.186] l.3747 - Don't worry about strange floating point values.
  if abs(mem[p+glue_offset].int)<@'4000000 then print("?.?")
  else if abs(g)>float_constant(20000) then
@y 3747
  { The Unix |pc| folks removed this restriction with a remark that
    invalid bit patterns were vanishingly improbable, so we follow
    their example without really understanding it.
  |if abs(mem[p+glue_offset].int)<@'4000000 then print('?.?')|
  |else| }
  if fabs(g)>float_constant(20000) then
@z


@x [16.213] l.4321 - texarray
@!nest:array[0..nest_size] of list_state_record;
@y
@!nest:^list_state_record;
@z

@x [16.215] l.4344 - remove mem[] reference from initialize.
@<Start a new current page@>;
@y
@/{The following piece of code is a copy of module 991:}
page_contents:=empty; page_tail:=page_head; {|link(page_head):=null;|}@/
last_glue:=max_halfword; last_penalty:=0; last_kern:=0;
page_depth:=0; page_max_depth:=0;
@z

@x [17.241] l.5219 - Do `fix_date_and_time' in C.
@ The following procedure, which is called just before \TeX\ initializes its
input and output, establishes the initial values of the date and time.
@^system dependencies@>
Since standard \PASCAL\ cannot provide such information, something special
is needed. The program here simply specifies July 4, 1776, at noon; but
users probably want a better approximation to the truth.

@p procedure fix_date_and_time;
begin time:=12*60; {minutes since midnight}
day:=4; {fourth day of the month}
month:=7; {seventh month of the year}
year:=1776; {Anno Domini}
end;
@y
@ The following procedure, which is called just before \TeX\ initializes its
input and output, establishes the initial values of the date and time.
It calls a macro-defined |date_and_time| routine.  |date_and_time|
in turn is a C macro, which calls |get_date_and_time|, passing
it the addresses of the day, month, etc., so they can be set by the
routine.  |get_date_and_time| also sets up interrupt catching if that
is conditionally compiled in the C code.
@^system dependencies@>

@d fix_date_and_time==o_date_and_time(int_base+time_code,
                                      int_base+day_code,
                                      int_base+month_code,
                                      int_base+year_code)
@z

@x [18.262] l.5584 - Remove more unsigned comparisons to zero.
else if (newtext(p)<0)or(newtext(p)>=str_ptr) then print_esc("NONEXISTENT.")
@y
else if (newtext(p)>=str_ptr) then print_esc("NONEXISTENT.")
@z

@x [19.271] l.5872 - texarray
@!save_stack : array[0..save_size] of memory_word;
@y
@!save_stack : ^memory_word;
@z

@x [22.301] l.6432 - texarray
@!input_stack : array[0..stack_size] of in_state_record;
@y
@!input_stack : ^in_state_record;
@z

@x [22.304] l.6536 - texarray
@!input_file : array[1..max_in_open] of alpha_file;
@!input_file_mode : array[1..max_in_open] of halfword;
@!input_file_translation : array[1..max_in_open] of halfword;
@!line : integer; {current line number in the current source file}
@!line_stack : array[1..max_in_open] of integer;
@y
@!input_file : ^alpha_file;
@!input_file_mode : ^halfword;
@!input_file_translation : ^halfword;
@!line : integer; {current line number in the current source file}
@!line_stack : ^integer;
@z

@x [22.308] l.6701 - texarray
@!param_stack:array [0..param_size] of pointer;
  {token list pointers for parameters}
@y
@!param_stack: ^pointer;
  {token list pointers for parameters}
@z

@x [29.513] l.9951 - Area and extension rules for filenames.
@ The file names we shall deal with for illustrative purposes have the
following structure:  If the name contains `\.>' or `\.:', the file area
consists of all characters up to and including the final such character;
otherwise the file area is null.  If the remaining file name contains
`\..', the file extension consists of all such characters from the first
remaining `\..' to the end, otherwise the file extension is null.
@y
@ The file names we shall deal with have the
following structure:  If the name contains `\./' or `\.:'
(for Amiga only), the file area
consists of all characters up to and including the final such character;
otherwise the file area is null.  If the remaining file name contains
`\..', the file extension consists of all such characters from the last
`\..' to the end, otherwise the file extension is null.
@z

@x [29.513] l.9963 - Area and extension rules for filenames.
@!area_delimiter:pool_pointer; {the most recent `\.>' or `\.:', if any}
@!ext_delimiter:pool_pointer; {the relevant `\..', if any}
@y
@!area_delimiter:pool_pointer; {the most recent `\./', if any}
@!ext_delimiter:pool_pointer; {the most recent `\..', if any}
@z

@x [29.514] l.9973 - TeX area directories.
@d TEX_area=="TeXinputs:"
@.TeXinputs@>
@d TEX_font_area=="TeXfonts:"
@.TeXfonts@>
@y
In C, the default paths are specified separately.
@z

@x [29.516] l.9994 - filenames: more_name
  if (c=">")or(c=":") then
@y
  if IS_DIR_SEP(c) then
@z

@x [29.516] l.9997 - filenames: more_name
  else if (c=".")and(ext_delimiter=0) then ext_delimiter:=cur_length;
@y
  else if c="." then ext_delimiter:=cur_length;
@z

@x [29.517] l.10002 - end_name: string recycling
@ The third.
@^system dependencies@>
 
@p procedure end_name;
@y
@ The third.
@^system dependencies@>
If a string is already in the string pool, the function
|slow_make_string| does not create a new string but returns this string
number, thus saving string space.  Because of this new property of the
returned string number it is not possible to apply |flush_string| to
these strings.

@p procedure end_name;
var temp_str: str_number; {result of file name cache lookups}
@!j: pool_pointer; {running index}
@z

@x [29.517] l.10011 - end_name: string recycling
  str_start(str_ptr+1):=str_start(str_ptr)+area_delimiter; incr(str_ptr);
  end;
if ext_delimiter=0 then
  begin cur_ext:=""; cur_name:=make_string;
@y
  str_start(str_ptr+1):=str_start(str_ptr)+area_delimiter; incr(str_ptr);
  temp_str:=search_string(cur_area);
  if temp_str>0 then
    begin cur_area:=temp_str;
    decr(str_ptr);  {no |flush_string|, |pool_ptr| will be wrong!}
    for j:=str_start(str_ptr+1) to pool_ptr-1 do
      begin str_pool[j-area_delimiter]:=str_pool[j];
      end;
    pool_ptr:=pool_ptr-area_delimiter; {update |pool_ptr|}
    end;
  end;
if ext_delimiter=0 then
  begin cur_ext:=""; cur_name:=slow_make_string;
@z

@x [29.517] l.10016 - end_name: string recycling
else  begin cur_name:=str_ptr;
  str_start(str_ptr+1):=str_start(str_ptr)+ext_delimiter-area_delimiter-1;
  incr(str_ptr); cur_ext:=make_string;
@y
else  begin cur_name:=str_ptr;
  str_start(str_ptr+1):=str_start(str_ptr)+ext_delimiter-area_delimiter-1;
  incr(str_ptr); cur_ext:=make_string;
  decr(str_ptr); {undo extension string to look at name part}
  temp_str:=search_string(cur_name);
  if temp_str>0 then
    begin cur_name:=temp_str;
    decr(str_ptr);  {no |flush_string|, |pool_ptr| will be wrong!}
    for j:=str_start(str_ptr+1) to pool_ptr-1 do
      begin str_pool[j-ext_delimiter+area_delimiter+1]:=str_pool[j];
      end;
    pool_ptr:=pool_ptr-ext_delimiter+area_delimiter+1;  {update |pool_ptr|}
    end;
  cur_ext:=slow_make_string;  {remake extension string}
@z

% [29.519] In pack_file_name, leave room for the extra null we append at
% the end of a filename.
@x [29.519] l.10047 - pack_file_name, leave room for the extra null
for j:=str_start(a) to str_start(a+1)-1 do append_to_name(so(str_pool[j]));
@y
if name_of_file then libc_free (name_of_file);
name_of_file:= xmalloc_array(char, 1 + length(a) + length(n) + length(e));
for j:=str_start(a) to str_start(a+1)-1 do append_to_name(so(str_pool[j]));
@z

@x [29.519] l.10051 - pack_file_name, append the extra null
for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
@y
name_of_file[name_length+1]:=0;
@z

@x [29.520] l.10060 - filenames: default format.
@d format_default_length=20 {length of the |TEX_format_default| string}
@d format_area_length=11 {length of its area part}
@d format_ext_length=4 {length of its `\.{.fmt}' part}
@d format_extension=".fmt" {the extension, as a \.{WEB} constant}
@y
Under {\mc UNIX} we don't give the area part, instead depending
on the path searching that will happen during file opening.  Also, the
length will be set in the main program.

@d format_area_length=0 {length of its area part}
@d format_ext_length=4 {length of its `\.{.oft}' part}
@d format_extension=".oft" {the extension, as a \.{WEB} constant}
@z

@x [29.521] l.10066 - filenames: default format, where `plain.fmt' is.
@!TEX_format_default:packed array[1..format_default_length] of char;

@ @<Set init...@>=
TEX_format_default:='TeXformats:plain.fmt';
@y
@!format_default_length: integer;
@!TEX_format_default: ^char;

@ We set the name of the default format file and the length of that name
in C, instead of Pascal, since we want them to depend on the name of the
program.
@z

@x [29.523] l.10095 - Change to pack_buffered_name as with pack_file_name.
for j:=1 to n do append_to_name(TEX_format_default[j]);
@y
if name_of_file then libc_free (name_of_file);
name_of_file := xmalloc_array (char, 1 + n + (b - a + 1) + format_ext_length);
for j:=1 to n do append_to_name(TEX_format_default[j]);
@z

@x [29.523] l.10100 - Change to pack_buffered_name as with pack_file_name.
for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
@y
name_of_file[name_length+1]:=0;
@z

@x [29.524] l.10118 - Format file opening: only try once, with path searching.
  pack_buffered_name(0,loc,j-1); {try first without the system file area}
  if w_open_in(fmt_file) then goto found;
  pack_buffered_name(format_area_length,loc,j-1);
    {now try the system format file area}
  if w_open_in(fmt_file) then goto found;
@y
  pack_buffered_name(0,loc,j-1); {Kpathsea does everything}
  if w_open_in(fmt_file) then goto found;
@z

@x [29.524] l.10124 - replace `PLAIN' in error messages with `default'.
  wterm_ln('Sorry, I can''t find that format;',' will try PLAIN.');
@y
  wterm ('Sorry, I can''t find the format `');
  fputs (name_of_file + 1, stdout);
  wterm ('''; will try `');
  fputs (TEX_format_default + 1, stdout);
  wterm_ln ('''.');
@z

@x [29.524] l.10132 - replace `PLAIN' in error messages with `default'.
  wterm_ln('I can''t find the PLAIN format file!');
@.I can't find PLAIN...@>
@y
  wterm ('I can''t find the format file `');
  fputs (TEX_format_default + 1, stdout);
  wterm_ln ('''!');
@.I can't find the format...@>
@z

@x [29.530] l.10239 - prompt_file_name: No default extension is TeX input file.
if e=".tex" then show_context;
@y
if (e=".tex") or (e="") then show_context;
@z

@x [29.532] l.10263 - avoid conflict, `logname' in <unistd.h> on some systems.
@ Here's an example of how these conventions are used. Whenever it is time to
ship out a box of stuff, we shall use the macro |ensure_dvi_open|.

@y
@ Here's an example of how these conventions are used. Whenever it is time to
ship out a box of stuff, we shall use the macro |ensure_dvi_open|.

@d log_name == texmf_log_name
@z

@x [29.534] l.10285 - Adjust for C string conventions.
@!months:packed array [1..36] of char; {abbreviations of month names}
@y
@!months:^char;
j:integer;
@z

@x [29.534] l. - Send the job_name to the file recorder.
begin old_setting:=selector;
if job_name=0 then job_name:="texput";
@y
begin old_setting:=selector;
if job_name=0 then job_name:="texput";
pack_job_name(".ofl");
recorder_change_filename(stringcast(name_of_file+1));
@z

@x [29.536] l.10324 - Print rest of banner.
begin wlog(banner);
@y
begin wlog(banner);
wlog(version_string);
@z

@x [29.536] l.10327 - Adjust for C string conventions.
months:='JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC';
@y
months := ' JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC';
@z

% [29.537] Use a path when calling a_open_in to do a \input; also, try
% to open the file with and without the `.tex' extension, regardless of
% whether the file already has an extension.  This allows filenames like
% `foo' and `foo.bar.tex', as well as `foo.tex' and `foo.bar'.
@x [29.537] l.10338 - start_input
begin scan_file_name; {set |cur_name| to desired file name}
if cur_ext="" then cur_ext:=".tex";
pack_cur_name;
loop@+  begin begin_file_reading; {set up |cur_file| and new level of input}
  if a_open_in(cur_file) then goto done;
  if cur_area="" then
    begin pack_file_name(cur_name,TEX_area,cur_ext);
    if a_open_in(cur_file) then goto done;
    end;
@y
var temp_str: str_number; k: integer;
begin scan_file_name; {set |cur_name| to desired file name}
pack_cur_name;
loop@+  begin begin_file_reading; {set up |cur_file| and new level of input}
  tex_input_type := 1; {Tell |open_input| we are \.{\\input}.}
  {Kpathsea tries all the various ways to get the file.}
  if open_in_name_ok(name_of_file+1)
     and a_open_in(cur_file, kpse_tex_format) then
    {At this point |name_of_file| contains the actual name found.
     We extract the |cur_area|, |cur_name|, and |cur_ext| from it.}
    begin k:=1;
    name_in_progress:=true;
    begin_name;
    while (k<=name_length)and(more_name(name_of_file[k])) do
      incr(k);
    end_name;
    name_in_progress:=false;
    goto done;
    end;
@z

@x [29.537] l.10348 - start_input: don't force ".tex" extension.
  prompt_file_name("input file name",".tex");
@y
  prompt_file_name("input file name","");
@z

@x [29.537] l.10350 - start_input: string recycling
done: name:=a_make_name_string(cur_file);
@y
done: name:=a_make_name_string(cur_file);
if name=str_ptr-1 then {we can try to conserve string pool space now}
  begin temp_str:=search_string(name);
  if temp_str>0 then
    begin name:=temp_str; flush_string;
    end;
  end;
@z

@x [29.537] l.10352 - start_input: use different heuristic for initex.
  begin job_name:=cur_name; open_log_file;
@y
  begin job_name:=get_job_name; open_log_file;
@z

@x [29.537] l.10359 - start_input: don't return filename to string pool.
if name=str_ptr-1 then {we can conserve string pool space now}
  begin flush_string; name:=cur_name;
  end;
@y
@z

@x [30.563] l.10943 - Don't use TEX_font_area.
if aire="" then pack_file_name(nom,TEX_font_area,".ofm")
else pack_file_name(nom,aire,".ofm");
if not b_open_in(tfm_file) then abort;
@y
{|kpse_find_file| will append the |".ofm"| or |".tfm"|,
 and avoid searching the disk before the font alias files as well.}
pack_file_name(nom,aire,"");
if not ofm_open_in(tfm_file) then abort;
@z

% [30.564] Reading the tfm file.  As a special case, whenever we open a
% tfm file, we read its first byte into `tfm_temp' right away.  TeX
% looks at `fbyte' before calling `fget', so it ends up seeing every
% byte.  This is Pascal-like I/O.
@x [30.564] l.10956 - reading the tfm file, define fget & fbyte
@d fget==get(tfm_file)
@d fbyte==tfm_file^
@y
@d fget==tfm_temp:=getc(tfm_file)
@d fbyte==tfm_temp
@z

% [32.575] We only want `eof' on the TFM file to be true if we
% previously had EOF, not if we're at EOF now.  This is like `feof', and
% unlike our implementation of `eof' elsewhere.
@x [32.575] l.11161 - Reading the tfm file, replace eof() by feof().
if eof(tfm_file) then abort;
@y
if feof(tfm_file) then abort;
@z

@x [32.595] l.11860 - texarray
@!dvi_buf:array[dvi_index] of real_eight_bits; {buffer for \.{DVI} output}
@!half_buf:dvi_index; {half of |dvi_buf_size|}
@!dvi_limit:dvi_index; {end of the current half buffer}
@!dvi_ptr:dvi_index; {the next available buffer address}
@y
@!dvi_buf:^real_eight_bits; {buffer for \.{DVI} output}
@!half_buf:integer; {half of |dvi_buf_size|}
@!dvi_limit:integer; {end of the current half buffer}
@!dvi_ptr:integer; {the next available buffer address}
@z

@x [32.597] l.11886 - write_dvi done in C.
@p procedure write_dvi(@!a,@!b:dvi_index);
var k:dvi_index;
begin for k:=a to b do write(dvi_file,dvi_buf[k]);
end;
@y
In C, we use a macro to call |fwrite| or |write| directly, writing all
the bytes in one shot.  Much better even than writing four
bytes at a time.
@z

@x [32.617] l.12261 - Use output_comment if the user set it. Assume it's short enough.
  old_setting:=selector; selector:=new_string;
@y
if output_comment then
  begin l:=strlen(output_comment); dvi_out(l);
  for s:=0 to l-1 do dvi_out(output_comment[s]);
  end
else begin {the default code is unchanged}
  old_setting:=selector; selector:=new_string;
@z

@x [32.617] l.12268 - Use output_comment if the user set it.
  pool_ptr:=str_start(str_ptr); {flush the current string}
@y
  pool_ptr:=str_start(str_ptr); {flush the current string}
end;
@z

% We output each portion of the page as we get to it, if we are using
% IPC, so that the previewer (TeXView) can display it immediately. [SPM]
@x [32.640] l.12690 - IPC
dvi_out(eop); incr(total_pages); cur_s:=-1;
@y
dvi_out(eop); incr(total_pages); cur_s:=-1;
ifdef ('IPC')
if ipc_on>0 then
  begin if dvi_limit=half_buf then
    begin write_dvi(half_buf, dvi_buf_size-1);
    flush_dvi;
    dvi_gone:=dvi_gone+half_buf;
    end;
  if dvi_ptr>0 then
    begin write_dvi(0, dvi_ptr-1);
    flush_dvi;
    dvi_offset:=dvi_offset+dvi_ptr; dvi_gone:=dvi_gone+dvi_ptr;
    end;
  dvi_ptr:=0; dvi_limit:=dvi_buf_size;
  ipc_page(dvi_gone);
  end;
endif ('IPC');
@z

@x [32.642] l.12742 - Use dvi_offset instead of dvi_buf_size with IPC stuff.
  k:=4+((dvi_buf_size-dvi_ptr) mod 4); {the number of 223's}
@y
ifdef ('IPC')
  k:=7-((3+dvi_offset+dvi_ptr) mod 4); {the number of 223's}
endif ('IPC')
ifndef ('IPC')
  k:=4+((dvi_buf_size-dvi_ptr) mod 4); {the number of 223's}
endifn ('IPC')
@z

@x [38.859] l.16855 - Fix a casting/expression evaluation problem.
if abs(fit_class-fitness(r))>1 then d:=d+adj_demerits;
@y
if abs(intcast(fit_class)-intcast(fitness(r)))>1 then d:=d+adj_demerits;
@z

@x [39.875] l.17170 - Another casting problem.
  begin line_diff:=line_number(r)-best_line;
@y
  begin line_diff:=intcast(line_number(r))-intcast(best_line);
@z

@x [42.920] l.18068 - bigtrie: allow larger hyphenation tries.
@!trie_pointer=0..trie_size; {an index into |trie|}
@y
@!trie_pointer=0..ssup_trie_size; {an index into |trie|}
@z
 
@x [42.921] l.18075 - bigtrie: allow larger hyphenation tries.
@!trie:array[trie_pointer] of two_halves; {|trie_link|, |trie_char|, |trie_op|}
@y
{We will dynamically allocate this arrays.}
@!trie:^two_halves; {|trie_link|}
@z

%%%%%%%% dynamic hyph_size
@x 18126 m.925
different from $\alpha$, we can conclude that $\alpha$ is not in the table.
@y  18126
different from $\alpha$, we can conclude that $\alpha$ is not in the table.
This is a clever scheme which saves the need for a hash link array.
However, it is difficult to increase the size of the hyphen exception
arrays. To make this easier, the ordered hash has been replaced by
a simple hash, using an additional array |hyph_link|. The value
|0| in |hyph_link[k]| means that there are no more entries corresponding
to the specific hash chain. When |hyph_link[k]>0|, the next entry in
the hash chain is |hyph_link[k]-1|. This value is used because the
arrays start at |0|.
@z

%%%%%%%% dynamic hyph_size
@x 18134 m.925
@!hyph_pointer=0..hyph_size; {an index into the ordered hash table}
@y  18134
@!hyph_pointer=0..ssup_hyph_size; {index into hyphen exceptions hash table;
                     enlarging this requires changing (un)dump code}
@z

%%%%%%%% dynamic hyph_size
@x 18137 m.926
@!hyph_word:array[hyph_pointer] of str_number; {exception words}
@!hyph_list:array[hyph_pointer] of pointer; {list of hyphen positions}
@!hyph_count:hyph_pointer; {the number of words in the exception dictionary}
@y  18139
@!hyph_word: ^str_number; {exception words}
@!hyph_list: ^pointer; {list of hyphen positions}
@!hyph_link: ^hyph_pointer; {link array for hyphen exceptions hash table}
@!hyph_count:integer; {the number of words in the exception dictionary}
@!hyph_next:integer; {next free slot in hyphen exceptions hash table}
@z

%%%%%%%% dynamic hyph_size
@x 18145 m.928
for z:=0 to hyph_size do
  begin hyph_word[z]:=0; hyph_list[z]:=null;
  end;
hyph_count:=0;
@y  18148
for z:=0 to hyph_size do
  begin hyph_word[z]:=0; hyph_list[z]:=null; hyph_link[z]:=0;
  end;
hyph_count:=0;
hyph_next:=hyph_prime+1; if hyph_next>hyph_size then hyph_next:=hyph_prime;
@z

%%%%%%%% dynamic hyph_size
@x 18163 m.930
h:=hc[1]; incr(hn); hc[hn]:=cur_lang;
for j:=2 to hn do h:=(h+h+hc[j]) mod hyph_size;
loop@+  begin @<If the string |hyph_word[h]| is less than \(hc)|hc[1..hn]|,
    |goto not_found|; but if the two strings are equal,
    set |hyf| to the hyphen positions and |goto found|@>;
  if h>0 then decr(h)@+else h:=hyph_size;
  end;
not_found: decr(hn)
@y  18170
h:=hc[1]; incr(hn); hc[hn]:=cur_lang;
for j:=2 to hn do h:=(h+h+hc[j]) mod hyph_prime;
loop@+  begin @<If the string |hyph_word[h]| is less than \(hc)|hc[1..hn]|,
    |goto not_found|; but if the two strings are equal,
    set |hyf| to the hyphen positions and |goto found|@>;
  h:=hyph_link[h]; if h=0 then goto not_found;
  decr(h);
  end;
not_found: decr(hn)
@z

%%%%%%%% dynamic hyph_size
@x 18172 m.931
@ @<If the string |hyph_word[h]| is less than \(hc)...@>=
k:=hyph_word[h]; if k=0 then goto not_found;
if length(k)<hn then goto not_found;
if length(k)=hn then
  begin j:=1; u:=str_start(k);
  repeat if so(str_pool[u])<hc[j] then goto not_found;
  if so(str_pool[u])>hc[j] then goto done;
  incr(j); incr(u);
  until j>hn;
  @<Insert hyphens as specified in |hyph_list[h]|@>;
  decr(hn); goto found;
  end;
done:
@y  18184
@ @<If the string |hyph_word[h]| is less than \(hc)...@>=
{This is now a simple hash list, not an ordered one, so
the module title is no longer descriptive.}
k:=hyph_word[h]; if k=0 then goto not_found;
if length(k)=hn then
  begin j:=1; u:=str_start(k);
  repeat
  if so(str_pool[u])<>hc[j] then goto done;
  incr(j); incr(u);
  until j>hn;
  @<Insert hyphens as specified in |hyph_list[h]|@>;
  decr(hn); goto found;
  end;
done:
@z

% The GNU C compiler complains of unused variables.
@x
@!s,@!t:str_number; {strings being compared or stored}
@y
@!s:str_number; {strings being compared or stored}
@z

%%%%%%%% dynamic hyph_size
@x 18274 m.939
  begin h:=(h+h+hc[j]) mod hyph_size;
@y  18274
  begin h:=(h+h+hc[j]) mod hyph_prime;
@z

%%%%%%%% dynamic hyph_size
@x 18281 m.940
@ @<Insert the \(p)pair |(s,p)|...@>=
if hyph_count=hyph_size then overflow("exception dictionary",hyph_size);
@:TeX capacity exceeded exception dictionary}{\quad exception dictionary@>
incr(hyph_count);
while hyph_word[h]<>0 do
  begin @<If the string |hyph_word[h]| is less than \(or)or equal to
  |s|, interchange |(hyph_word[h],hyph_list[h])| with |(s,p)|@>;
  if h>0 then decr(h)@+else h:=hyph_size;
  end;
hyph_word[h]:=s; hyph_list[h]:=p
@y  18290
@ @<Insert the \(p)pair |(s,p)|...@>=
  if hyph_next <= hyph_prime then
     while (hyph_next>0) and (hyph_word[hyph_next-1]>0) do decr(hyph_next);
if (hyph_count=hyph_size)or(hyph_next=0) then
   overflow("exception dictionary",hyph_size);
@:TeX capacity exceeded exception dictionary}{\quad exception dictionary@>
incr(hyph_count);
while hyph_word[h]<>0 do
  begin @<If the string |hyph_word[h]| is less than \(or)or equal to
  |s|, interchange |(hyph_word[h],hyph_list[h])| with |(s,p)|@>;
  if hyph_link[h]=0 then
  begin
    hyph_link[h]:=hyph_next;
    if hyph_next >= hyph_size then hyph_next:=hyph_prime;
    if hyph_next > hyph_prime then incr(hyph_next);
  end;
  h:=hyph_link[h]-1;
  end;

found: hyph_word[h]:=s; hyph_list[h]:=p
@z

%%%%%%%% dynamic hyph_size
@x 18292 m.941
@ @<If the string |hyph_word[h]| is less than \(or)...@>=
k:=hyph_word[h];
if length(k)<length(s) then goto found;
if length(k)>length(s) then goto not_found;
u:=str_start(k); v:=str_start(s);
repeat if str_pool[u]<str_pool[v] then goto found;
if str_pool[u]>str_pool[v] then goto not_found;
incr(u); incr(v);
until u=str_start(k+1);
found:q:=hyph_list[h]; hyph_list[h]:=p; p:=q;@/
t:=hyph_word[h]; hyph_word[h]:=s; s:=t;
not_found:
@y  18303
@ @<If the string |hyph_word[h]| is less than \(or)...@>=
{This is now a simple hash list, not an ordered one, so
the module title is no longer descriptive.}
k:=hyph_word[h];
if length(k)<>length(s) then goto not_found;
u:=str_start(k); v:=str_start(s);
repeat if str_pool[u]<>str_pool[v] then goto not_found;
incr(u); incr(v);
until u=str_start(k+1);
{repeat hyphenation exception; flushing old data}
flush_string; s:=hyph_word[h]; {avoid |slow_make_string|!}
decr(hyph_count);
{ We could also |flush_list(hyph_list[h]);|, but it interferes
  with \.{trip.log}. }
goto found;
not_found:
@z

@x [43.943] l.18346 - web2c can't parse negative lower bounds in arrays.  Sorry.
@!init@! trie_op_hash:array[-trie_op_size..trie_op_size] of 0..trie_op_size;
@y
@!init@! trie_op_hash:array[neg_trie_op_size..trie_op_size] of 0..trie_op_size;
@z

@x [43.944] l.18365 - web2c can't parse negative lower bounds in arrays.  Sorry
var h:-trie_op_size..trie_op_size; {trial hash location}
@y
var h:neg_trie_op_size..trie_op_size; {trial hash location}
@z

@x [43.944] l.18370 - Another casting problem, and use |neg_trie_op_size|.
begin h:=abs(n+313*d+361*v+1009*cur_lang) mod (trie_op_size+trie_op_size)
  - trie_op_size;
@y
begin h:=abs(intcast(n)+313*intcast(d)+361*intcast(v)+1009*intcast(cur_lang))
  mod (trie_op_size - neg_trie_op_size)
  + neg_trie_op_size;
@z

@x [43.947] l.18438 - Dynamically allocate arrays, and a casting problem.
@!init @!trie_c:packed array[trie_pointer] of ASCII_code;
  {characters to match}
@t\hskip10pt@>@!trie_o:packed array[trie_pointer] of quarterword;
  {operations to perform}
@t\hskip10pt@>@!trie_l:packed array[trie_pointer] of trie_pointer;
  {left subtrie links}
@t\hskip10pt@>@!trie_r:packed array[trie_pointer] of trie_pointer;
  {right subtrie links}
@t\hskip10pt@>@!trie_ptr:trie_pointer; {the number of nodes in the trie}
@t\hskip10pt@>@!trie_hash:packed array[trie_pointer] of trie_pointer;
  {used to identify equivalent subtries}
tini
@y
@!init @!trie_c:^ASCII_code;
  {characters to match}
@t\hskip10pt@>@!trie_o:^quarterword;
  {operations to perform}
@t\hskip10pt@>@!trie_l:^trie_pointer;
  {left subtrie links}
@t\hskip10pt@>@!trie_r:^trie_pointer;
  {right subtrie links}
@t\hskip10pt@>@!trie_ptr:trie_pointer; {the number of nodes in the trie}
@t\hskip10pt@>@!trie_hash:^trie_pointer;
  {used to identify equivalent subtries}
tini
@z

@x [43.948] l.18471 - Another casting problem.
begin h:=abs(trie_c[p]+1009*trie_o[p]+@|
    2718*trie_l[p]+3142*trie_r[p]) mod trie_size;
@y
begin h:=abs(intcast(trie_c[p])+1009*intcast(trie_o[p])+@|
    2718*intcast(trie_l[p])+3142*intcast(trie_r[p])) mod trie_size;
@z

@x [43.590] l.18524 - Dynamically allocate & larger tries.
@!init@!trie_taken:packed array[1..trie_size] of boolean;
  {does a family start here?}
@t\hskip10pt@>@!trie_min:array[ASCII_code] of trie_pointer;
  {the first possible slot for each character}
@t\hskip10pt@>@!trie_max:trie_pointer; {largest location used in |trie|}
@t\hskip10pt@>@!trie_not_ready:boolean; {is the trie still in linked form?}
tini
@y
@!init@!trie_taken: ^boolean;
  {does a family start here?}
@t\hskip10pt@>@!trie_min:^trie_pointer;
  {the first possible slot for each character}
@t\hskip10pt@>@!trie_max:trie_pointer; {largest location used in |trie|}
@t\hskip10pt@>@!trie_not_ready:boolean; {is the trie still in linked form?}
tini
@z

@x [43.951] l.18539 - Dynamically allocate.
trie_not_ready:=true; trie_root:=0; trie_c[0]:=si(0); trie_ptr:=0;
@y
trie_not_ready:=true;
@z

%%
%% We can rewrite the original code after "main_loop_move+2" upto the
%% "tail_append(lig_stack)" in module 1036 as
%%
%
% main_loop_move+2:
% if font_bc[main_f]<=cur_chr then
%  if cur_chr<=font_ec[main_f] then
%    begin  main_i:=char_info(main_f)(cur_l);
%    if char_exists(main_i) goto main_loop_move+3;
%    end;
% char_warning(main_f,cur_chr); free_avail(lig_stack); goto big_switch;
% main_loop_move+3:
% tail_append(lig_stack) {|main_loop_lookahead| is next}
%

@x [49.1252] l.23230 - INI = VIR, so have to do runtime test.
hyph_data: if cur_chr=1 then
    begin @!init new_patterns; goto done;@;@+tini@/
@y  23215
hyph_data: if cur_chr=1 then
    begin @!init if ini_version then
      begin new_patterns; goto done; end; @;@+tini@/
@z

@x
@!flushable_string:str_number; {string not yet referenced}
@y
@z

% undo Knuth's change because
%   a) the string is already replaced in |scan_file_name| and therefore
%   b) the wrong string will get flushed!!!
%
@x [49.1260] l.23331 new_font: string recycling -- already done
flushable_string:=str_ptr-1;
@y
@z

% If you don't want to remove code with the following change,
% please replace the former change by
%
% @x
% flushable_string:=str_ptr-1;
% @y
% if cur_name=str_ptr-1 then
%   flushable_string:=str_ptr-1
% else
%   flushable_string:=str_ptr;  {number of a non-existing}
% @z
%
% otherwise the wrong string will get removed by |flush_string|!!
%
@x [49.1260] l.23334 new_font: string recycling -- already done
    begin if cur_name=flushable_string then
      begin flush_string; cur_name:=font_name(f);
      end;
    if s>0 then
@y
    begin if s>0 then
@z

@x [49.1265] if batchmode, mktex... scripts should be silent.
interaction:=cur_chr;
@y
interaction:=cur_chr;
if interaction = batch_mode
then kpse_make_tex_discard_errors := 1
else kpse_make_tex_discard_errors := 0;
@z

@x [49.1275] l.23441 - Same stuff as for \input, this time for \openin.
  if cur_ext="" then cur_ext:=".tex";
  pack_cur_name;
  if a_open_in(read_file[n]) then read_open[n]:=just_open;
@y
  pack_cur_name;
  tex_input_type:=0; {Tell |open_input| we are \.{\\openin}.}
  if open_in_name_ok(name_of_file+1)
      and a_open_in(read_file[n], kpse_tex_format) then
     begin k:=1;
     name_in_progress:=true;
     begin_name;
     while (k<=name_length)and(more_name(name_of_file[k])) do
       incr(k);
     end_name;
     name_in_progress:=false;
     read_open[n]:=just_open;
     end;
@z

@x [50.1301] l.23679 - INI = VIR, so runtime test.
format_ident:=" (INIOMEGA)";
@y
if ini_version then format_ident:=" (INIOMEGA)";
@z

% The GNU C compiler complains of unused variables.
@x [50.1302] l.23690 - Eliminate now-unused variable `w' in `store_fmt_file'.
var j,@!k,@!l:integer; {all-purpose indices}
@!p,@!q: pointer; {all-purpose pointers}
@!x: integer; {something to dump}
@!w: four_quarters; {four ASCII codes}
@y
var @!k:integer; {all-purpose indices}
@!p,@!q: pointer; {all-purpose pointers}
@!x: integer; {something to dump}
@z

@x [50.1303] l.23722 - Ditto, for `load_fmt_file'.
@!w: four_quarters; {four ASCII codes}
@y
@z

@x [50.1305] l.23751 - Do dumping and undumping of fmt files in C.
@d dump_wd(#)==begin fmt_file^:=#; put(fmt_file);@+end
@d dump_int(#)==begin fmt_file^.int:=#; put(fmt_file);@+end
@d dump_hh(#)==begin fmt_file^.hh:=#; put(fmt_file);@+end
@d dump_qqqq(#)==begin fmt_file^.qqqq:=#; put(fmt_file);@+end
@y
@z
@x [1306]
@d undump_wd(#)==begin get(fmt_file); #:=fmt_file^;@+end
@d undump_int(#)==begin get(fmt_file); #:=fmt_file^.int;@+end
@d undump_hh(#)==begin get(fmt_file); #:=fmt_file^.hh;@+end
@d undump_qqqq(#)==begin get(fmt_file); #:=fmt_file^.qqqq;@+end
@y
@z
@x [still 1306] debug format file
@d undump_size_end_end(#)==too_small(#)@+else undump_end_end
@y
@d format_debug_end(#)==
    write_ln (stderr, ' = ', #);
  end;
@d format_debug(#)==
  if debug_format_file then begin
    write (stderr, 'fmtdebug:', #);
    format_debug_end
@d undump_size_end_end(#)==
  too_small(#)@+else format_debug (#)(x); undump_end_end
@z

@x [50,1307] l.23779 - texarray
dump_int(@$);@/
@y
dump_int(@$);@/
dump_int(max_halfword);@/
@z

%%%%%%%% dynamic hyph_size
@x 23784 m.1307
dump_int(hyph_size)
@y  23784
dump_int(hyph_prime)
@z

@x [50.1308] l.23793 - texarray
x:=fmt_file^.int;
if x<>@$ then goto bad_fmt; {check that strings are the same}
undump_int(x);
if x<>mem_bot then goto bad_fmt;
undump_int(x);
if x<>mem_top then goto bad_fmt;
@y
@+init
if ini_version then
  begin libc_free(str_pool); libc_free(str_start_ar); libc_free(yzmem);
  end;
@+tini
undump_int(x);
format_debug('string pool checksum')(x);
if x<>@$ then goto bad_fmt; {check that strings are the same}
undump_int(x);
if x<>max_halfword then goto bad_fmt; {check |max_halfword|}

undump_int(x); format_debug ('mem_bot')(x);
if x<>mem_bot then goto bad_fmt;
undump_int(mem_top); format_debug ('mem_top')(mem_top);
if mem_bot+1100>mem_top then goto bad_fmt;

 
head:=contrib_head; tail:=contrib_head;
     page_tail:=page_head;  {page initialization}

mem_min := mem_bot - extra_mem_bot;
mem_max := mem_top + extra_mem_top;

yzmem:=xmalloc_array (memory_word, mem_max - mem_min);
zmem := yzmem - mem_min;   {this pointer arithmetic fails with some compilers}
mem := zmem;
@z

%%%%%%%% dynamic hyph_size
@x 23804 m.1308
if x<>hyph_size then goto bad_fmt
@y  23804
if x<>hyph_prime then goto bad_fmt
@z

% [1309] Make dumping/undumping more efficient by doing whole arrays at
% a time, via fread/fwrite in texmfmp.c.
@x [50.1309] l.23814 - Make dumping/undumping more efficient.
for k:=too_big_char to str_ptr do dump_int(str_start(k));
k:=0;
while k+4<pool_ptr do
  begin dump_four_ASCII; k:=k+4;
  end;
k:=pool_ptr-4; dump_four_ASCII;
@y
dump_things(str_start(too_big_char), str_ptr-too_big_char+1);
dump_things(str_pool[0], pool_ptr);
@z

@x [50.1310] l.23829 - Make dumping/undumping more efficient.
undump_size(0)(pool_size)('string pool size')(pool_ptr);
undump_size(0)(max_strings)('max strings')(str_ptr);
for k:=too_big_char to str_ptr do undump(0)(pool_ptr)(str_start(k));
k:=0;
while k+4<pool_ptr do
  begin undump_four_ASCII; k:=k+4;
  end;
k:=pool_ptr-4; undump_four_ASCII;
@y
undump_size(0)(sup_pool_size-pool_free)('string pool size')(pool_ptr);
if pool_size<pool_ptr+pool_free then
  pool_size:=pool_ptr+pool_free;
undump_size(0)(sup_max_strings-strings_free)('sup strings')(str_ptr);@/
if max_strings<str_ptr+strings_free then
  max_strings:=str_ptr+strings_free;
str_start_ar:=xmalloc_array(pool_pointer, max_strings-biggest_char);
undump_things(str_start(too_big_char), str_ptr-too_big_char+1);
str_pool:=xmalloc_array(packed_ASCII_code, pool_size);
undump_things(str_pool[0], pool_ptr);
@z

@x [50.1311] l.23850 - Make dumping/undumping more efficient.
repeat for k:=p to q+1 do dump_wd(mem[k]);
@y
repeat dump_things(mem[p], q+2-p);
@z

@x [50.1311] l.23855 - Make dumping/undumping more efficient.
for k:=p to lo_mem_max do dump_wd(mem[k]);
@y
dump_things(mem[p], lo_mem_max+1-p);
@z

@x [50.1311] l.23858 - Make dumping/undumping more efficient.
for k:=hi_mem_min to mem_end do dump_wd(mem[k]);
@y
dump_things(mem[hi_mem_min], mem_end+1-hi_mem_min);
@z

@x [50.1312] l.23873 - Make dumping/undumping more efficient.
repeat for k:=p to q+1 do undump_wd(mem[k]);
@y
repeat undump_things(mem[p], q+2-p);
@z

@x [50.1312] l.23878 - Make dumping/undumping more efficient.
for k:=p to lo_mem_max do undump_wd(mem[k]);
@y
undump_things(mem[p], lo_mem_max+1-p);
@z

@x [50.1312] l.23888 - Make dumping/undumping more efficient.
for k:=hi_mem_min to mem_end do undump_wd(mem[k]);
@y
undump_things (mem[hi_mem_min], mem_end+1-hi_mem_min);
@z

%%%%%%%% dynamic hyph_size
@x 24058 m.1324
dump_int(hyph_count);
for k:=0 to hyph_size do if hyph_word[k]<>0 then
  begin dump_int(k); dump_int(hyph_word[k]); dump_int(hyph_list[k]);
  end;
@y  24061
dump_int(hyph_count);
if hyph_next <= hyph_prime then hyph_next:=hyph_size;
dump_int(hyph_next);{minumum value of |hyphen_size| needed}
for k:=0 to hyph_size do if hyph_word[k]<>0 then
  begin dump_int(k+65536*hyph_link[k]);
        {assumes number of hyphen exceptions does not exceed 65535}
   dump_int(hyph_word[k]); dump_int(hyph_list[k]);
  end;
@z

@x [50.1324] l.24066 - Make dumping/undumping more efficient - trie
for k:=0 to trie_max do dump_hh(trie[k]);
dump_int(max_hyph_char);
dump_int(trie_op_ptr);
for k:=1 to trie_op_ptr do
  begin dump_int(hyf_distance[k]);
  dump_int(hyf_num[k]);
  dump_int(hyf_next[k]);
  end;
@y
dump_things(trie[0],trie_max+1);
dump_int(max_hyph_char);
dump_int(trie_op_ptr);
dump_things(hyf_distance[1], trie_op_ptr);
dump_things(hyf_num[1], trie_op_ptr);
dump_things(hyf_next[1], trie_op_ptr);
@z

@x [50.1325] l.24086 - Make dumping/undumping more efficient - trie
@<Undump the hyphenation tables@>=
@y
{This is only used for the hyphenation tries below, and the size is
 always |j+1|.}
@d xmalloc_and_undump(#) ==
  if not # then #:=xmalloc_array(two_halves, j+1);
  undump_things(#[0], j+1);

@<Undump the hyphenation tables@>=
@z

%%%%%%%% dynamic hyph_size
@x 24087 m.1325
undump(0)(hyph_size)(hyph_count);
for k:=1 to hyph_count do
  begin undump(0)(hyph_size)(j);
  undump(0)(str_ptr)(hyph_word[j]);
  undump(min_halfword)(max_halfword)(hyph_list[j]);
  end;
@y  24092
undump_size(0)(hyph_size)('hyph_size')(hyph_count);
undump_size(hyph_prime)(hyph_size)('hyph_size')(hyph_next);
j:=0;
for k:=1 to hyph_count do
  begin undump_int(j); if j<0 then goto bad_fmt;
   if j>65535 then
   begin hyph_next:= j div 65536; j:=j - hyph_next * 65536; end
       else hyph_next:=0;
   if (j>=hyph_size)or(hyph_next>hyph_size) then goto bad_fmt;
   hyph_link[j]:=hyph_next;
  undump(0)(str_ptr)(hyph_word[j]);
  undump(min_halfword)(max_halfword)(hyph_list[j]);
  end;
  {|j| is now the largest occupied location in |hyph_word|}
  incr(j);
  if j<hyph_prime then j:=hyph_prime;
  hyph_next:=j;
  if hyph_next >= hyph_size then hyph_next:=hyph_prime else
  if hyph_next >= hyph_prime then incr(hyph_next);
@z


@x [50.1325] l.24094 - Make dumping/undumping more efficient - trie
for k:=0 to j do undump_hh(trie[k]);
undump_int(max_hyph_char);
undump_size(0)(trie_op_size)('trie op size')(j); @+init trie_op_ptr:=j;@+tini
for k:=1 to j do
  begin undump(0)(63)(hyf_distance[k]); {a |small_number|}
  undump(0)(63)(hyf_num[k]);
  undump(min_quarterword)(max_quarterword)(hyf_next[k]);
  end;
@y
{The first array has not been allocated yet unless we're \.{INITEX};
 we do that precisely so we don't allocate more space than necessary.}
xmalloc_and_undump(trie);
undump_int(max_hyph_char);
undump_size(0)(trie_op_size)('trie op size')(j); @+init trie_op_ptr:=j;@+tini
{I'm not sure we have such a strict limitation (64) on these values, so
 let's leave them unchecked.}
undump_things(hyf_distance[1], j);
undump_things(hyf_num[1], j);
undump_upper_check_things(max_quarterword, hyf_next[1], j);
@z

@x [50.1327] l.24117 - Allow command line to override dumped value.
undump(batch_mode)(error_stop_mode)(interaction);
@y
undump(batch_mode)(error_stop_mode)(interaction);
if interaction_option<>unspecified_mode then interaction:=interaction_option;
@z

% [50.1327] As with TFM files, `eof' here means `have we previously
% encountered the end-of-file', not `are we at the end of the file'.
@x [50.1327] l.24120 - dumping/undumping, replace eof() by feof()
if (x<>69069)or eof(fmt_file) then goto bad_fmt
@y
if (x<>69069)or feof(fmt_file) then goto bad_fmt
@z

@x [50.1328] l.24124 - Eliminate probably wrong word `preloaded' from format_idents.
print(" (preloaded format="); print(job_name); print_char(" ");
@y
print(" (format="); print(job_name); print_char(" ");
@z

@x [51.1332] l.24203 - make the main program a procedure, for eqtb hack.
@p begin @!{|start_here|}
@y
@d const_chk(#)==begin if # < inf@&# then # := inf@&# else
                         if # > sup@&# then # := sup@&# end

{|setup_bound_var| stuff duplicated in \.{mf.ch}.}
@d setup_bound_var(#)==bound_default:=#; setup_bound_var_end
@d setup_bound_var_end(#)==bound_name:=#; setup_bound_var_end_end
@d setup_bound_var_end_end(#)==
  setup_bound_variable(address_of(#), bound_name, bound_default);

@p procedure main_body;
begin @!{|start_here|}

{Always start the file recorder?}
  recorder_enabled:=true;

{Bounds that may be set from the configuration file. We want the user to
 be able to specify the names with underscores, but \.{TANGLE} removes
 underscores, so we're stuck giving the names twice, once as a string,
 once as the identifier. How ugly.}
  setup_bound_var (1000000)('main_memory')(main_memory);
    {|memory_word|s for |mem| in \.{INITEX}}
  setup_bound_var (0)('extra_mem_top')(extra_mem_top);
    {increase high mem in \.{VIRTEX}}
  setup_bound_var (0)('extra_mem_bot')(extra_mem_bot);
    {increase low mem in \.{VIRTEX}}
  setup_bound_var (300000)('pool_size')(pool_size);
  setup_bound_var (200000)('string_vacancies')(string_vacancies);
  setup_bound_var (5000)('pool_free')(pool_free); {min pool avail after fmt}
  setup_bound_var (100000)('max_strings')(max_strings);
  setup_bound_var (100)('strings_free')(strings_free);
  setup_bound_var (20000)('trie_size')(trie_size);
    {if |ssup_trie_size| increases, recompile}
  setup_bound_var (659)('hyph_size')(hyph_size);
  setup_bound_var (20000)('buf_size')(buf_size);
  setup_bound_var (50)('nest_size')(nest_size);
  setup_bound_var (15)('max_in_open')(max_in_open);
  setup_bound_var (60)('param_size')(param_size);
  setup_bound_var (4000)('save_size')(save_size);
  setup_bound_var (300)('stack_size')(stack_size);
  setup_bound_var (16384)('dvi_buf_size')(dvi_buf_size);
  setup_bound_var (79)('error_line')(error_line);
  setup_bound_var (50)('half_error_line')(half_error_line);
  setup_bound_var (79)('max_print_line')(max_print_line);
  const_chk (main_memory);
@+init
  if ini_version then begin
    extra_mem_top := 0;
    extra_mem_bot := 0;
  end;
@+tini
  if extra_mem_bot>sup_main_memory then extra_mem_bot:=sup_main_memory;
  if extra_mem_top>sup_main_memory then extra_mem_top:=sup_main_memory;
  mem_top := mem_bot + main_memory;
  mem_min := mem_bot;
  mem_max := mem_top;

  {Check other constants against their sup and inf.}
  const_chk (trie_size);
  const_chk (hyph_size);
  const_chk (buf_size);
  const_chk (nest_size);
  const_chk (max_in_open);
  const_chk (param_size);
  const_chk (save_size);
  const_chk (stack_size);
  const_chk (dvi_buf_size);
  const_chk (pool_size);
  const_chk (string_vacancies);
  const_chk (pool_free);
  const_chk (max_strings);
  const_chk (strings_free);
  if error_line > ssup_error_line then error_line := ssup_error_line;

  {array memory allocation}
  buffer:=xmalloc_array (ASCII_code, buf_size);
  nest:=xmalloc_array (list_state_record, nest_size);
  save_stack:=xmalloc_array (memory_word, save_size);
  input_stack:=xmalloc_array (in_state_record, stack_size);
  input_file:=xmalloc_array (alpha_file, max_in_open);
  input_file_mode:=xmalloc_array (halfword, max_in_open);
  input_file_translation:=xmalloc_array (halfword, max_in_open);
  line_stack:=xmalloc_array (integer, max_in_open);
  param_stack:=xmalloc_array (halfword, param_size);
  dvi_buf:=xmalloc_array (real_eight_bits, dvi_buf_size);
  hyph_word:=xmalloc_array (str_number, hyph_size);
  hyph_list:=xmalloc_array (halfword, hyph_size);
  hyph_link:=xmalloc_array (hyph_pointer, hyph_size);
@+init
if ini_version then begin
  yzmem:=xmalloc_array (memory_word, mem_top - mem_bot);
  zmem := yzmem - mem_bot;   {Some compilers require |mem_bot=0|}

  str_start_ar:=xmalloc_array (pool_pointer, max_strings-biggest_char);
  str_pool:=xmalloc_array (packed_ASCII_code, pool_size);
end;
@+tini
@z

@x [51.1332] l.24215 - INI = VIR, so pool init needs runtime test
@!init if not get_strings_started then goto final_end;
init_prim; {call |primitive| for each primitive}
init_str_ptr:=str_ptr; init_pool_ptr:=pool_ptr; fix_date_and_time;
tini@/
@y
@+init if ini_version then
  begin if not get_strings_started then goto final_end;
  init_prim; {call |primitive| for each primitive}
  init_str_ptr:=str_ptr; init_pool_ptr:=pool_ptr; fix_date_and_time;
  end;
@+tini@/
@z

@x [51.1332] l.24225 - main
end_of_TEX: close_files_and_terminate;
final_end: ready_already:=0;
end.
@y
close_files_and_terminate;
final_end: do_final_end;
end {|main_body|};
@z

@x [51.1333] l.24254 - Print new line before termination; switch to editor if necessary.
    slow_print(log_name); print_char(".");
    end;
  end;
@y
    slow_print(log_name); print_char(".");
    end;
  end;
print_ln;
if (edit_name_start<>0) and (interaction>batch_mode) then
  call_edit(str_pool,edit_name_start,edit_name_length,edit_line);
@z

@x [51.1334] l. - Remove reference to fontmemsize
  wlog_ln(', out of ',font_mem_size:1,' for ',font_max-font_base:1);@/
@y
@z

@x [51.1335] l.24335 - Only do dump if ini.
  begin @!init for c:=top_mark_code to split_bot_mark_code do
    if cur_mark[c]<>null then delete_token_ref(cur_mark[c]);
  store_fmt_file; return;@+tini@/
@y
  begin @!init if ini_version then
    begin for c:=top_mark_code to split_bot_mark_code do
      if cur_mark[c]<>null then delete_token_ref(cur_mark[c]);
    store_fmt_file; return;
    end;@+tini@/
@z

@x [51.1337] l.24361 - Handle %&format in all cases.
if (format_ident=0)or(buffer[loc]="&") then
@y
if (format_ident=0)or(buffer[loc]="&")or dump_line then
@z

@x [51.1337] l.24371 - Allocate hyphenation tries.
fix_date_and_time;@/
@y
fix_date_and_time;@/

@!init
if trie_not_ready then begin {initex without format loaded}
  trie:=xmalloc_array (two_halves, trie_size);

  trie_c:=xmalloc_array (ASCII_code, trie_size);
  trie_o:=xmalloc_array (quarterword, trie_size);
  trie_l:=xmalloc_array (trie_pointer, trie_size);
  trie_r:=xmalloc_array (trie_pointer, trie_size);
  trie_hash:=xmalloc_array (trie_pointer, trie_size);
  trie_taken:=xmalloc_array (boolean, trie_size);
  trie_min:=xmalloc_array (trie_pointer, too_big_char);

  trie_root:=0; trie_c[0]:=si(0); trie_ptr:=0;
  end;
  tini@/

@z

% [52.1338] Core-dump in debugging mode on 0 input.  Under Unix, it's
% not possible to portably switch into the debugger while a program is
% running.  The best approximation is to do a core dump, then run the
% debugger on it later.
@x [52.1338] l.24411 - Core-dump in debugging mode on 0 input.
    begin goto breakpoint;@\ {go to every label at least once}
    breakpoint: m:=0; @{'BREAKPOINT'@}@\
    end
@y
    dump_core {do something to cause a core dump}
@z

% The GNU C compiler complains of unused variables.
@x
procedure do_extension;
var i,@!j,@!k:integer; {all-purpose integers}
@!p,@!q,@!r:pointer; {all-purpose pointers}
@y
procedure do_extension;
var @!k:integer; {all-purpose integers}
@!p:pointer; {all-purpose pointers}
@z

% [53.1350] (new_write_whatsit) Allow 18 as a \write stream. We never
% refer to an actual file, though, so we don't need to change the
% write_file or write_open arrays. We provide for disabling this at
% runtime, for paranoids.
@x [53.1350] l.24609 - system: Allow 18 as a \write stream.
  else if cur_val>15 then cur_val:=16;
@y
  else if (cur_val>15) and (cur_val <> 18) then cur_val:=16;
@z

@x [53.1370] l.24770 - system: (write_out) \write18{foo} => system(foo).
begin @<Expand macros in the token list
@y
@!d:integer; {number of characters in incomplete current string}
@!clobbered:boolean; {system string is ok?}
begin @<Expand macros in the token list
@z

@x [53.1370] l.24773 - system: (write_out) \write18{foo} => system(foo).
if write_open[j] then selector:=j
@y
if shell_enabled_p and (j=18) then
  begin selector := new_string;
  end
else if write_open[j] then selector:=j
@z

% Then call system(3) on that string.
@x [53.1370] l.24779 - system: (write_out) \write18{foo} => system(foo).
flush_list(def_ref); selector:=old_setting;
@y
flush_list(def_ref);
if j=18 then
  begin if (tracing_online<=0) then
    selector:=log_only  {Show what we're doing in the log file.}
  else selector:=term_and_log;  {Show what we're doing.}
  print_nl("system(");
  for d:=0 to cur_length-1 do
    begin {|print| gives up if passed |str_ptr|, so do it by hand.}
    print(so(str_pool[str_start(str_ptr)+d])); {N.B.: not |print_char|}
    end;
  print(")...");
  if shell_enabled_p then
    begin str_room(1); append_char(0); {Append a null byte to the expansion.}
    clobbered:=false;
    for d:=0 to cur_length-1 do {Convert to external character set.}
      begin str_pool[str_start(str_ptr)+d]:=xchr[str_pool[str_start(str_ptr)+d]];
      if (str_pool[str_start(str_ptr)+d]=null_code)
         and (d<cur_length-1) then clobbered:=true;
        {minimal checking: NUL not allowed in argument string of |system|()}
      end;
    if clobbered then print("clobbered")
    else begin {We have the string; run system(3). We don't have anything
            reasonable to do with the return status, unfortunately discard it.}
      for d:=0 to cur_length-1 do {Convert to external character set.}
        begin
        outside_string_array[d]:=xchr[str_pool[str_start(str_ptr)+d]];
        end;
      outside_string_array[cur_length]:=null_code;
      system(outside_string_array);
      print("executed");
      end;
    pool_ptr:=str_start(str_ptr);  {erase the string}
    end
  else begin print("disabled");
  end;
  print_char("."); print_nl(""); print_ln;
end;
selector:=old_setting;
@z

@x [53.1373] Need new local.
procedure out_what(@!p:pointer);
var j:small_number; {write stream number}
@y
procedure out_what(@!p:pointer);
var j:small_number; {write stream number}
    @!old_setting:0..max_selector;
@z

@x [53.1374] Disallow certain \openout filenames, and log results.
      while not a_open_out(write_file[j]) do
        prompt_file_name("output file name",".tex");
      write_open[j]:=true;
@y
      while not a_open_out(write_file[j]) 
            or not open_out_name_ok(name_of_file+1) do
        prompt_file_name("output file name",".tex");
      write_open[j]:=true;
      {If on first line of input, log file is not ready yet, so don't log.}
      if log_opened then begin
        old_setting:=selector;
        if (tracing_online<=0) then
          selector:=log_only  {Show what we're doing in the log file.}
        else selector:=term_and_log;  {Show what we're doing.}
        print_nl("\openout");
        print_int(j);
        print(" = `");
        print_file_name(cur_name,cur_area,cur_ext);
        print("'."); print_nl(""); print_ln;
        selector:=old_setting;
      end;
@z

@x [54.1376] l.24903 - Add editor-switch variables to globals.
@* \[54] System-dependent changes.
@y
@* \[54/web2c] System-dependent changes for Web2c.
Here are extra variables for Web2c.  (This numbering of the
system-dependent section allows easy integration of Web2c and e-\TeX, etc.)
@^<system dependencies@>

@<Glob...@>=
@!edit_name_start: pool_pointer; {where the filename to switch to starts}
@!edit_name_length,@!edit_line: integer; {what line to start editing at}
@!ipc_on: integer; {level of IPC action, 0 for none [default]}

@ The |edit_name_start| will be set to point into |str_pool| somewhere after
its beginning if \TeX\ is supposed to switch to an editor on exit.

@<Set init...@>=
edit_name_start:=0;

@ These are used when we regenerate the representation of the first 256
strings.

@<Global...@> =
@!save_str_ptr: str_number;
@!save_pool_ptr: pool_pointer;
@!shell_enabled_p: boolean;
@!output_comment: ^char;
@!k,l: 0..65535; {used by `Make the first 256 strings', etc.}

@ When debugging a macro package, it can be useful to see the exact
control sequence names in the format file.  For example, if ten new
csnames appear, it's nice to know what they are, to help pinpoint where
they came from.  (This isn't a truly ``basic'' printing procedure, but
that's a convenient module in which to put it.)

@<Basic printing procedures@> =
procedure print_csnames (hstart:integer; hfinish:integer);
var c,h:integer;
begin
  write_ln(stderr, 'fmtdebug:csnames from ', hstart, ' to ', hfinish, ':');
  for h := hstart to hfinish do begin
    if newtext(h) > 0 then begin {if have anything at this position}
      for c := str_start(newtext(h)) to str_start(newtext(h) + 1) - 1
      do begin
        put_byte(str_pool[c], stderr); {print the characters}
      end;
      write_ln(stderr, '|');
    end;
  end;
end;

@ Are we printing extra info as we read the format file?

@<Glob...@> =
@!debug_format_file: boolean;


@* \[54/web2c-string] The string recycling routines.  \TeX{} uses 2
upto 4 {\it new\/} strings when scanning a filename in an \.{\\input},
\.{\\openin}, or \.{\\openout} operation.  These strings are normally
lost because the reference to them are not saved after finishing the
operation.  |search_string| searches through the string pool for the
given string and returns either 0 or the found string number.

@<Declare additional routines for string recycling@>=
function search_string(@!search:str_number):str_number;
label found;
var result: str_number;
@!s: str_number; {running index}
@!len: integer; {length of searched string}
begin result:=0; len:=length(search);
if len=0 then  {trivial case}
  begin result:=""; goto found;
  end
else  begin s:=search-1;  {start search with newest string below |s|; |search>1|!}
  while s>65535 do  {first 65536 strings depend on implementation!!}
    begin if length(s)=len then
      if str_eq_str(s,search) then
        begin result:=s; goto found;
        end;
    decr(s);
    end;
  end;
found:search_string:=result;
end;

@ The following routine is a variant of |make_string|.  It searches
the whole string pool for a string equal to the string currently built
and returns a found string.  Otherwise a new string is created and
returned.  Be cautious, you can not apply |flush_string| to a replaced
string!

@<Declare additional routines for string recycling@>=
function slow_make_string : str_number;
label exit;
var s: str_number; {result of |search_string|}
@!t: str_number; {new string}
begin t:=make_string; s:=search_string(t);
if s>0 then
  begin flush_string; slow_make_string:=s; return;
  end;
slow_make_string:=t;
exit:end;


@* \[54] System-dependent changes.
@z
