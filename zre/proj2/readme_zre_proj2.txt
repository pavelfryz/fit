Ukolem bylo rozpoznat tyto slova: auto, stres, muzika, počítač
Data pochází z nahrávek, které jsem pořídil na cvičení a od kolegů
z cvik, vzorkovací frekvence 8000, 16bit signed,
jeden kanál. 

Slovo   f + k+ p+ z  = počet
auto    24+15+21+19  =  79
stres   28+15+28+20  =  91
muzika  31+15+37+14  =  97
pocitac 27+15+29+20  =  91
celkem                 358

HCopy -T 1 -C cfg/hcopy.conf -S scripts/test.scp
HCopy -T 1 -C cfg/hcopy.conf -S scripts/train.scp
./bin/MakeProtoHMMSet protoconf/models.pcf
HCompV -T 7 -I mlf/train.mlf -l AUTO -m -S scripts/train_htk.scp  -M hmm0 proto/AUTO
HCompV -T 7 -I mlf/train.mlf -l MUZIKA -m -S scripts/train_htk.scp  -M hmm0 proto/MUZIKA
HCompV -T 7 -I mlf/train.mlf -l STRES -m -S scripts/train_htk.scp  -M hmm0 proto/STRES
HCompV -T 7 -I mlf/train.mlf -l POCITAC -m -S scripts/train_htk.scp  -M hmm0 proto/POCITAC
HRest -T 7 -I mlf/train.mlf -l AUTO -S scripts/train_htk.scp -M hmm1 hmm0/AUTO 
HRest -T 7 -I mlf/train.mlf -l MUZIKA -S scripts/train_htk.scp -M hmm1 hmm0/MUZIKA 
HRest -T 7 -I mlf/train.mlf -l STRES -S scripts/train_htk.scp -M hmm1 hmm0/STRES
HRest -T 7 -I mlf/train.mlf -l POCITAC -S scripts/train_htk.scp -M hmm1 hmm0/POCITAC
HParse  net\oldnetwork net\network
HVite -T 1 -d hmm1 -S scripts/test_htk.scp -i mlf/testout.mlf -w net/network dics/dictionary lists/models 
HResults -I mlf/test.mlf lists/models mlf/testout.mlf 
====================== HTK Results Analysis =======================
  Date: Fri May 15 21:57:11 2015
  Ref : mlf/test.mlf
  Rec : mlf/testout.mlf
------------------------ Overall Results --------------------------
SENT: %Correct=93.52 [H=101, S=7, N=108]
WORD: %Corr=93.52, Acc=93.52 [H=101, D=0, S=7, I=0, N=108]
===================================================================

Jelikož nahraná data obsahují šum, který vznikal při cvičení, považuji
rozpoznávání za úspěšné, například nahrávka /data/b/auto-01.raw, která
byla špatně rozpoznána, obsahuje na začátku hlasitý šum, jehož zdrojem bylo
pravděpodobně tlesktnutí pana Novotného.
