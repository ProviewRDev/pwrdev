#!/usr/bin/perl
#
# pwre.pl
#

use POSIX;

%verbs = (
  "add", 	"add:usage_add",
  "configure", 	"configure:usage_configure",
  "build", 	"build:usage_build",
  "build_all", 	"build_module:usage_build_module",
  "build_module", "build_module:usage_build_module",
  "build_kernel", "build_kernel:usage_build_kernel",
  "build_all_modules", "build_all_modules:usage_build_all_modules",
  "build_all_wbl", "build_all_wbl:usage_build_all_wbl",
  "ebuild",    "ebuild:usage_ebuild",
  "clean_exe_all", "clean_exe_all:usage_clean_exe_all",
  "method_build", "method_build:usage_method_build",
  "copy", 	"copy:usage_copy",
  "create", 	"create:usage_create",
  "create_all_modules", "create_all_modules:usage_create_all_modules",
  "delete", 	"dele:usage_dele",
  "help",	"help:usage_help",
  "list",	"list:usage_list",
  "init",	"init:usage_init",
  "import",	"import:usage_import",
  "module",	"module:usage_module",
  "merge",	"merge:usage_merge",
  "modify",	"modify:usage_modify",
  "show",	"show:usage_show",
  "tags",	"tags:usage_tags",
  "_build", 	"_build:",
  "_exists", 	"_exists:",
  "_print",	"_print:"
);

#
# Variables
#
$user = $ENV{"USER"};

$ver    = 0;
$label  = $ENV{"pwre_env"};
$sroot	= "/view/$user/vobs/pwr_src/src";
$vmsinc = "";
$broot  = "/usr/users/$user/$user" . "_dbg";
$btype  = "dbg";
$os	= "linux";
if ($ENV{"pwre_hosttype"} eq "rs6000") {
  $hw	= "ppc";
} else {
  if ($ENV{"pwre_hosttype"} eq "x86_64") {
    $hw	= "x86_64";
  } else {
    $hw	= "x86";
  }
}
$real_hw = $hw;

$desc	= $user. "'s environment";
@vars;
$varstr;

if (($dbname = $ENV{"pwre_env_db"}) eq "") {
  $dbname = $ENV{"HOME"} . "/pwre/pwre";
}
$os = substr($ENV{"pwre_os"}, 3, 100);
$hw = substr($ENV{"pwre_hw"}, 3, 100);
$configfile = $ENV{"pwre_broot"} . "/pwre_" . $hw . "_" . $os . ".cnf";
$parallel = $ENV{"PWRE_CONF_PARALLEL"};

$arg1 = $ARGV[0];
shift;

if ($arg1 eq "") {
  help();
}

$amb = 0;
$found = 0;
@ambs;

foreach $key (sort keys (%verbs)) {
  if ($key =~ /^$arg1/) {
    if ($key =~ /$arg1\b/) {
      $amb = 0;
      $found = 1;
      ($verb, $usage)  =  split(/:/, $verbs{$key});
      last;
    }

    push(@ambs, $key);
    if ($found == 1) {
      $amb = 1;
    } else {
      $found = 1;
     ($verb, $usage)  =  split(/:/, $verbs{$key});
    }
  }
}

if ($amb == 1) {
  printf("++\n++ Verb \"%s\" is ambigous\n", $arg1);
  printf("++ Select one of: ");
  foreach (@ambs) {
    printf("%s ", $_);
  }
  print("\n");
  exit 1;
} elsif ($found == 1) {
  &$verb(@ARGV);
  exit 0;
} else {
  help();
  exit 1;
}

#
# Verb subroutines

#
# add()
#
sub add()
{
  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";
  $label = $_[0];
  $varstr = $envdb{$label};
  if ($varstr ne "") {
    printf("++ Environment %s already exists\n", $_[0]);
  } else {
    get_vars(@_);
    update_db();
  }
  untie(%envdb)|| die "++ can't untie $dbname!";
}

#
# configure()
#
sub configure()
{
  if (!defined($ENV{"pwre_env"})) {
    print("++ Environment is not initialized!\n");
    exit 1;
  }

  if ($hw eq "x86" && $real_hw eq "x86_64") {
    $ENV{"cross_compile"} = "-m32";
  }
  system($ENV{"pwre_bin"} . "/pwre_configure.sh"." ".$_[0]." ".$_[1]." ".$_[2]);
}

sub set_flavour # args: flavour
{
  # printf("Set flavour %s\n", $_[0]);
  if ($_[0] eq "qt") {
    $ENV{pwre_conf_qt} = "1";
  }
  else {
    delete $ENV{"pwre_conf_qt"};
  }
  if ($_[0] eq "gtk") {
    $ENV{"pwre_conf_gtk"} = "1";
  }
  else {
    delete $ENV{"pwre_conf_gtk"};
  }
}

#
# build()
#
sub build() # args: branch, subbranch, flavour, phase
{
  if (!defined($ENV{"pwre_env"})) {
    print("++ Environment is not initialized!\n");
    exit 1;
  }

  if (! -e $configfile ) {
    print("++ Environment is not configured!\n");
    exit 1;
  }

  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";

  read_vars();

  if ($_[0] eq "-v") {
    $ver = 1;
    shift(@_);
  }

  my($flavour);
  if ($_[0] eq "all") {
    $flavour = $_[1];
    build_all_modules($flavour);
    return;
  }

  if ($_[0] eq "module" || $_[0] eq "") {
    $flavour = $_[1];
    build_module($flavour);
    return;
  }

  my($branch) = $_[0];
  if (!defined($branch)) {
    usage_build();
    exit 1;
  }

  my($subbranch) = $_[1];
  if (!defined($subbranch)) {
    usage_build();
    exit 1;
  }

  shift(@_);
  shift(@_);

  if ($_[0] eq "") {
    $flavour = "src";
  } else {
    $flavour = $_[0];
    shift(@_);
  }
  set_flavour($flavour);

  printf("--\n");
  printf("-- Build\n");
  printf("-- Branch.............: %s\n", $branch);
  printf("-- Sub branch.........: %s\n", $subbranch);
  printf("-- Sub branch flavour.: %s\n", $flavour);
  printf("-- Args...............: ");
  foreach (@_) {
    printf("%s ", $_);
    $ENV{"PWRE_ARGS"} = $_;
  }
  printf("\n--\n");

  _build($branch, $subbranch, $flavour, @_);
}

