#!/usr/bin/python3
# -*- coding: utf-8 -*-
#SYN:xfryzp00

import re
import sys
import argparse

#
#  name: insertTagsToText
#  Vlozi tagy z asociativniho pole do vstupniho retezce na dane pozice.
#  @param Vstupni retezec
#  @param Asociativni pole tvaru posize:tag (key:value)
#  @return Retezec s vlozenymi tagy
#
def insertTagsToText(text,tagDict):
  keys = tagDict.keys();
  keys = sorted(keys,reverse=True)
  for position in keys:
    text = text[:position]+tagDict[position]+text[position:]
  return text

#
#  name: insertTagsToDict
#  Najde vsechny vyskyty retezcu, ktere vyhovuji zadanemu regularnimu vyrazu
#  a do asociativniho pole vlozi startTag a endTag na danou pozici
#  @param Vstupni retezec
#  @param Asociativni pole tvaru posize:tag (key:value)
#  @param Mnozina (pocatecniTagy,koncoveTagy,regularniVyraz)
#
def insertTagsToDict(text,tagDict,StEtRe):
  (startTag,endTag,regExpr)=StEtRe
  try:
    matches = re.finditer(regExpr,text,re.S)
  except:
    print("Chyba: Chybny regularni vyraz",file=sys.stderr)
    exit(4)
  for match in matches:
    s = match.start()
    e = match.end()
    if s!=e:
      tagDict[s]=tagDict.get(s,'')+startTag
      tagDict[e]=endTag+tagDict.get(e,'')

#
#  name: evalEscape
#  Funkce vraci retezec kterym ma byt nahrazeno %<znak>
#  Funkce pouzita pro volani re.sub
#  @param Objekt match
#  @return Nahrazovaci retezec
#
def evalEscape(match):
  retDict = {r's' : r'[ \t\n\r\f\v]',
             r'a' : r'.',
             r'd' : r'[0-9]',
             r'l' : r'[a-z]',
             r'L' : r'[A-Z]',
             r'w' : r'[a-zA-Z]',
             r'W' : r'[a-zA-Z0-9]',
             r't' : r'\t',
             r'n' : r'\n',
             r'.' : r'\.',
             r'|' : r'\|',
             r'!' : r'!',
             r'*' : r'\*',
             r'+' : r'\+',
             r'(' : r'\(',
             r')' : r'\)',
             r'%' : r'%',
  }
  return retDict[match.group(1)]

#
#  name: evalNegation
#  Funkce vraci retezec kterym ma byt nahrazeno !<znak>
#  pripadne !%<znak>
#  Funkce pouzita pro volani re.sub
#  @param Objekt match
#  @return Nahrazovaci retezec
#
def evalNegation(match):
  retDict = {r'%s' : r'[^ \t\n\r\f\v]',
             r'%a' : r'[^\s\S]',
             r'%d' : r'[^0-9]',
             r'%l' : r'[^a-z]',
             r'%L' : r'[^A-Z]',
             r'%w' : r'[^a-zA-Z]',
             r'%W' : r'[^a-zA-Z0-9]',
             r'%t' : r'[^\t]',
             r'%n' : r'[^\n]',
             r'%.' : r'[^.]',
             r'%|' : r'[^|]',
             r'%!' : r'[^!]',
             r'%*' : r'[^*]',
             r'%+' : r'[^+]',
             r'%(' : r'[^(]',
             r'%)' : r'[^)]',
             r'%%' : r'[^%%]',
             r'\]' : r'[^\]]',
             r'\[' : r'[^\[]',
             r'\^' : r'[^^]',
             r'\$' : r'[^$]',
             r'\\' : r'[^\\]',
             r'\?' : r'[^?]',
             r'\{' : r'[^{]',
             r'\}' : r'[^}]',
             r']'  : r'!]',
  }
  return match.group(1)+retDict.get(match.group(3),'[^'+match.group(3)+']')

