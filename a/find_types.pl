#! /usr/bin/perl

use strict;
use warnings;

my %types;
while(<>)
{
	next unless /([a-zA-Z0-9_]+_[tT])[^a-zA-Z0-9_]/;
	$types{$1} = 1;
}
foreach my $key (sort(keys(%types)))
{
	print "$key\n";
}
