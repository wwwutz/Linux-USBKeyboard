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

local $| = 1;

# sorry, no forks
while(1) {
  my $c = $k->keycode;
  next if($c < -1);
  if($c == 69) {
    print "NumLock!\n"
  }
  else {
    print Linux::USBKeyboard::code_to_key(0, $c);
  }
}

# vim:ts=2:sw=2:et:sta