#
#  name: evalStarPlus
#  Funkce vraci retezec kterym ma byt nahrazena
#  * a +
#  Funkce pouzita pro volani re.sub
#  @param Objekt match
#  @return Nahrazovaci retezec
#
def evalStarPlus(match):
  if '*' in match.group(3):
    return match.group(1)+'*'
  return match.group(1)+'+'

#
#  name: evalBraceStarPlus
#  Funkce vraci retezec kterym ma byt nahrazen
#  retezec obsahujici *+ za sebou v zavorkach
#  Funkce pouzita pro volani re.sub
#  @param Objekt match
#  @return Nahrazovaci retezec
#
def evalBraceStarPlus(match):
  count = len(match.group(3))
  operator = match.group(5)
  tail = ''
  for c in match.group(6):
    if count < 0:
      tail = tail + c
    elif c == ')':
      tail = tail + c
      count = count - 1
    elif c == '*':
      operator = '*'
    elif c == '+' and operator != '*':
      operator = '+'
  return match.group(1)+match.group(3)+match.group(4)+operator+tail

#
#  name: checkRegExpr
#  Jednoduchy test formatu regularniho vyrazu
#  True->Chyba, False->OK
#  @param Regularni vyraz
#  @return Zda je regularni vyraz korektni
#
def checkRegExpr(regExpr):
  return re.search(r"""
               (?<!%)(%%)*             (
                 \.[.|*+)]             | # za . je spatny znak
                 \|[.|*+)]             | # za | je spatny znak
                 ![.|!*+()]            | # za ! je spatny znak
                 \([.|*+)]             | # za ( je spatny znak
                 %[^.|!*+()%sadlLwWtn] | # za % je spatny znak
                 [.|!(%]$              ) # neukoncene specialni symboly
                                       |
               ^[.|*+)]                | # RV nemuze zacinat . | * + )
               ^$                        # RV nemuze byt prazdny
               """, regExpr, re.X) != None

#
#  name: parseRegExpr
#  Prevede vstupni RV do RV pythonu
#  @param Regularni vyraz
#  @return Regularni vyraz v pythonu
#
def parseRegExpr(regExpr):
  # Odstrani vsechny tecky znacici konkatenaci
  regExpr = re.subn(r'(?<!%)((%%)*)\.',r'\1',regExpr)[0]
  # Nahradi posloupnost *+ jednou * nebo +
  regExpr = re.subn(r'(?<!%)((%%)*)([*+]+)',evalStarPlus,regExpr)[0]
  # Nahradi +* mezi zavorkami jendou * nebo +
  regExpr = re.subn(r'(?<!%)((%%)*)(\(+)(!%.|!.|%.|.)([*+]?)([)+*]*)',evalBraceStarPlus,regExpr)[0]
  # Escapuje vsechny znaky ktere maji v pythonu specialni vyznam,
  # krome znaku ktere maji specialni vyznam ve vstupnim RV
  regExpr = re.subn(r'([][^$\\?{}])',r'\\\1',regExpr)[0]
  # Nahradi ! odpovidajicim RV v pythonu
  regExpr = re.subn(r'(?<!%)((%%)*)!(%.|\\.|.)',evalNegation,regExpr)[0]
  regExpr = re.subn(r'(?<!%)((%%)*)!(%.|\\.|.)',evalNegation,regExpr)[0]
  # Nahradi % odpovidajicim RV v pythonu
  regExpr = re.subn(r'%(.)',evalEscape,regExpr)[0]
  return regExpr

