#!/usr/bin/perl

use strict;

use IO::Handle;
use Time::HiRes;

# executable of Vampire
my $vampire = "/home/voronkov/Vampire/vampire --mode kif --backward_subsumption on";
# my $kb = "/home/voronkov/Vampire/tests/XML/Merge.kif";
my $kb = "/home/voronkov/Vampire/Merge34.kif";
my $dummy = "/home/voronkov/Vampire/tests/empty";

my $dummyFilePlusTestFile = "/home/voronkov/Vampire/q";

sub test {
  my ($file) = @_;

  print "$file\n";
  print "A ";
  system "(time $vampire $kb < $dummy > /dev/null) 2>&1 | grep user";
  system "cat $dummy $file > $dummyFilePlusTestFile";
  print "B ";
  system
    "(time $vampire $kb < $dummyFilePlusTestFile > /dev/null) 2>&1 | grep user";
  print "\n";

}

foreach my $file (<tests/XML/*.xml>) {
  test ($file);
}

