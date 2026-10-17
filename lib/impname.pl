#!/usr/bin/perl
# impname.pl <lib> <nametype>: turn lib /def ordinal imports into by-name imports.
# The .def "@N" then only sets the hint. nametype: 1 = name as is, 3 = undecorated.
use strict;
my ($p, $t) = @ARGV;
open(my $f, '<:raw', $p) or die "$p: $!";
my $d = do { local $/; <$f> };
close $f;
my ($i, $n) = (0, 0);
while (($i = index($d, "\x00\x00\xff\xff", $i)) >= 0) {
    if (substr($d, $i + 6, 2) eq "\x4c\x01") {  # short import header, x86
        my $ty = unpack('v', substr($d, $i + 18, 2));
        if ((($ty >> 2) & 7) == 0) {
            substr($d, $i + 18, 2) = pack('v', ($ty & ~0x1c) | ($t << 2));
            $n++;
        }
    }
    $i += 4;
}
open($f, '>:raw', $p) or die "$p: $!";
print $f $d;
close $f;
print "$p: $n imports\n";
