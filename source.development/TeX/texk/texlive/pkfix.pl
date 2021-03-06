use strict;
$^W=1; # turn warning on
#
# pkfix.pl
#
# Copyright (C) 2001 Heiko Oberdiek.
#
# This program may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either version 1.2
# of this license or (at your option) any later version.
# The latest version of this license is in
#   http://www.latex-project.org/lppl.txt
# and version 1.2 or later is part of all distributions of LaTeX
# version 1999/12/01 or later.
#
# See file "README" for a list of files that belongs to this project.
#
# This file "pkfix.pl" may be renamed to "pkfix"
# for installation purposes.
#
my $file        = "pkfix.pl";
my $program     = uc($&) if $file =~ /^\w+/;
my $project     = lc($program);
my $version     = "0.8";
my $date        = "2001/04/23";
my $author      = "Heiko Oberdiek";
my $copyright   = "Copyright (c) 2001 by $author.";
#
# Reqirements: Perl5, dvips
# History:
#   2001/04/12 v0.1:
#     * First try.
#   2001/04/13 v0.2:
#     * TeX/dvips is called for each font for the case of errors.
#     * First release.
#   2001/04/15 v0.3:
#     * Call of kpsewhich with option --progname.
#     * Extracting of texps.pro from temporary PostScript file,
#       if kpsewhich failed.
#     * Option -G0 for dvips run added.
#   2001/04/16 v0.4:
#     * Support for merging PostScript fonts added.
#     * \special{!...}/@fedspecial detection added.
#     * Bug fix: I detection.
#   2001/04/17 v0.5:
#     * Redirection of stderr (dvips run) if possible.
#   2001/04/20 v0.6:
#     * Bug fix: dvips font names can contain numbers.
#   2001/04/21 v0.7:
#     * Bug fix: long dvi file name in ps file.
#   2001/04/23 v0.8:
#     * Bug fix: post string parsing.
#

### program identification
my $title = "$program $version, $date - $copyright\n";

### error strings
my $Error = "!!! Error:"; # error prefix
my $Warning = "!!! Warning:"; # warning prefix

### variables
my $envvar    = uc($project);
my $infile    = "";
my $outfile   = "";
my $texpsfile = "texps.pro";
my $prefix    = "_${project}_$$";
# my $prefix    = "_${project}_";
my $tempfile  = "$prefix";
my $texfile   = "$tempfile.tex";
my $dvifile   = "$tempfile.dvi";
my $logfile   = "$tempfile.log";
my $psfile    = "$tempfile.ps";
my $missfile  = "missfont.log";
my @cleanlist = ($texfile, $dvifile, $logfile, $psfile);
push(@cleanlist, $missfile) unless -f $missfile;

my $err_redirect = " 2>&1";
$err_redirect = "" if $^O =~ /dos/i ||
                      $^O =~ /os2/i ||
                      $^O =~ /mswin32/i ||
                      $^O =~ /cygwin/i;

my $x_resolution    = 0;
my $y_resolution    = 0;
my $blocks_found    = 0;
my $fonts_converted = 0;
my $fonts_merged    = 0;
my $fonts_misses    = 0;

### option variables
my @bool = ("false", "true");
$::opt_tex     = "tex";
$::opt_dvips   = "dvips";
$::opt_kpsewhich = "kpsewhich --progname $project";
$::opt_options = "-Ppdf -G0";
$::opt_help       = 0;
$::opt_quiet      = 0;
$::opt_debug      = 0;
$::opt_verbose    = 0;
$::opt_clean      = 1;

my $usage = <<"END_OF_USAGE";
${title}Syntax:   \L$program\E [options] <inputfile.ps> <outputfile.ps>
Function: This program tries to replace pk fonts in <inputfile.ps>
          by the type 1 versions. The result is written in <outputfile.ps>.
