%delka linearniho seznamu
delka([],0).
delka([_|T],S) :- delka(T,SS), S is SS + 1.

%je prvek clenem lin. seznamu?
jePrvek([X|_],X).
jePrvek([_|T],X) :- jePrvek(T,X).

%spojeni dvou linearnich seznamu
spoj([],L,L).
spoj([H|T],L,[H|TT]) :- spoj(T,L,TT).

%doplnte nasledujici predikaty:

% Vytvorte funkci pro rozdeleni linearniho seznamu na poloviny
% divide_half(INPUT,HALF1,HALF2)
divide_half(I,H1,H2) :- spoj(H1,H2,I),delka(H1,DH1),delka(H2,DH2),(DH1 is DH2;DH1 is DH2 + 1).

% Vytvorte funkci pro odstraneni obecneho prvku z obecneho seznamu
% remove_item_GEN(INPUT,ITEM,OUTPUT)
remove_item_GEN([],_,[]).
remove_item_GEN([H|T],H,O) :- remove_item_GEN(T,H,O), !.
remove_item_GEN([H|T],I,O) :- is_list(H),remove_item_GEN(H,I,O1), remove_item_GEN(T,I,O2), spoj([O1],O2,O),!.
remove_item_GEN([H|T],I,O) :- remove_item_GEN(T,I,O2), spoj([H],O2,O).

% Vytvorte funkci pro reverzaci obecneho seznamu
% reverse_GEN(INPUT,OUTPUT)
reverse_GEN([],[]).
reverse_GEN([H|T],O) :- is_list(H),reverse_GEN(H,HR), reverse_GEN(T,TR), spoj(TR,[HR],O),!.
reverse_GEN([H|T],O) :- reverse_GEN(T,TR), spoj(TR,[H],O).

% Vytvorte funkci pro vlozeni prvku na n-tou pozici linearniho seznamu
% insert_pos(LIST,POSITION,ITEM,OUTPUT)
insert_pos(L,P,I,O) :- spoj(H,T,L),delka(H,D),spoj(H,[I],HO),spoj(HO,T,O),P is D+1.

% Vytvorte funkci pro inkrementaci kazdeho prvku obecneho seznamu o hodnotu hloubky zanoreni prvku
% increment_general(INPUT,OUTPUT)
% input [0,0,[0]] -> output [1,1,[2]]

increment_general(I,O) :- increment(I,1,O).
increment([],_,[]).
increment([H|T],D,O) :- is_list(H),increment(H,D+1,O1), increment(T,D,O2), spoj([O1],O2,O),!.
increment([H|T],D,O) :- DH is D+H, increment(T,D,O2), spoj([DH],O2,O).
