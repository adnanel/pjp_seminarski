/*
Imamo globalne varijablu a tipa int, i garr tipa int[10]
*/
a;
garr[ 10 ];

/* pomocna funkcija za provjeru ispravnosti koda */
assert(expected, actual, poruka) {
    if ( expected != actual ) {
        printf("GRESKA!!! %s. Ocekivao vrijednost %d, dobio %d  ", poruka, expected, actual);
    }
}


bmain() {
    /*
    Ako zelimo da iskoristimo globalnu varijablu unutar neke funkcije, moramo je deklarisati
    sa kljucnom rijeci "extrn". U protivnom ce kompajler javiti semanticku gresku.
    */
    extrn a;

    /*
    Deklaraciju lokalnih varijabli radimo uz kljucnu rijec auto.
    Primjetite kako se inicijalizira varijaba "broj" na vrijednost 5.
    */
    auto broj 5, niz[10]; /* niz od 10 integera */

    /*
    proba petlje i postfiksnog operatora --,
    a ce na kraju biti 0 a elementi niza ce se
    inicijalizirati na sekvencu 14,13,12,11,9,8,7,6,5
    */
    a = 10;
    while ( a-- > 0 ) {
        niz[a] = broj + a;
        assert(broj + a, niz[a], "Element niza nije postavljen na ispravnu vrijednost?");
    }
    assert(-1, a, "A na kraju petlje nije -1?");

    auto b a;
return (a);
    /*
    B podrzava razne unarne operatore. Slijedi proba istih.
    */
    ++a;
    a++;
    --a;
    a--;


    a = 0;
    a === 1;
    a =!= 1;
    a =<= 1;
    a =>= 1;
    a =& 1;
    a =| 1;
    a =/ 1;
    a =* 1;
}