Options:                                                         (defaults:)
  --help            print usage
  --(no)quiet       suppress messages                            ($bool[$::opt_quiet])
  --(no)verbose     verbose printing                             ($bool[$::opt_verbose])
  --(no)debug       debug informations                           ($bool[$::opt_debug])
  --(no)clean       clear temp files                             ($bool[$::opt_clean])
  --tex texcmd      tex command name (plain format)              ($::opt_tex)
  --dvips dvipscmd  dvips command name                           ($::opt_dvips)
  --options opt     dvips options                                ($::opt_options)
END_OF_USAGE

### environment variable PKFIX
if ($ENV{$envvar}) {
  unshift(@ARGV, split(/\s+/, $ENV{$envvar}));
}

### process options
my @OrgArgv = @ARGV;
use Getopt::Long;
GetOptions(
  "help!",
  "quiet!",
  "debug!",
  "verbose!",
  "clean!",
  "tex=s",
  "dvips=s",
  "options=s"
) or die $usage;
!$::opt_help or die $usage;
@ARGV < 3 or die "$usage$Error Too many files!\n";
@ARGV == 2 or die "$usage$Error Missing file names!\n";

$::opt_quiet = 0 if $::opt_verbose;
$::opt_clean = 0 if $::opt_debug;

### get file names
$infile = $ARGV[0];
$outfile = $ARGV[1];

print $title unless $::opt_quiet;

print "*** input file: `$infile'\n" if $::opt_verbose;
print "*** output file: `$outfile'\n" if $::opt_verbose;

if ($::opt_debug) {
  print <<"END_DEB";
*** OSNAME: $^O
*** PERL_VERSION: $]
*** ARGV: @OrgArgv
END_DEB
}

### get texps.pro
my $texps_data   = 0;
my $texps_string = get_texps_pro();

### open input and output files
open(IN, $infile) or die "$Error Cannot open `$infile'!\n";
open(OUT, ">$outfile") or die "$Error Cannot write `$outfile'!\n";

##################################
# expected format:
#   ...
#   %%DVIPSParameters:... dpi=([\dx]+)...
#   ...
#   TeXDict begin \d+ \d+ \d+ \d+ \d+ \(\S+\)
#   @start ...
#   ...
#   %DVIPSBitmapFont: (\S+) (\S+) ([\d\.]+) (\d+)
#   /(\S+) ...
#   ...
#   %EndDVIPSBitmapFont
#   ...
#   ... end
#   %%EndProlog
#
# or if \special{!...} was used, the lines with TeXDict:
#   TeXdict begin @defspecial
#
#   ...
#
#   @fedspecial end TeXDict begin
#   \d+ \d+ \d+ \d+ \d+ \(\S+\) @start
#
# bitmap font:
# start:
#   %%DVIPSBitmapFont: {dvips font} {font name} {at x pt} {chars}
#   /{dvips font} {chars} {max. char number + 1} df
# character, variant a:
#   <{hex code}>{char number} D
# character, variant b:\
#   [<{hex code}>{num1} {num2} {num3} {num4} {num5} {char number} D
# end:
#   E
#   %%EndDVIPSBitmapFont
#
# type 1 font:
# before TeXDict line:
#   %%BeginFont: CMR10
#   ...
#   %%EndFont
# after @start:
#   /Fa ... /CMR10 rf
###################################

my $x_comment_resolution = 0;
my $y_comment_resolution = 0;
my $start_string = "";
my $post_string = "";
my $dvips_resolution = "";
my $texps_found = 0;
my @font_list = ();
my %font_txt = ();
my %font_count = ();
my %font_entry = ();

sub init {
  $x_comment_resolution = 0;
  $y_comment_resolution = 0;
  $x_resolution = 0;
  $y_resolution = 0;
  $start_string = "";
  $texps_found = 0;
  @font_list = ();
  %font_txt = ();
  %font_count = ();
  %font_entry = ();
}

init();