#
# build_kernel()
#
sub build_kernel # args: flavour
{
  my $flavour = $_[0];

  _module("rt");
  build_module("copy", $flavour);
  merge();
  _module("xtt");
  build_module("copy", $flavour);
  merge();
  _module("wb");
  build_module("copy", $flavour);
  merge();
  _module("nmps");
  build_module("copy", $flavour);
  merge();
  _module("dataq");
  build_module("copy", $flavour);
  merge();
  _module("remote");
  build_module("copy", $flavour);
  merge();
  _module("sev");
  build_module("copy", $flavour);
  merge();
  _module("opc");
  build_module("copy", $flavour);
  merge();
  _module("rt");
  build_module("lib", $flavour);
  merge();
  _module("xtt");
  build_module("lib", $flavour);
  merge();
  _module("wb");
  build_module("lib", $flavour);
  merge();
  _module("otherio");
  _build("exp","rt","src","copy");
  _build("lib","usbio_dummy","src","init");
  _build("lib","usbio_dummy","src","lib");
  _build("lib","usb_dummy","src","init");
  _build("lib","usb_dummy","src","lib");
  _build("lib","cifx_dummy","src","init");
  _build("lib","cifx_dummy","src","lib");
  _build("lib","nodave_dummy","src","init");
  _build("lib","nodave_dummy","src","lib");
  _build("lib","epl_dummy","src","init");
  _build("lib","epl_dummy","src","lib");
  merge();
  _module("xtt");
  build_module("exe", $flavour);
  merge();
  _module("wb");
  build_module("exe", $flavour);
  merge();
  _module("rt");
  build_module("exe", $flavour);
  merge();
}

#
# ebuild()
#
sub ebuild # args: pass flavour
{
  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";

  read_vars();

  my $fname = $ENV{"pwre_bin"} . "/ebuild.dat";
  open FILE, $fname or die $!;

  my %to_build = ();

  printf("-- Building modules\n");
  while (<FILE>) {
    my @value = split(' ', $_);
    if ($value[1] eq "1") {
      $to_build{$value[0]} = 1;
      printf("--   " . $value[0] . "\n");
    }
  }
  printf("\n");
  close FILE;

  set_flavour("");
  my $pass = $_[0];
  my $flavour = $_[1];

  if ($pass eq "rt" ) {
    _module("rt");
    _build("exp", "inc", "src", "all");
    _build("exp", "com", "src", "all");
    _build("tools/exe", "*", "src", "all");
    merge("exe/tools_cmsg2c");
    merge("exe/tools_msg2cmsg");
    merge("exe/tools_pdrgen");
    _module("nmps");
    _build("lib","nmps","src","copy");
    merge();
    _module("dataq");
    _build("lib","dataq","src","copy");
    merge();
    _module("rt");
    _build("msg", "*", "src", "all");
    _build("lib", "rt", "src", "init");
    _build("lib", "rt", "src", "copy");
#    _build("lib", "rt", $flavour, "copy");
    _build("lib", "co", "src", "init");
    _build("lib", "co", "src", "copy");
#    _build("lib", "co", $flavour, "copy");
    _build("lib", "dtt", "src", "init");
    _build("lib", "dtt", "src", "copy");
    _build("lib", "statussrv", "src", "init");
    _build("lib", "statussrv", "src", "copy");
    _build("lib", "co", "src", "all");
#    _build("lib", "co", $flavour, "all");
    _build("exe", "co*", "src", "all");
    _build("exe", "*", "src", "copy");
    _build("exp", "stdsoap2", "src", "copy");
    _build("lib", "rt", "src", "all");
    _build("lib", "statussrv", "src", "all");
    _build("lib", "msg_dummy", "src", "init");
    _build("lib", "msg_dummy", "src", "copy");
    _build("lib", "msg_dummy", "src", "lib");
    _build("exp", "rt", "src", "all");
    _build("exe", "wb_rtt", "src", "all");
    _build("lib", "dtt", "src", "all");
    _build("exp", "stdsoap2", "src", "all");
    merge();
    _module("otherio");
    _build("exp","rt","src","copy");
    _build("lib","usbio_dummy","src","init lib");
    _build("lib","usb_dummy","src","init lib");
    _build("lib","cifx_dummy","src","init lib");
    _build("lib","nodave_dummy","src","init lib");
    _build("lib","epl_dummy","src","init lib");
    _build("lib","piface_dummy","src","init lib");
    _build("lib","mqtt_dummy","src","init lib");
    merge();
    _module("sev");
    merge();
    _module("rt");
#    _build("exe", "rt*", "src", "all");
#    _build("exe", "co*", $flavour, "all");
#    _build("exe", "pwr_user", "src", "all");
    if ($to_build{"nmps"}) {
      merge();
      _module("nmps");
      _build("lib", "nmps", "src", "init copy lib");
      merge();
    }
    if ($to_build{"dataq"}) {
      merge();
      _module("dataq");
      _build("lib", "dataq", "src", "all");
      merge();
    }
    if ($to_build{"bcomp"}) {
      _module("profibus");
      merge();
      _module("bcomp");
      _build("lib", "rt", "src", "all");
      merge();
    }
    if ($to_build{"simul"}) {
      _module("simul");
      _build("lib", "simul", "src", "all");
      merge();
    }
    if ($to_build{"misc"}) {
      _module("misc");
      _build("lib", "misc", "src", "all");
      merge();
    }
    if ($to_build{"ssabox"}) {
      _module("ssabox");
      _build("lib", "rt", "src", "all");
      _build("lib", "ssabox", "src", "all");
      merge();
    }
    if ($to_build{"otherio"}) {
      _module("otherio");
      _build("lib", "rt", "src", "all");
      merge();
    }
    if ($to_build{"profibus"}) {
      _module("profibus");
      _build("lib", "rt", "src", "all");
      _build("lib","pnak_dummy","src","init lib");
      merge();
    }
    if ($to_build{"remote"}) {
      _module("remote");
      _build("lib", "remote", "src", "all");
#      _build("exe", "*", "src", "all");
      merge();
    }
    if ($to_build{"opc"}) {
      _module("opc");
      _build("lib", "opc", "src", "all");
      _build("exe", "*", "src", "all");
      merge();
    }
    if ($to_build{"tlog"}) {
      _module("tlog");
#      _build("lib", "tlog", "src", "all");
      merge();
    }
    if ($to_build{"java"}) {
      _module("java");
#      _build("exe", "jpwr_rt_gdh", "src", "all");
      merge();
    }
    if ($to_build{"sev"}) {
      _module("sev");
      _build("lib", "sev", "src", "all");
#      _build("exe", "sev_server", "src", "all");
      merge();
    }
    if ($to_build{"abb"}) {
      _module("abb");
      merge();
    }
    if ($to_build{"siemens"}) {
      _module("siemens");
      merge();
    }
    if ($to_build{"othermanu"}) {
      _module("othermanu");
      merge();
    }
    if ($to_build{"inor"}) {
      _module("inor");
      merge();
    }
    if ($to_build{"klocknermoeller"}) {
      _module("klocknermoeller");
      merge();
    }
    if ($to_build{"telemecanique"}) {
      _module("telemecanique");
      merge();
    }
    if ($to_build{"simul"}) {
      _module("simul");
      merge();
    }
    if ($to_build{"misc"}) {
      _module("misc");
      merge();
    }
    if ($to_build{"z2m"}) {
      _module("z2m");
      _build("tools/exe", "*", "src", "all");
      _build("exp", "z2m", "src", "all");
      merge();
    }
  } elsif ($pass eq "op" ) {
    _module("xtt");
    _build("lib", "*", "src", "init copy");
    _build("lib", "*", $flavour, "copy");
    _build("exp", "*", "src", "init copy");
    _build("mmi", "*", "src", "copy");
    _build("mmi", "*", $flavour, "copy");
    _build("exe", "*", "src", "copy");
    _build("lib", "*", "src", "lib");
    _build("lib", "*", $flavour, "lib");
    _build("exp", "*", "src", "lib");
    _build("exe", "rt*", "src", "all");
    _build("exe", "xtt*", "src", "all");
    _build("exe", "co*", "src", "all");
    _build("exe", "pwr*", "src", "all");
    _build("exe", "rt*", $flavour, "all");
    _build("exe", "xtt*", $flavour, "all");
    _build("exe", "co*", $flavour, "all");
    _build("exe", "pwr*", $flavour, "all");
    merge();
    if ($to_build{"bcomp"}) {
      _module("bcomp");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"profibus"}) {
      _module("profibus");
      _build("mmi", "*", "src", "copy");
      _build("lib", "cow", "src", "init copy lib");
      _build("lib", "cow", $flavour, "init copy lib");
      _build("lib", "xtt", "src", "init copy lib");
      _build("lib", "xtt", $flavour, "init copy lib");
      merge();
    }
    if ($to_build{"opc"}) {
      _module("opc");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"otherio"}) {
      _module("otherio");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"sev"}) {
      _module("sev");
      _build("exe", "sev_xtt", "src", "all");
      _build("exe", "sev_xtt", $flavour, "all");
      merge();
    }
    if ($to_build{"othermanu"}) {
      _module("othermanu");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"abb"}) {
      _module("abb");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"inor"}) {
      _module("inor");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"siemens"}) {
      _module("siemens");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"klocknermoeller"}) {
      _module("klocknermoeller");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"simul"}) {
      _module("simul");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"misc"}) {
      _module("misc");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"ssabox"}) {
      _module("ssabox");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    if ($to_build{"telemecanique"}) {
      _module("telemecanique");
      _build("mmi", "*", "src", "copy");
      merge();
    }
    method_build("rt_io_comm", $flavour);
    method_build("rt_xtt", $flavour);
  }
}

