#!/usr/bin/perl

$phi = 19200 * 1000 / 3;

@note = split(',', "C,Cs,D,Ds,E,F,Fs,G,Gs,A,As,B");

for $phase (1..2) {
  $nt  = 9;  # A
  $oct = 3;
  $hz  = 220;

  if ($phase == 1) {
    print "\t.section .data\n";
    print "\t.global  note_table\n";
    print "\t.p2align 2\n";
    print "note_table:\n";
  }
  for $i (0..31) {
    $cnt = $phi / $hz;
    $nn = sprintf("%s%d", $note[$nt], $oct);
    if ($phase == 1) {
      printf("\t.word\t%d\t@ %3s, %f\n", $cnt, $nn, $hz);
    } else {
      printf("\t.equ\t%3s, %d\n", $nn, $i);
    }
    $hz = $hz * (2 ** (1.0 / 12));
    if (++$nt > $#note) {
      $nt = 0;
      $oct++;
    }
  }
}

print "\t.global\tn_utick\n";
print "\t.equ\tTIMER_HZ, (1000 * 1000)\n";
print "n_utick:\t.word\tTIMER_HZ * 60 / tempo / tick\n";