while (<IN>) {

  if (/^%%Creator: (dvips\S*) (\S+)\s/) {
    print "*** %%Creator: $1 $2\n" if $::opt_debug;
  }

  if (/^%DVIPSParameters:.*dpi=([\dx]+)/) {
    print OUT;
    my $str = $1;
    $x_comment_resolution = 0;
    $y_comment_resolution = 0;
    if ($str =~ /^(\d+)x(\d+)$/) {
      $x_comment_resolution = $1;
      $y_comment_resolution = $2;
    }
    if ($str =~ /^(\d+)$/) {
      $x_comment_resolution = $1;
      $y_comment_resolution = $1;
    }
    print "*** %DVIPSParameters: dpi=$str " .
          "(x=$x_comment_resolution, y=$y_comment_resolution)\n"
      if $::opt_debug;
    $x_comment_resolution > 0 && $y_comment_resolution > 0 or
      die "$Error Wrong resolution value " .
          "($x_comment_resolution x $y_comment_resolution)!\n";
    next;
  }

  if (/^%%BeginProcSet: texps.pro/) {
    $texps_found = 1;
    print "*** texps.pro found\n" if $::opt_debug;
  }

  if (/^TeXDict begin \@defspecial/) {
    print "*** \@defspecial found.\n" if $::opt_debug;
    $start_string = $_;
    while (<IN>) {
      $start_string .= $_;
      last if /\@fedspecial end TeXDict begin/;
    }
  }
  elsif (/^TeXDict begin \d+ \d+ \d+ \d+ \d+/) {
    print "*** TeXDict begin <5 nums> found.\n" if $::opt_debug;
    $start_string = $_;
  }
  if ($start_string ne "") {
    # look for @start
    unless (/\@start/) {
      while (<IN>) {
        $start_string .= $_;
        last if /\@start/;
      }
    }

    # divide post part
    $start_string =~ /^([\s\S]*\@start)\s*([\s\S]*)$/ or
      die "$Error Parse error (\@start)!\n";
    $start_string = "$1\n";
    $post_string = $2;
    $post_string =~ s/\s*$//;
    $post_string .= "\n" unless $post_string eq "";

    $start_string =~
      /\d+\s+\d+\s+\d+\s+(\d+)\s+(\d+)\s+\((.*)\)\s+\@start/ or
      die "$Error Parse error (\@start parameters)!\n";

    $blocks_found++;
    print "*** dvi file: $3\n" if $::opt_debug;

    # get and check resolution values
    $x_resolution = $1;
    $y_resolution = $2;
    print "*** resolution: $x_resolution x $y_resolution\n"
      if $::opt_debug;
    $x_comment_resolution > 0 or
      die "$Error Missing comment `%DVIPSParameters'!\n";
    $x_resolution == $x_comment_resolution &&
    $y_resolution == $y_comment_resolution or
      die "$Error Resolution values in comment and PostScript " .
          "does not match!\n";
    # setting dvips resolution option(s)
    if ($x_resolution == $y_resolution) {
      $dvips_resolution = "-D $x_resolution";
    }
    else {
      $dvips_resolution = "-X $x_resolution -Y $y_resolution";
    }

    while (<IN>) {
      if (/^%%EndProlog/) {
        $texps_data > 0 or die "$Error File `texps.pro' not found!\n";
        print OUT $texps_string unless $texps_found;
        foreach (@font_list) {
          my $fontname = $_;
          print "*** Adding font `$fontname'\n"
            if $::opt_debug;
          my ($dummy, $err);
          if ($font_count{$fontname} > 1) {
            $fonts_merged++;
            print "*** Merging font `$fontname' ($font_count{$fontname}).\n"
              unless $::opt_quiet;
            ($font_txt{$fontname}, $dummy, $err) =
              get_font($font_entry{$fontname});
            $err == 0 or die "$Error Cannot merge font `$fontname'!\n";
          }
          print OUT $font_txt{$fontname};
        }
        print OUT $start_string,
                  $post_string,
                  $_;
        print "*** %%EndProlog\n" if $::opt_debug;
        init();
        last;
      }

      if (/^%DVIPSBitmapFont: (\S+) (\S+) ([\d.]+) (\d+)/) {
        my $bitmap_string = $_;
        my $dvips_fontname = $1;
        my $fontname = $2;
        my $entry = "\\Font\{$1\}\{$2\}\{$3\}\{";
        print "*** Font $1: $2 at $3pt, $4 chars\n" if $::opt_verbose;
        my $line = "";
        my $num = -1;
        my $chars = $4;
        my $count = 0;
        while (<IN>) {
          $bitmap_string .= $_;
          last if /^%EndDVIPSBitmapFont/;
          chomp;
          $line .= " " . $_;
        }
        $line =~ s/<[0-9A-F ]*>/ /g;

        print "*** <Font> $line\n" if $::opt_debug;

        while ($line =~ /\s(\d+)\s+D(.*)/) {
          $num = $1;
          $count++;
          $entry .= "$num,";
          $line = $2;
          while ($line =~ /^[\s\d\[]*I(.*)/) {
            $num++;
            $count++;
            $entry .= "$num,";
            $line = $1;
          }
        }
        $chars == $count or
          die "$Error Parse error, $count chars of $chars found " .
            "($fontname)!\n";

        $entry =~ s/,$//;
        $entry .= "\}";

        print "*** Converting font `$fontname'.\n" unless $::opt_quiet;
        my ($font_part, $start_part, $err) = get_font($entry);
        if ($err == 0) {
          if (defined($font_count{$fontname})) {
            $font_count{$fontname}++;
            $font_entry{$fontname} .= "\n$entry";
          }
          else {
            push @font_list, $fontname;
            $font_txt{$fontname} = $font_part;
            $font_count{$fontname} = 1;
            $font_entry{$fontname} = $entry;
          }
          $start_part =~ s/\/Fa/\/$dvips_fontname/;
          $start_string .= $start_part;
          $fonts_converted++;
        }
        else {
          $start_string .= $bitmap_string;
          $fonts_misses++;
          print "!!! Font conversion of `$fontname' failed!\n";
        }

        next;
      }

      $post_string .= $_;
    }
    next;
  }

  print OUT;
}

close(IN);
close(OUT);

if ($::opt_clean) {
  print "*** clear temp files\n" if $::opt_verbose;
  foreach (@cleanlist) {
    unlink;
  }
}

if (!$::opt_quiet) {
  if ($blocks_found > 1) {
    print "==> $blocks_found blocks found.\n";
  }
  if ($fonts_misses) {
    print "==> $fonts_misses font conversion(s) failed.\n";
  }
  if ($fonts_converted) {
    print "==> $fonts_converted font(s) converted.\n";
    if ($fonts_merged) {
      print "==> $fonts_merged font(s) merged.\n";
    }
  }
  else {
    print "==> no fonts converted\n";
  }
}


# get type 1 font
# param:  $entry: font entry as TeX string
# return: $font:  font file as string
#         $start: font definition after @start
#         $err:   error indication
sub get_font {
  my $entry = $_[0];
  my $font = "";
  my $start = "";
  my $err = 0;
  my @err = ("", "", 1);
  local *OUT;
  local *IN;

  ### write temp tex file
  open(OUT, ">$texfile") or die "$Error Cannot write `$texfile'!\n";
  print OUT <<'TEX_HEADER';
\nonstopmode
\nopagenumbers
\def\Font#1#2#3#4{%
  \expandafter\font\csname font@#1\endcsname=#2 at #3pt\relax
  \csname font@#1\endcsname
  \hbox to 0pt{%
    \ScanChar#4,\NIL
    \hss
  }%
}
\def\ScanChar#1,#2\NIL{%
  \char#1\relax
  \ifx\\#2\\%
  \else
    \ReturnAfterFi{%
      \ScanChar#2\NIL
    }%
  \fi
}
\long\def\ReturnAfterFi#1\fi{\fi#1}
\noindent
TEX_HEADER

  print OUT "$entry\n\\bye\n";
  close(OUT);

  ### run tex
  {
    print "*** run TeX\n" if $::opt_verbose;

    my $cmd = "$::opt_tex $tempfile";
    print ">>> $cmd\n" if $::opt_verbose;
    my @capture = `$cmd`;
    if (!defined(@capture)) {
      print "$Warning Cannot execute TeX!\n";
      return @err;
    }
    if ($::opt_verbose) {
      print @capture;
    }
    else {
      foreach (@capture) {
        print if /^!\s/;
      }
    }
    if ($?) {
      my $exitvalue = $?;
      if ($exitvalue > 255) {
        $exitvalue >>= 8;
        print "$Warning Closing TeX (exit status: $exitvalue)!\n";
        return @err;
      }
      print "$Warning Closing TeX ($exitvalue)!\n";
      return @err;
    }
  }

  ### run dvips
  {
    print "*** run dvips\n" if $::opt_verbose;

    my $cmd = "$::opt_dvips $::opt_options $dvips_resolution $tempfile";
    print ">>> $cmd\n" if $::opt_verbose;
    # dvips writes on stderr :-(
    my @capture = `$cmd$err_redirect`;
    if ($::opt_verbose) {
      print @capture;
    }
    if ($?) {
      my $exitvalue = $?;
      if ($exitvalue > 255) {
        $exitvalue >>= 8;
        print "$Warning Closing dvips (exit status: $exitvalue)!\n";
        return @err;
      }
      print "$Warning Closing dvips ($exitvalue)!\n";
      return @err;
    }
  }

  ### get font and start part
  open(IN, $psfile) or die "$Error Cannot open `$psfile'!\n";

  while (<IN>) {
    if ($texps_data == 0 && /^%%BeginProcSet: texps.pro/) {
      $texps_string = $_;
      while (<IN>) {
        $texps_string .= $_;
        last if /^%%EndProcSet/;
      }
      $texps_data = 1;
      print "*** texps.pro extracted.\n" if $::opt_debug;
      next;
    }
    if (/^%%BeginFont:/) {
      $font .= $_;
      while (<IN>) {
        $font .= $_;
        last if /^%%EndFont/;
      }
      next;
    }
    if (/^\@start/) {
      s/^\@start\s*//;
      $start .= $_;
      while (<IN>) {
        last if /^%%EndProlog/;
        $start .= $_;
      }
      if (($start =~ s/\s*end\s*$/\n/) != 1) {
        $err = 1;
        print "$Warning Parse error, `end' not found!\n";
      }
      print "*** start: $start" if $::opt_debug;
      last;
    }
  }
  close(IN);

  if ($font eq "") {
    print "$Warning `%%BeginFont' not found!\n";
    return @err;
  }
  return ($font, $start, $err);
}


# get_texps_pro
# return: string with content of texps.pro
sub get_texps_pro {
  $texps_data = 0;
  # get file name
  my $backupWarn = $^W;
  $^W = 0;
  my $file = `$::opt_kpsewhich $texpsfile`;
  $^W = $backupWarn;
  if (!defined($file) or $file eq "") {
    print "$Warning: Cannot find `$texpsfile' with kpsewhich!\n"
      if $::opt_debug;
    return "";
  }
  chomp $file;
  print "*** texps.pro: $file\n" if $::opt_debug;

  # read file
  local *IN;
  open(IN, $file) or die "$Error: Cannot open `$file'!\n";
  my @lines = <IN>;
  @lines > 0 or die "$Error: Empty file `$file'!\n";
  chomp $lines[@lines-1];
  my $str = "%%BeginProcSet: texps.pro\n";
  $"="";
  $str .= "@lines\n";
  $"=" ";
  $str .= "%%EndProcSet\n";
  $texps_data = 1;
  return $str;
}

__END__