#
# import()
#
sub import()
{
  my $flavour = $_[1];

  my($vmsinc) = $ENV{"pwre_vmsinc"};
  if ($vmsinc ne "" ) {
    copy($_[0], $flavour);

    _module("rt");
    merge();
    _module("bcomp");
    merge();
    _module("nmps");
    merge();
    _module("dataq");
    merge();
    _module("opc");
    merge();
    _module("profibus");
    merge();
    _module("otherio");
    merge();
    _module("remote");
    merge();
  }
}

#
# build_all_modules()
#
sub build_all_modules()
{
  my $flavour;
  if ($_[0] eq "") {
    $flavour = "gtk";
  } else {
    $flavour = $_[0];
  }
  set_flavour($flavour);

  build_kernel($flavour);
  _module("nmps");
  build_module($flavour);
  merge();
  _module("dataq");
  build_module($flavour);
  merge();
  _module("tlog");
  build_module($flavour);
  merge();
  _module("remote");
  build_module($flavour);
  merge();
  _module("profibus");
  build_module($flavour);
  merge();
  _module("bcomp");
  build_module($flavour);
  merge();
  if ($ENV{"PWRE_CONF_JNI"} eq "1") {
    _module("java");
    build_module($flavour);
    merge();
  }
  _module("opc");
  build_module($flavour);
  merge();
  _module("simul");
  build_module($flavour);
  merge();
  _module("misc");
  build_module($flavour);
  merge();
  _module("ssabox");
  build_module($flavour);
  merge();
  _module("otherio");
  build_module($flavour);
  merge();
  _module("othermanu");
  build_module($flavour);
  merge();
  _module("sev");
  build_module($flavour);
  merge();
  _module("test");
  build_module($flavour);
  merge();
  _module("abb");
  build_module($flavour);
  merge();
  _module("siemens");
  build_module($flavour);
  merge();
  _module("inor");
  build_module($flavour);
  merge();
  _module("klocknermoeller");
  build_module($flavour);
  merge();
  _module("telemecanique");
  build_module($flavour);
  merge();
  _module("z2m");
  build_module($flavour);
  merge();

  method_build($flavour);

  build_documentation();
}

sub build_documentation()
{
  _module("rt");
  _build("doc", "prm", "src", "all");
}

#
# clean_exe_all()
#
sub clean_exe_all()
{
  _module("xtt");
  _build("exe","*","src","clean_exe");
  _module("rt");
  _build("exe","rt*","src","clean_exe");
  _build("exe","co*","src","clean_exe");
  _build("exe","wb*","src","clean_exe");
  _module("wb");
  _build("exe","*","src","clean_exe");
  _module("nmps");
  _build("exe","*","src","clean_exe");
  _module("dataq");
  _build("exe","*","src","clean_exe");
  _module("tlog");
  _build("exe","*","src","clean_exe");
  _module("remote");
  _build("exe","*","src","clean_exe");
  _module("rt");
}

