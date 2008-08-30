#!/usr/bin/perl

# echo keypresses with open_keys()

use warnings;
use strict;

use lib 'lib';

use Linux::USBKeyboard;

# e.g. 0x0e6a, 0x0001
my @args = @ARGV;
(@args) or
  die 'run `lsusb` to determine your vendor_id, product_id';
my ($vendor, $product) = map({hex($_)}
  $#args ? @args : split(/:/, $args[0]));

$product = 1 unless(defined($product));

warn "getting $vendor, $product\n";
my $k = eval {Linux::USBKeyboard->open_keys($vendor, $product)};
if($@) { die "$@ - you might have the wrong permissions or address"; }

$SIG{INT} = sub { warn "bye\n"; exit};
while(my $line = <$k>) {
  print $line;
}

# vim:ts=2:sw=2:et:sta

