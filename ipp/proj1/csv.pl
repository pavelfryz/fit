#!/usr/bin/perl

#CSV:xfryzp00

use strict;
use warnings;
use IO::File;
use Encode;
use Text::CSV_PP;
use XML::Writer;
use Getopt::Long;
Getopt::Long::Configure("gnu_compat");

#
# name: usage
# Vrati retezec se zpusobem pouziti
#
sub usage{
  return "Pouziti: ".$0." [--help] [--input=SOUBOR] [--output=SOUBOR] [-n] [-r=TAG]\n".
         "                [-s=ODDELOVAC] [-h] [-l=TAG] [-i] [--start=CISLO]\n".
         "                [-e, --error-recovery] [--missing-val=HODNOTA]\n".
         "                [--all-columns] [--padding] [--validate]\n";
}

#
# name: usage
# Vrati retezec s napovedou
#
sub help{
  return  usage()."\n".
          "Popis: Skript konvertuje zadany CSV soubor do XML. Kazdeme radku odpovida\n".
          "       jeden parovy element, ktery bude obsahovat elementy pro jednotlive\n".
          "       sloupce.\n\n".
          "Volby:\n".
          "  --help           Vypise tuto napovedu\n".
          "  --input=SOUBOR   Nastavi vstup ze zadaneho souboru\n".
          "  --output=SOUBOR  Nastavu vystup do zadaneho souboru\n".
          "  -n               Negeneruje XML hlavicku do vysledneho souboru\n".
          "  -r=TAG           Obali vysledek  elementem do zadaneho tagu\n".
          "  -s=ODDELOVAC     Nastavi oddelovac sloupcu CSV souboru\n".
          "  -h               Jmena tagu pro sloupce se odvodi z prvniho radku CSV souboru\n".
          "  -l=TAG           Zmeni jmeno tagu pro radky na zadadny tag\n".
          "  -i               Prida k radku atribut index s cislem radku\n".
          "  --start=CISLO    Nastavi citac radku na zadane kladne cislo\n".
          "  -e, --error-recovery Zapne zotaveni z chybneho poctu sloupcu v CSV souboru\n".
          "  --missing-value=HODNOTA Nastavi hodnotu, kterou se doplni chybejici sloupce\n".
          "  --all-columns    Nastavi vypis na vsechny sloupce\n".
          "  --padding        Doplni citace sloupcu a radku nulami\n".
          "  --validate       Zkontroluje vstupni CSV soubor\n\n".
          "  Parametr help nelze kombinovat, parametr validate lze kombinovan pouze\n".
          "  s parametry input a output. Spolecne s parametrem i musi byt definovan\n".
          "  parametr l. Spolecne s parametrem start musi byt definovan parametr i.\n".
          "  Parametry missing-value a all-columns mohou byt pouzity pouze pokud je\n".
          "  definovan parametr error-recovery\n\n".
          "Navratove hodnoty:\n".
          "  0  Vse v poradku\n".
          "  1  Spatne parametry prikazove radky\n".
          "  2  Neexistujici vstupni soubor nebo se nepovedl otevrit\n".
          "  3  Chyba pri pokusu o otevreni vystupniho souboru\n".
          "  4  Chybny format vstupniho souboru (bez par. validate)\n".
          "  30 Nevalidni tag v parametrech\n".
          "  31 Prvni radek nesel prevest na tagy pro sloupce\n".
          "  32 Ruzny pocet sloupcu v CSV (bez par. validate)\n".
          "  39 Chybny format vstupniho souboru s parametrem validate\n";
}

##########################################################################################
####  Deklarace promenych
##########################################################################################

my $input;              # vstupni soubor
my $output;             # vystupni soubor
my @tagsArray;          # pole elementu oznacujici sloupce
my $generateHeader = 1; # generovani hlavicky XML dokumentu
my $rootTag;            # korenovy element
my $lineCnt;            # index radku
my $lineTag = 'row';    # element obalujici radek
my $columnCnt = 0;      # pocet sloupcu prvniho radku
my $errorRecovery;      # zotaveni z chyb(false)
my $missingVal = '';    # doplnovana hodnota do chybejicich sloupcu
my $allColumns;         # zaznamenat vsechny radky(false)
my $h;                  # nacist jmena elementu z prvniho radku(false)
my $separator = ',';    # separator bunek CSV souboru
my $padding;            # doplnovat nuly u citacu(false)
my $validate;           # validovat vstupni csv(false)