#
# build_all_wbl()
#
sub build_all_wbl()
{
  _module("rt");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "pwrs", "src", "lib");
  _build("wbl", "pwrb", "src", "lib");
  _build("wbl", "wb", "src", "lib");
  _build("wbl", "rt", "src", "lib");
  merge();
  _module("nmps");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "nmps", "src", "lib");
  merge();
  _module("dataq");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "dataq", "src", "lib");
  merge();
  _module("tlog");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "tlog", "src", "lib");
  merge();
  _module("profibus");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("opc");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("sev");
  my($load_dir) = $ENV{"pwr_load"};
  system( "rm $load_dir/*.dbs");
  _build("wbl", "sev", "src", "lib");
  merge();
  _module("remote");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "remote", "src", "lib");
  merge();
  _module("bcomp");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "bcomp", "src", "lib");
  merge();
  _module("otherio");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("simul");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "simul", "src", "lib");
  merge();
  _module("misc");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "misc", "src", "lib");
  merge();
  _module("ssabox");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("abb");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("siemens");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("telemecanique");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("inor");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("klocknermoeller");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("othermanu");
  my($load_dir) = $ENV{"pwr_load"};
  system("rm $load_dir/*.dbs");
  _build("wbl", "mcomp", "src", "lib");
  merge();
  _module("rt");
}

sub method_build()
{
  my $flavour;
  my $program;
  if ($_[0] eq "motif" || $_[0] eq "gtk" || $_[0] eq "qt" ) {
    $flavour = $_[0];
  } else {
    $program = $_[0];
    $flavour = $_[1];
  }

  if (!defined($program)) {
    printf("-- Relink method dependent programs %s\n", $flavour);
    method_build("rt_io_comm");
    method_build("wb", $flavour);
    method_build("rt_xtt", $flavour);
    return;
  }
  if ($_[0] eq "rt_io_comm" ) {
    printf("-- Method build %s\n", $program);
    _module("rt");
    $ENV{"export_type"} = "exp";
     my($exe_dir) = $ENV{"pwr_exe"};
    system("if [ -e $exe_dir/rt_io_comm ]; then rm $exe_dir/rt_io_comm; fi");
    _build("exe", "rt_io_comm", "src", "all");
    merge();
  }
  if ($flavour eq "") {
    $flavour = "gtk";
  }
  if ($_[0] eq "wb" ) {
    printf("-- Method build %s %s\n", $program, $flavour);

    _module("wb");
    $ENV{"export_type"} = "exp";
    my($exe_dir) = $ENV{"pwr_exe"};
    system("if [ -e $exe_dir/wb_$flavour ]; then rm $exe_dir/wb_$flavour; fi");
    _build("exe", "wb", $flavour, "all");
    _build("exe", "wb", "src", "all");
     merge();
  }
  if ($_[0] eq "rt_xtt" ) {
    printf("-- Method build %s %s\n",  $program, $flavour);

    _module("xtt");
    $ENV{"export_type"} = "exp";
    my($exe_dir) = $ENV{"pwr_exe"};
    system("if [ -e $exe_dir/rt_xtt_$flavour ]; then rm $exe_dir/rt_xtt_$flavour; fi");
    _build("exe", "rt_xtt", $flavour, "all");
    _build("exe", "rt_xtt", "src", "all");
    merge();
  }
}

sub create_all_modules()
{
  if (! -e $configfile ) {
    configure();
  }

  _module("rt");
  create_module();
  _module("xtt");
  create_module();
  _module("wb");
  create_module();
  _module("nmps");
  create_module();
  _module("dataq");
  create_module();
  _module("tlog");
  create_module();
  _module("remote");
  create_module();
  _module("java");
  create_module();
  _module("bcomp");
  create_module();
  _module("simul");
  create_module();
  _module("misc");
  create_module();
  _module("profibus");
  create_module();
  _module("sev");
  create_module();
  _module("opc");
  create_module();
  _module("test");
  create_module();
  _module("ssabox");
  create_module();
  _module("othermanu");
  create_module();
  _module("otherio");
  create_module();
  _module("abb");
  create_module();
  _module("siemens");
  create_module();
  _module("inor");
  create_module();
  _module("klocknermoeller");
  create_module();
  _module("telemecanique");
  create_module();
  _module("z2m");
  create_module();
}

