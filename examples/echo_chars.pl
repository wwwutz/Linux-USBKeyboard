#!/usr/bin/perl

use warnings;
use strict;

use lib 'lib';

use Linux::USBKeyboard;

# e.g. 0x0e6a, 0x0001
(@ARGV) or
  die 'run `lsusb` to determine your vendor_id, product_id';
my ($vendor, $product) = map({hex($_)} @ARGV);
$product = 1 unless(defined($product));

my $k = eval {Linux::USBKeyboard->new($vendor, $product)};
if($@) { die "$@ - you might have the wrong permissions or address"; }

if(0) {
  print $k->_char, ".1\n";
  print $k->_char, ".2\n";
  print $k->_char, ".3\n";
  print $k->_char, ".4\n";
  print $k->_char, ".5\n";
  print $k->_char, ".6\n";
}
else {
  local $| = 1;
  while(1) {
    my $c = $k->char;
    #print $c, '(', length($c), ')';
    print $c;
  }
}

# vim:ts=2:sw=2:et:sta