##########################################################################################
####  Zacatek programu
##########################################################################################

# zpracovani parametru
optsParse();

# otevreni vstupniho souboru
if(openInput()){
  print STDERR "Chyba: Nepodarilo se otevrit vstupni soubor\n";
  exit 2;
}

# otevreni vystupniho souboru
if(openOutput()){
  print STDERR "Chyba: Nepodarilo se otevrit vystupni soubor\n";
  exit 3;
}

# inicializece a nastaveni modulu Text::CSV_PP pro nacteni vstupu
my $csv = Text::CSV_PP->new({binary => 1,sep_char => $separator,eol => "\015\012",verbatim=>1});

# nacteni vstupu pomoci modulu Text::CSV_PP
my @file = @{$csv->getline_all($input)};

# kontrola nacteni vstupu
if(!$csv->eof()){
  # vstup nebyl nacten cely -> chyba
  print STDERR "Chyba: Nevalidni CSV soubor\n";
  if($validate){ exit 39;}
  exit 4;
}

undef $input;
undef $csv;

# nastavi pocet sloupcu podle prvniho radku
$columnCnt = @{$file[0]};

# nastavy padding u citace radku
if ($padding && defined $lineCnt) {
  $lineCnt = sprintf("%0*d", length (scalar @file + $lineCnt - 1), $lineCnt);
}

# naplni pole tagu podle prvniho radku
if($h) { fillTagArray();}

# inicializace modulu pro vypis XML dokumentu
my $xmloutput;
my $writer = XML::Writer->new(OUTPUT => \$xmloutput, DATA_MODE => 1,UNSAFE => 1,
                              DATA_INDENT => 2,ENCODING => 'utf-8');

# vypsani hlavicky XML dokumentu
if($generateHeader){ $writer->xmlDecl('UTF-8');}

# vypsani startovniho korenoveho elementu XML dokumentu
if(defined $rootTag){ $writer->startTag($rootTag);}

# cyklus projde jednotlive radky nacteneho CSV souboru
for (@file){
  my @line = @$_;
  # vypsani startovniho elementu radku
  if(defined $lineCnt){
    $writer->startTag($lineTag, 'index'=> $lineCnt);
    $lineCnt++;
  }else{
    $writer->startTag($lineTag);
  }

  # zkontrolovani poctu sloupcu
  if(@line != $columnCnt){
    if($errorRecovery){
      # doplneni chybejicich sloupcu
      while(@line < $columnCnt){
        push @line, $missingVal;
      }
      # smazani prebyvajicich
      if(!$allColumns){
        while(@line > $columnCnt){
          pop @line;
        }
      }
    }else{
      # spatny pocet sloupcu
      print STDERR "Chyba: Spatny pocet sloupcu CSV souboru\n";
      if($validate){ exit 39;}
      exit 32;
    }
  }

  # cyklus projde jednotlive sloupce nacteneho CSV souboru
  for (my $cnt=0; $cnt < scalar @line; $cnt++) {
    # vypsani startovniho tagu sloupce
    if($h && $cnt < $columnCnt){
      $writer->startTag($tagsArray[$cnt]);
    }elsif ($padding){
      $writer->startTag("col".sprintf("%0*d", length (scalar @line), $cnt+1));
    }else{
      $writer->startTag("col".($cnt+1));
    }

    # vypsani dat sloupce
    $writer->characters($line[$cnt]);

    # vypsani koncoveho tagu sloupce
    if ($h && $cnt < $columnCnt) {
      $writer->endTag($tagsArray[$cnt]);
    } elsif ($padding) {
      $writer->endTag("col".sprintf("%0*d", length (scalar @line), $cnt+1));
    } else {
      $writer->endTag("col".($cnt+1));
    }
  }

  # vypsani koncoveho elementu radku
  $writer->endTag($lineTag);
}