#
# build_module()
#
sub build_module()
{
  $copy = 0;
  $lib = 0;
  $exe = 0;

  if ($_[0] eq "copy" || $_[1] eq "copy" || $_[2] eq "copy") {
    $copy = 1;
  }
  if ($_[0] eq "lib" || $_[1] eq "lib" || $_[2] eq "lib") {
    $lib = 1;
  }
  if ($_[0] eq "exe" || $_[1] eq "exe" || $_[2] eq "exe") {
    $exe = 1;
  }
  if ($copy == 0 && $lib == 0 && $exe == 0) {
    $copy = 1;
    $lib = 1;
    $exe = 1;
  }
  if ($_[0] eq "qt" || $_[1] eq "qt" || $_[2] eq "qt" || $_[3] eq "qt") {
    $flavour = "qt";
  } else {
    $flavour = "gtk";
  }
  set_flavour($flavour);

  if (!defined($ENV{"pwre_env"})) {
    print("++ Environment is not initialized!\n");
    exit 1;
  }
  my($module) = $ENV{"pwre_module"};
  my($einc) = $ENV{"pwr_einc"};
  my($remoteclasses) = $einc . "/pwr_remoteclasses.h";
  my($nmpsclasses) = $einc . "/pwr_nmpsclasses.h";
  my($dataqclasses) = $einc . "/pwr_dataqclasses.h";
  my($profibusclasses) = $einc . "/pwr_profibussclasses.h";
  my($bcompclasses) = $einc . "/pwr_basecomponentsclasses.h";
  my($otherioclasses) = $einc . "/pwr_otherioclasses.h";

  if ($ver == 1) {
    printf("\n");
  }
  _log('I', '', '', 'Building module');
  #show();

  if ($module eq "rt") {
    if ($copy == 1) {
      _build("exp", "inc", "src", "all");
      _build("exp", "com", "src", "all");
      _build("tools/exe", "*", "src", "all");
      merge("exe/tools_cmsg2c");
      merge("exe/tools_msg2cmsg");
      merge("exe/tools_pdrgen");
      _build("msg", "*", "src", "all");
      _build("lib", "rt", "src", "init");
      _build("lib", "rt", "src", "copy");
      _build("lib", "co", "src", "init");
      _build("lib", "co", "src", "copy");
      _build("lib", "dtt", "src", "init");
      _build("lib", "dtt", "src", "copy");
      _build("lib", "msg_dummy", "src", "init");
      _build("lib", "msg_dummy", "src", "copy");
      _build("lib", "statussrv", "src", "init");
      _build("lib", "statussrv", "src", "copy");
      _build("lib", "co", "src", "all");
      _build("exe", "co*", "src", "all");
      _build("wbl", "pwrs", "src", "init");
      _build("wbl", "pwrs", "src", "copy");
      _build("wbl", "pwrb", "src", "init");
      _build("wbl", "pwrb", "src", "copy");
      _build("exe", "*", "src", "copy");
      _build("exp", "stdsoap2", "src", "copy");
    }

    if ($lib == 1) {
      if (! -e $nmpsclasses ) {
        _module("nmps");
        _build("wbl", "nmps", "src", "init");
        _build("wbl", "nmps", "src", "copy");
        merge("inc/pwr_nmpsclasses.h");
      }
      if (! -e $dataqclasses ) {
        _module("dataq");
        _build("wbl", "dataq", "src", "init");
        _build("wbl", "dataq", "src", "copy");
        merge("inc/pwr_dataqclasses.h");
      }
      if (! -e $remoteclasses ) {
        _module("remote");
        _build("wbl", "remote", "src", "init");
        _build("wbl", "remote", "src", "copy");
        merge("inc/pwr_remoteclasses.h");
      }
      if (! -e $profibusclasses ) {
        _module("profibus");
        _build("wbl", "mcomp", "src", "init");
        _build("wbl", "mcomp", "src", "copy");
        merge("inc/pwr_profibusclasses.h");
      }
      if (! -e $bcompclasses ) {
        _module("bcomp");
        _build("wbl", "bcomp", "src", "init");
        _build("wbl", "bcomp", "src", "copy");
        merge("inc/pwr_basecomponentclasses.h");
      }
      if (! -e $otherioclasses ) {
        _module("otherio");
        _build("wbl", "mcomp", "src", "init");
        _build("wbl", "mcomp", "src", "copy");
        merge("inc/pwr_otherioclasses.h");
      }
      _module("rt");
      _build("lib", "rt", "src", "all");
      _build("exp", "rt", "src", "all");
      _build("lib", "msg_dummy", "src", "all");
      _build("exp", "rt", "src", "all");
      _build("lib", "statussrv", "src", "all");
      _build("exp", "stdsoap2", "src", "all");
    }
    if ($exe == 1) {
      _build("exe", "wb_rtt", "src", "all");
      _build("lib", "dtt", "src", "all");
      _build("exe", "rt*", "src", "all");
      _build("exe", "co*", $flavour, "all");
      _build("exe", "pwr_user", "src", "all");
      _build("exe", "pwr_rtmon", "src", "all");
      _build("exe", "pwr_rtmon", "$flavour", "all");
      _build("exp", "ge", "src", "all");
      _build("exp", "py", "src", "all");
      _build("exp", "*", "src", "exe");
      _build("mmi", "*", "src", "copy");
      _build("mmi", "*", $flavour, "copy");
      _build("wbl", "pwrs", "src", "lib");
      _build("wbl", "pwrb", "src", "lib");
      _build("wbl", "wb", "src", "lib");
      _build("wbl", "rt", "src", "lib");
      _build("doc", "web", "src", "all");
      _build("doc", "dweb", "src", "all");
      _build("doc", "orm", "src", "all");
      _build("doc", "man", "src", "all");
      _build("doc", "dox", "src", "all");
      _build("wbl", "pwrs", "src", "exe");
      _build("wbl", "pwrb", "src", "exe");
    }
  } elsif ($module eq "java") {
    if ($copy == 1) {
      _build("jsw", "*", "src", "all");
    }
    if ($lib == 1) {
      _build("jpwr", "rt", "src", "all");
      #_build("jpwr", "jopg", "src", "all");
      #_build("jpwr", "jop", "src", "all");
      #_build("jpwr", "rt_client", "src", "all");
    }
    if ($exe == 1) {
      _build("exe", "jpwr_rt_gdh", "src", "all");
    }
  } elsif ($module eq "profibus") {
    if ($copy == 1) {
      _build("wbl", "*", "src", "init");
      _build("wbl", "*", "src", "copy");
      _build("lib", "*", "src", "init");
      _build("lib", "*", "src", "copy");
      _build("lib", "*", $flavour, "copy");
      _build("exp", "*", "src", "init");
      _build("exp", "*", "src", "copy");
      _build("mmi", "*", "src", "copy");
      _build("mmi", "*", $flavour, "copy");
      _build("exe", "*", "src", "copy");
    }
    if ($lib == 1) {
      _build("lib", "*", "src", "lib");
      _build("lib", "*", $flavour, "lib");
      _build("exp", "*", "src", "lib");
    }
    if ($exe == 1) {
      _build("wbl", "*", "src", "lib");
      _build("wbl", "*", "src", "exe");
      if ($lib == 1) {
	merge();
      }
      _build("exe", "*", "src", "all");
      _build("exe", "*", $flavour, "all");
    }
  } else {
    if ($copy == 1) {
      _build("wbl", "*", "src", "init");
      _build("wbl", "*", "src", "copy");
      _build("lib", "*", "src", "init");
      _build("lib", "*", "src", "copy");
      _build("lib", "*", $flavour, "copy");
      _build("exp", "*", "src", "init");
      _build("exp", "*", "src", "copy");
      _build("mmi", "*", "src", "copy");
      _build("mmi", "*", $flavour, "copy");
      _build("exe", "*", "src", "copy");
    }
    if ($lib == 1) {
      _build("lib", "*", "src", "lib");
      _build("lib", "*", $flavour, "lib");
      _build("exp", "*", "src", "lib");
    }
    if ($exe == 1) {
      _build("doc", "orm", "src", "copy");
      _build("wbl", "*", "src", "lib");
      _build("wbl", "*", "src", "exe");
      _build("exe", "*", "src", "all");
      _build("exe", "*", $flavour, "all");
      _build("exp", "*", "src", "exe");
      _build("doc", "dsh", "src", "copy");
    }
  }
}

