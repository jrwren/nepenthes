#!/usr/bin/perl

$data = $ARGV[0];

while( $data =~ /^([0-9a-fA-F]{2})(.*)$/g )
{
	printf("%c", hex($1));
	$data = $2;
}