# vypsani koncoveho korenoveho elementu XML dokumentu
if(defined $rootTag){ $writer->endTag($rootTag);}

$writer->end();
undef $writer;

# vypsani vysledneho XML dokumentu na vystup
print $output $xmloutput;
undef $output;
exit 0;

##########################################################################################
####  Konec programu
##########################################################################################

##########################################################################################
####  Pomocne funkce
##########################################################################################

#
#  name: openInput
#  Otevre vstupni soubor
#  a na konec prida znaky crlf
#
sub openInput{
  if(defined $input){
    $input = IO::File->new($input,"r")||return -1;
  }else{
    $input = new IO::File;
    $input->fdopen(fileno(STDIN),"r")||return -1;
  }
  binmode( $input, ':utf8' );
  my $tmpinput=join("",$input->getlines());
  $tmpinput = $tmpinput."\015\012";
  my $in;
  open ($in, '<' , \$tmpinput)||return -1;
  undef $input;
  $input = new IO::File;
  $input->fdopen($in,'r')||return -1;
  binmode( $input, ':utf8' );
  return 0;
}

##########################################################################################

#
#  name: openOutput
#  Otevre vystupni soubor
#
sub openOutput{
  if(defined $output){
    $output = IO::File->new($output,"w")||return -1;
  }else{
    $output = new IO::File;
    $output->fdopen(fileno(STDOUT),"w")||return -1;
  }
  binmode( $output, ':utf8' );
  return 0;
}

##########################################################################################

#
#  name: optsParse
#  Zpracuje programove argumenty
#
sub optsParse {
  my %opts = ();
  my $ok = GetOptions(\%opts, 'help+', 'input=s@', 'output=s@', 'n+', 'r=s@', 's=s@',
           'h+', 'l=s@', 'i+', 'start=i@', 'e|error-recovery+', 'missing-value=s@',
           'all-columns+','padding+','validate+');
  if (!$ok || @ARGV > 0){
    print STDERR usage();
    exit 1;
  }

  #Kontrola opakovani parametru
  while ( my ($key, $value) = each(%opts) ) {
    if (ref($value) eq 'ARRAY') {
      if (@$value > 1) {
        #nejaky parametr byl zadán vicekrat
        print STDERR usage();
        print STDERR "Chyba: Vicenasobne zadani parametru\n";
        exit 1;
      };
    }elsif ($value > 1) {
      #nejaky parametr byl zadan vicekrat
      print STDERR usage();
      print STDERR "Chyba: Vicenasobne zadani parametru\n";
      exit 1;
    }
  }

  #Tisk napovedy
  if (defined $opts{'help'}) {
    if(keys %opts != 1) {
      # parametr help muze byt pouze sam
      print STDERR usage();
      print STDERR "Chyba: Parametr help nelze kombinovat\n";
      exit 1;
    }
    print help();
    exit 0;
  }

  if (defined $opts{'input'})   { $input = decode("utf8", ${$opts{'input'}}[0]);}
  if (defined $opts{'output'})  { $output = decode("utf8", ${$opts{'output'}}[0]);}
  if (defined $opts{'n'})       { $generateHeader = 0;}
  if (defined $opts{'h'})       { $h = 1;}
  if (defined $opts{'e'})       { $errorRecovery = 1;}
  if (defined $opts{'padding'}) { $padding = 1;}

  # nastaveni korenoveho elementu
  if (defined $opts{'r'}) {
    $rootTag = decode("utf8", ${$opts{'r'}}[0]);
    # nevalidni XML element
    if ((replaceInvChar(\$rootTag)) || ($rootTag eq '' || $rootTag =~ /^[-.0-9]/)) {
      print STDERR "Chyba: Nevalidni jmeno pro korenovy element\n";
      exit 30;
    }
  }

  # nastaveni separatoru
  if (defined $opts{'s'}) {
    if (${$opts{'s'}}[0] eq 'TAB') {
      $separator = "\t";
    } else {
      $separator = decode("utf8", ${$opts{'s'}}[0]);
      if (length $separator != 1 || $separator eq '"' || $separator eq "\015" ||
          $separator eq "\012") {
        print STDERR "Chyba: Spatny separator sloupcu\n";
        exit 1;
      }
    }
  }

  # nastaveni elementu pro radek
  if (defined $opts{'l'}) {
    $lineTag = decode("utf8", ${$opts{'l'}}[0]);
    # nevalidni XML element
    if ((replaceInvChar(\$lineTag)) || ($lineTag eq '' || $lineTag =~ /^[-.0-9]/)) {
      print STDERR "Chyba: Nevalidni jmeno elementu pro radek\n";
      exit 30;
    }
  }

  # zapnuti cislovani radku
  if (defined $opts{'i'}) {
    if (!$lineTag) {
      print STDERR usage();
      print STDERR "Chyba: Parametr i musi byt kombinovan s parametrem l\n";
      exit 1;
    }
    $lineCnt = 1;
  }

  # nastaveni cislovani
  if (defined $opts{'start'}) {
    if (!$lineCnt) {
      print STDERR usage();
      print STDERR "Chyba: Parametr start musi byt kombinovan s parametrem i\n";
      exit 1;
    }
    $lineCnt = ${$opts{'start'}}[0];
    if ($lineCnt < 0) {
      print STDERR usage();
      print STDERR "Chyba: Citac radku musi byt kladne cislo\n";
      exit 1;
    }
  }

  # doplnovana hodnota u chybejicich sloupcu pri zotaveni z chyb
  if (defined $opts{'missing-value'}) {
    if (!$errorRecovery) {
      print STDERR usage();
      print STDERR "Chyba: Parametr missing-value musi byt kombinovan s parametrem ".
                   "error-recovery\n";
      exit 1;
    }
    $missingVal = decode("utf8", ${$opts{'missing-value'}}[0]);
  }

  # vypis vsech sloupci pri zotaveni z chyb
  if (defined $opts{'all-columns'}) {
    if (!$errorRecovery) {
      print STDERR usage();
      print STDERR "Chyba: Parametr all-columns musi byt kombinovan s parametrem ".
                   "error-recovery\n";
      exit 1;
    }
    $allColumns = 1;
  }

  # zapnuti validace vstupniho csv
  if (defined $opts{'validate'}){
    if ((keys %opts == 2 && !(defined $input || defined $output) ) ||
       ( keys %opts == 3 && !(defined $input && defined $output) ) ||
       ( keys %opts >  3 )) {
      # paramter validate muze byt pouze s parametry input a output
      print STDERR usage();
      print STDERR "Chyba: Parametr validate nelze kombinovat s jinimy parametry".
                   " nez input a output\n";
      exit 1;
    }

    $validate = 1;
  }
}