#
# copy()
#
sub copy()
{
  if (!defined($ENV{"pwre_env"})) {
    print("++ Environment is not initialized!\n");
    exit 1;
  }

  my($vmsinc) = $ENV{"pwre_vmsinc"};
  my($bindir) = $ENV{"pwre_bin"};
  my($docroot) = $ENV{"pwre_broot"} . "/" . $ENV{"pwre_os"} . "/" . $ENV{"pwre_hw"} . "/exp/doc/";
  my($exproot) = $ENV{"pwre_broot"} . "/" . $ENV{"pwre_os"} . "/" . $ENV{"pwre_hw"};

  printf("--\n");
  printf("-- Copy file from Import root\n");
  printf("-- Import root: %s\n", $vmsinc);
  printf("--\n");

  if ($_[0] eq "doc" ) {
    system("cp -r " . $vmsinc . "/exp/doc/* " . $docroot);
  } elsif ($_[0] eq "dbs" ) {
    system("$bindir/import_dbs.sh " . $vmsinc . " " . $exproot);
  } else {
    if ($hw eq "x86" && $real_hw eq "x86_64") {
      $ENV{"cross_compile"} = "-m32";
    }
    system("make -f $bindir/import_files.mk" . " " . $_[0]);

    my $flavour = $_[1];
    if ($flavour eq "") {
      $flavour = "gtk";
    }

    system("make -f $bindir/import_files.mk" . " " . $_[0] . "_" . $flavour);
  }
}

sub create()
{
  if ($_[0] eq "all" || $_[0] eq "") {
    create_all_modules();
    return;
  }

  if ($_[0] eq "module" || $_[0] eq "") {
    create_module();
    return;
  }

  printf("++ Undefined command\n");
}

#
# create
#
sub create_module()
{
  my($newdir);
  my($root) = $ENV{"pwre_broot"};
  my($module) = $ENV{"pwre_bmodule"};

 if (!defined($root)) {
    printf("++\n++ No build root is defined\n");
    exit 1;
  }

  create_dir($root);

  $root .= "/" . $ENV{"pwre_os"};
  create_dir($root);

  $root .= "/" . $ENV{"pwre_hw"};
  create_dir($root);

  $str = 'Creating build tree ' . $root . '/' . $module;
  _log('I', '', '', $str);

  $newdir = $root . "/bld";
  create_dir($newdir);
  create_dir($newdir . "/exe");
  create_dir($newdir . "/lib");
  create_dir($newdir . "/msg");
  create_dir($newdir . "/jpwr");
  create_dir($newdir . "/wbl");

  if ($module eq "rt") {
    $newdir = $root . "/exp";
    create_base($newdir);
  }
#  else {
    $newdir = $root . "/$module";
    create_base($newdir);
#  }
  $newdir = $root . "/tmp";
  create_dir($newdir);
}

#
# dele()
#
sub dele()
{
  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";

  $label = $_[0];

  if (defined($envdb{$label})) {
    print("-- Delete environment \"$label\" [n]? ");
    my($tmp);
    chomp($tmp = <STDIN>);

    if ($tmp =~ /^[yY]/) {
      delete $envdb{$label};
      printf("-- Environment \"%s\" has been deleted\n", $label);
    }

  } else {
    printf("++ Environment \"%s\" doesn't exists\n", $label);
  }

  untie(%envdb)|| die "++ can't untie $dbname!";
}

#
# help
#
sub help()
{
  my($key);

  printf("++\n");
  printf("++ Usage: pwre verb [...]\n");
  printf("++ A verb name may be shortned as long as it isn't ambiguos, eg. pwre h[elp]\n");
  printf("++ Verbs:\n");

  foreach $key (sort keys (%verbs)) {
    ($verb, $usage)  =  split(/:/, $verbs{$key});
    if (!($verb =~ /^_/)) {
      printf("++\n");
      &$usage();
    }
  }

  printf("++\n");
  exit 1;
}

#
# list()
#
sub list()
{
  my($env);
  printf("Using Database:%s\n",$dbname);

  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";

  print("-- Defined environments:\n");
  foreach $env (sort keys (%envdb)) {
      $varstr = $envdb{$env};
      ($sroot, $vmsinc, $broot, $btype, $os, $hw, $desc)  =  split(/;/, $varstr);
      @vars = ($sroot, $vmsinc, $broot, $btype, $os, $hw, $desc);
    printf("   %s			%s\n", $env, $desc);
  }
  print("--\n");

  untie(%envdb)|| die "++ can't untie $dbname!";
}

#
# modify()
#
sub modify()
{
  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";
  $label = $_[0];
  $varstr = $envdb{$label};
  if ($varstr ne "") {
    read_vars();
    get_vars();
    update_db();
  } else {
    printf("Environment %s doesn't exists\n", $label);
  }
  untie(%envdb)|| die "++ can't untie $dbname!";
}

#
# show
#
sub show()
{
  if ($_[0] ne "") {
    $label = $_[0];
  } else {
    $label = $ENV{"pwre_env"};
    if (!defined($label)) {
      print("++ Environment is not initialized!\n");
      exit 1;
    }
  }

  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";

  read_vars();
  if ($varstr ne "") {
    print("--\n");
    printf("-- Environment    : %s\n", $label);
    show_vars();
    print("--\n");
  } else {
    printf("++ Environment %s doesn't exists\n", $label);
  }
  untie(%envdb)|| die "++ can't untie $dbname!";
}

#
# tags()
#
sub tags()
{
  if (!defined($ENV{"pwre_env"})) {
    print("++ Environment is not initialized!\n");
    exit 1;
  }

  printf("--\n");
  printf("-- Creating a CRiSP tag-file named ~/pwre/tags\n");
  system("pwr_tags") && exit;
}