#
#  name: parseLine
#  Rozlozi radek formatovaciho souboru na retezec s pocatecnimi
#  tagy, retezec s koncovymi tagy a regularni vyraz.
#  @param line Jeden radek formatovaciho souboru
#  @return Mnozinu (pocatecniTagy,koncoveTagy,regulraniVyraz)
#
def parseLine(line):
  line = line.split('\t',1)
  if len(line) == 1:
    print("Chyba: Chybi formatovaci retezec",file=sys.stderr)
    exit(4)
  regExpr = line[0];
  if checkRegExpr(regExpr):
    print("Chyba: Chybny regularni vyraz",file=sys.stderr)
    exit(4)
  regExpr = parseRegExpr(regExpr)
  formatList = line[1].split(',')
  startTag = ''
  endTag = ''
  for format in formatList:
    format = format.strip(' \t\n')
    size = re.match(r'size:([1-7])',format)
    color = re.match(r'color:([0-9A-F]{6})',format)
    if format == 'bold':
      startTag = startTag+'<b>'
      endTag = '</b>'+endTag
    elif format == 'italic':
      startTag = startTag+'<i>'
      endTag = '</i>'+endTag
    elif format == 'underline':
      startTag = startTag+'<u>'
      endTag = '</u>'+endTag
    elif format == 'teletype':
      startTag = startTag+'<tt>'
      endTag = '</tt>'+endTag
    elif size:
      startTag = startTag+'<font size='+size.group(1)+'>'
      endTag = '</font>'+endTag
    elif color:
      startTag = startTag+'<font color=#'+color.group(1)+'>'
      endTag = '</font>'+endTag
    else:
      print("Chyba: Chybny formatovaci retezec",file=sys.stderr)
      exit(4)
  return (startTag,endTag,regExpr)

#
#  name: parseArgs
#  Zpracuje parametry
#  @return Zpracovane parametry
#
def parseArgs():
  p = argparse.ArgumentParser(add_help=False, description='Program prida formatovaci tagy dle '+
    'zadaneho formatovaci souboru do vstupniho souboru. Formatovaci soubor obsahuje na kazdem radku ' +
    'regularni vyraz nasledovany tabulatorem a formatovacimi retezci oddelenymi jednou carkou.')
  p.add_argument('--help', action='count', dest='help', default=0, help='Vytiskne tuto napovedu')
  p.add_argument('--input', action='append', dest='input', default=[],metavar='SOUBOR', help='Nastavi vstup na zadany soubor')
  p.add_argument('--output', action='append', dest='output', default=[],metavar='SOUBOR', help='Nastavi vystup do zadaneho souboru')
  p.add_argument('--format', action='append', dest='format', default=[],metavar='SOUBOR', help='Nastavi formatovaci soubor')
  p.add_argument('--br', action='count', dest='br', default=0, help='Prida tag <br /> na konec radku')
  try:
    args = p.parse_args()
  except:
    sys.exit(1)
  if args.help == 1 and not (args.input or args.output or args.format or args.br):
    p.print_help()
    sys.exit(0)

  if args.help > 0 or len(args.input) > 1 or len(args.output) > 1 or len(args.format) > 1 or args.br > 1:
    p.print_usage(sys.stderr)
    print("Chyba: Zakazana kombinace parametru",file=sys.stderr)
    sys.exit(1)
  return args

#
#  name: openFiles
#  Otevre vstupni a vystupni soubory
#  @param a Parametry s nazvy souboru
#  @return Mnozinu souboru (input,output,format)
#
def openFiles(a):
  input=sys.stdin
  output=sys.stdout
  format=None
  if a.input:
    try:
      input = open(a.input[0], mode='r', encoding='utf8')
    except:
      print("Chyba: Nepodarilo se otevrit vstupni soubor",file=sys.stderr)
      sys.exit(2)
  if a.output:
    try:
      output = open(a.output[0], mode='w', encoding='utf8')
    except:
      print("Chyba: Nepodarilo se otevrit vystupni soubor",file=sys.stderr)
      sys.exit(3)
  if a.format:
    try:
      format = open(a.format[0], mode='r', encoding='utf8')
    except:
      print("Chyba: Nepodarilo se otevrit formatovaci soubor",file=sys.stderr)
      sys.exit(2)
  return (input,output,format)

#
#  name: main
#  Hlavni funkce programu
#
def main():
  args = parseArgs()
  (input,output,format) = openFiles(args)

  text = input.readlines()
  text = ''.join(text)

  tagDict = {}
  if format:
    for line in format:
      insertTagsToDict(text,tagDict,parseLine(line))

  text=insertTagsToText(text,tagDict)
  if args.br:
    text = text.replace('\n','<br />\n')

  print(text,file=output,end='');
  sys.exit(0)

main()