##########################################################################################

#
#  name: replaceInvChar
#  Nahradi nepovolene znaky XML elementu pomlckou,
#  muze vest na nevalidni  XML element
#  @param reference na retezec s nazvem XML elementu
#  @return Pocet nahrazenych znaku pomlckou
#
sub replaceInvChar {
  my $stringref = shift;
  return ${$stringref} =~ tr/:A-Z_a-z\x{C0}-\x{D6}\x{D8}-\x{F6}\x{F8}-\x{2FF}\x{370}-\x{37D}\x{37F}-\x{1FFF}\x{200C}-\x{200D}\x{2070}-\x{218F}\x{2C00}-\x{2FEF}\x{3001}-\x{D7FE}\-.0-9\x{B7}\x{0300}-\x{036F}\x{203F}-\x{2040}/-/c;
}

##########################################################################################

#
#  name: fillTagArray
#  Naplni pole tagsArray retezci tagu
#  podle prviho radku.
#
sub fillTagArray {
  # vezme prvni radek ze souboru
  my $header = shift @file;
  # projde vsechny sloupce
  for (my $i = 0; $i < $columnCnt; $i++) {
    $tagsArray[$i] = $$header[$i];
    replaceInvChar(\$tagsArray[$i]);

    if ($tagsArray[$i] eq '' || $tagsArray[$i] =~ /^[-.0-9\x{B7}\x{0300}-\x{036F}\x{203F}-\x{2040}]/){
      # nevalidni XML element
      print STDERR "Chyba: Jmeno elementu obsahuje nevalidni znaky\n";
      exit 31;
    };
  }
}