#
# _build()
#
sub _build() # args: branch, subbranch, flavour, phase
{
  my($branch) = $_[0];
  if (!defined($branch)) {
    usage_build();
    exit 1;
  }

  my($subbranch) = $_[1];
  if (!defined($subbranch)) {
    usage_build();
    exit 1;
  }
  my($flavour) = $_[2];
  shift(@_);
  shift(@_);
  shift(@_);

  if ($ver == 1) {
    printf("--   Build %s %s %s\n", $branch, $subbranch, $flavor);
  }

  my($grepstr) = $ENV{"pwre_target"};

  my($globstr) = $ENV{"pwre_sroot"} . "/$branch";
  my(@dirs1) = glob($globstr);
  my($dir1);
  my(@dirs2);
  my($dir2);
  my(@mfiles);
  my($globstr1);
  my($makefile);

  if ($hw eq "x86" && $real_hw eq "x86_64") {
    $ENV{"cross_compile"} = "-m32";
  }

  chomp(my $cpu_count = `grep -c -P '^processor' /proc/cpuinfo`);
  $cpu_count = $cpu_count * 2;

  foreach $dir1 (@dirs1) {
    $globstr1 = "$dir1" . "/$subbranch/$flavour";
    @dirs2 = glob($globstr1);
    foreach $dir2 (@dirs2) {
      if (! -d $dir2) {
        next;
      }
      chdir($dir2) || die "cannot cd to $dir2 ($!)";
      $ENV{"PWD"} = $dir2;

      if (-e "$dir2/makefile") {
        $makefile = "$dir2/makefile";
      } else {
        $makefile = $ENV{"pwre_croot"} . "/src/tools/bld/src/generic_makefile";
      }

      if ($parallel eq "1" && (($branch eq "lib" && $subbranch ne "dtt") || $branch eq "wbl")) {
        # All libraries and wbl files can be compiled in parallel
        if (system("make -f $makefile -j$cpu_count -l$cpu_count @_")) {
	  _log("F", $branch, $subbranch, "Fatal error, build terminated");
	  exit 1;
	}
      } else {
        if (system("make -f $makefile @_")) {
	  _log("F", $branch, $subbranch, "Fatal error, build terminated");
	  exit 1;
	}
      }
    }
  }
}

#
# merge()
#
sub merge()
{
  if (!defined($ENV{"pwre_env"})) {
    print("++ Environment is not initialized!\n");
    exit 1;
  }

  if ($_[0] eq "-v") {
    $ver = 1;
    shift(@_);
  }

  my($eroot) = $ENV{"pwre_broot"};

  if (!defined($eroot)) {
    printf("++\n++ No build root is defined\n");
    exit 1;
  }
  my($file) = $_[0];

#  if ($module eq "rt") {
#    printf("++\n++ No merge for module rt needed\n");
#    exit 1;
#  }
  $eroot .= "/" . $ENV{"pwre_os"};
  $eroot .= "/" . $ENV{"pwre_hw"};
  my($mroot) = $eroot;
  $mroot .= "/" . $ENV{"pwre_module"};
  $eroot .= "/exp";
  if ($ver == 1) {
    printf("\n");
    printf("-- Merge %s %s\n", $module, $file);
  }

  system($ENV{pwre_bin} . "/pwre_merge.sh " . $ver . " " . $mroot . " " . $eroot . " " . $file);
}

#
# _exists()
#
sub _exists()
{
  my($ret) = 1;
  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";

  $label = $_[0];

  if (defined($envdb{$label})) {
    $ret = 0;
  }

  untie(%envdb)|| die "++ can't untie $dbname!";
  exit $ret;
}

#
# _print()
#
sub _print()
{
  use DB_File;
  tie(%envdb, "DB_File", $dbname, O_CREAT|O_RDWR, 0644) || die "++ can't tie $dbname!";

  $label = $_[0];
  read_vars();
  if ($varstr ne "") {
     foreach (@vars) {
       if ($_ eq "") {
	 printf("\"\"\n");
       } else {
         printf("%s\n", $_);
       }
     }
  } else {
    printf("++ Environment %s doesn't exists\n", $label);
  }
  untie(%envdb)|| die "++ can't untie $dbname!";
}

sub _module()
{
  if ($ver == 1) {
    printf("-- Set module %s\n", $_[0]);
  }

  my($modu) = $_[0];
  my($sroot) = $ENV{"pwre_sroot"};
  my($idx) = rindex($sroot,"/");
  $sroot = substr($sroot, 0, $idx);
  my($broot) = $ENV{"pwre_broot"} . "/" . $ENV{"pwre_os"} . "/" . $ENV{"pwre_hw"};

  $ENV{"pwre_module"} = $modu;
  $ENV{"pwre_bmodule"} = $modu;
  if ($modu eq "rt") {
#    $ENV{"pwre_bmodule"} = "exp";
    $ENV{"pwre_sroot"} = $sroot . "/src";
  } else {
    $ENV{"pwre_sroot"} = $sroot . "/" . $modu;
  }
  $ENV{"pwr_exe"} = $broot . "/" . $ENV{"pwre_bmodule"} . "/exe";
  $ENV{"pwr_inc"} = $broot . "/" . $ENV{"pwre_bmodule"} . "/inc";
  $ENV{"pwr_load"} = $broot . "/" . $ENV{"pwre_bmodule"} . "/load";
  $ENV{"pwr_lib"} = $broot . "/" . $ENV{"pwre_bmodule"} . "/lib";
  $ENV{"pwr_lis"} = $broot . "/" . $ENV{"pwre_bmodule"} . "/lis";
  $ENV{"pwr_obj"} = $broot . "/" . $ENV{"pwre_bmodule"} . "/obj";
  $ENV{"pwr_doc"} = $broot . "/" . "exp" . "/doc";
}

#
# Misc. subroutines
#
sub create_base()
{
  my($newdir) = $_[0];

  create_dir($newdir);
  create_dir($newdir . "/db");
  create_dir($newdir . "/exe");
  create_dir($newdir . "/exe/sv_se");
  create_dir($newdir . "/exe/en_us");
  create_dir($newdir . "/exe/de_de");
  create_dir($newdir . "/exe/fr_fr");
  create_dir($newdir . "/exe/zh_cn");
  create_dir($newdir . "/inc");
  create_dir($newdir . "/load");
  create_dir($newdir . "/lib");
  create_dir($newdir . "/lis");
  create_dir($newdir . "/obj");
  create_dir($newdir . "/src");
  create_dir($newdir . "/cnf");
  create_dir($newdir . "/doc");
  create_dir($newdir . "/doc/dox");
  create_dir($newdir . "/doc/prm");
  create_dir($newdir . "/doc/sv_se");
  create_dir($newdir . "/doc/en_us");
  create_dir($newdir . "/doc/sv_se/orm");
  create_dir($newdir . "/doc/en_us/orm");
  create_dir($newdir . "/doc/sv_se/dsh");
  create_dir($newdir . "/doc/en_us/dsh");
  create_dir($newdir . "/doc/help");
  create_dir($newdir . "/web");
}

