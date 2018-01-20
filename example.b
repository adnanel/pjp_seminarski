a;

fun() {
    return (1);
}

bmain() {
/*
    auto arr [ 10 ];
    auto i 5;
    auto j;
    a = 5;
    a =+ 3;
    a =+ a + 1;
    a =+ a;
    a === a;
*/
    auto i 100;

    while ( i > 25 ) {
        -- i;
    }

    i = '5';

    goto label;

    return (a);
label:
    return (i);
}


abmain (b) {
  auto a, d;
  a=2+b;
}

subr (c) {
  auto c,hahj;
  extrn a;
  c=a;

  if ( c < a ) {
    c = 1;
  } else {
    c = 0;
  }

  c = &a;


  if ( c < a ) {
    c = 10;
  }

  while ( c < a ) {
    c = c + 1;
  }


  c = a < 1 ? fun() : 1 + 2;
}