sub create_dir()
{
  my($dir) = $_[0];

  if (!chdir($dir)) {
    if (mkdir($dir, 0775)) {
#     printf("-- mkdir: %s\n", $dir);
    } else {
      printf("++\n++ Cannot mkdir %s, reason: %s\n", $dir, $!);
      exit 1;
    }
  }
}

sub get_vars()
{
  if ($_[1] eq "") {
    $sroot = 	get_var(" Source root [%s]? ", $sroot);
  } else {
    $sroot = $_[1];
  }
  if ($_[2] eq "") {
    $vmsinc = 	get_var(" Import root  [%s]? ", $vmsinc);
  } else {
    $vmsinc = $_[2];
  }
  if ($_[3] eq "") {
    $broot = 	get_var(" Build root  [%s]? ", $broot);
  } else {
    $broot = $_[3];
  }
  if ($_[4] eq "") {
    $btype = 	get_var(" Build type  [%s]? ", $btype);
  } else {
    $btype = $_[4];
  }
  if ($_[5] eq "") {
    $os =    	get_var(" OS          [%s]? ", $os);
  } else {
    $os = $_[5];
  }
  if ($_[6] eq "") {
    $hw =    	get_var(" Hardware    [%s]? ", $hw);
  } else {
    $hw = $_[6];
  }
  if ($_[7] eq "") {
    $desc =  	get_var(" Description [%s]? ", $desc);
  } else {
    $desc = $_[7];
  }

  $varstr = join(";", ($sroot, $vmsinc, $broot, $btype, $os, $hw, $desc));
}

sub get_var()
{
  my($tmp);

  printf($_[0], $_[1]);
  chomp($tmp = <STDIN>);
  if ($tmp eq "") {
    $tmp = $_[1];
  }
  return $tmp;
}

sub read_vars()
{
  $varstr = $envdb{$label};
  ($sroot, $vmsinc, $broot, $btype, $os, $hw, $desc)  =  split(/;/, $varstr);
  @vars = ($sroot, $vmsinc, $broot, $btype, $os, $hw, $desc);
}

sub show_vars()
{
  my($module) = $ENV{"pwre_module"};

  printf("-- Module.........: %s\n", $module);
  printf("-- Source root....: %s\n", $sroot);
  if ($vmsinc ne "") {
    printf("-- Import root....: %s\n", $vmsinc);
  }
  printf("-- Build root.....: %s\n", $broot);
  printf("-- Build type.....: %s\n", $btype);
  printf("-- OS.............: %s\n", $os);
  printf("-- Hardware.......: %s\n", $hw);
  printf("-- Description....: %s\n", $desc);
}

sub update_db()
{
  $envdb{$label} = $varstr;
}

sub usage_add()
{
  printf("++ add env                       : Adds an environment to the database\n");
}

sub usage_configure()
{
  printf("++ configure                     : Configures an environment\n");
}

sub usage_build()
{
  printf("++ build 'branch' 'subbranch' ['flavour']['phase']: Build, eg. pwre build exe rt* src all\n");
}

sub usage_build_module()
{
  printf("++ build_module                     : Builds all in current module\n");
}

sub usage_build_kernel()
{
  printf("++ build_kernel                  : Builds all in kernel modules (kernel, xtt, wb)\n");
}

sub usage_build_all_modules()
{
  printf("++ build_all_modules             : Builds all in all modules\n");
}

sub usage_build_all_wbl()
{
  printf("++ build_all_wbl                 : Builds wbl in all modules\n");
}

sub usage_ebuild()
{
  printf("++ ebuild 'block' 'flavour'       : Builds rt or op block\n");
}

sub usage_clean_exe_all()
{
  printf("++ clean_exe_all                 : Cleans all exe in all modules\n");
}

sub usage_method_build()
{
  printf("++ method_build                  : Rebuild method dependent programs\n");
}

sub usage_create_all_modules()
{
  printf("++ create_all_modules            : Create build trees for all modules\n");
}

sub usage_copy()
{
  printf("++ copy                          : Copy Proview include files from the VMS host\n");
}

sub usage_create()
{
  printf("++ create                        : Creates the build directory tree\n");
}

sub usage_dele()
{
  printf("++ delete 'env'                  : Deletes an environment from the database\n");
}

sub usage_help()
{
  printf("++ help                          : Lists all command verbs\n");
}

sub usage_list()
{
  printf("++ list                          : Lists all environments in the database\n");
}

sub usage_init()
{
  printf("++ init 'env'                    : Inits an environment\n");
}

sub usage_import()
{
  printf("++ import 'block' ['flavour']    : Import files from import root, block dbs, rt, op, java, doc\n");
}

sub usage_module()
{
  printf("++ module 'module'               : Set module\n");
}

sub usage_merge()
{
  printf("++ merge                         : Merge module base to exp base\n");
}

sub usage_modify()
{
  printf("++ modify 'env'                  : Modfies an existing environment\n");
}

sub usage_show()
{
  printf("++ show [env]                    : Shows current or given environment\n");
}

sub usage_tags()
{
  printf("++ tags                          : Creates a CRiSP tag-file named ~/pwre/tags\n");
}

sub _log() # args: severity, branch, subbranch, text
{
  my($severity) = $_[0];
  my($branch) = $_[1];
  my($subbranch) = $_[2];
  my($text) = $_[3];

  my($module) = $ENV{"pwre_module"};
  my($lisdir) = $ENV{"pwr_elis"};

  my $tim = strftime("%Y-%m-%d %H:%M:%S", localtime(time));
  my $str = $severity . ' ' . $tim . '.00  build-' . $module . ', ' . $text . "\n";

  my($fname) = ">>" . $lisdir . "/build.tlog";
  if (open(my $fd, $fname)) {
    print $fd $str;
    close($fd);
  }
  printf("%s", $str);
}
