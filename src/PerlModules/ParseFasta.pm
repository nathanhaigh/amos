# $Id$

# ParseFasta.pm
#

package TIGR::ParseFasta;
{

=head1 NAME

ParseFasta - class for reading a fasta-like formatted file

The assumption is that the file contains a set of multi-line records separated
by single-line headers starting with a specific separator (by default '>')

=head1 SYNOPSIS

use ParseFasta;
my $parser = new ParseFasta(\*STDIN);

while (my ($head, $data) = $parser->getRecord()){
   ...
}

=head1 DESCRIPTION

This module iterates through a fasta-like file retrieving the records in 
(header, data) pairs. When creating a new parser, the user may specify
both the prefix (as a string) that indicates a header (record separator) line
(default is '>'), and a line separator that is used when concatenating the
lines in the input forming the body of each record.  This separator is useful,
for example, when parsing a .qual file where the quality values are separated
by spaces.

=cut

    use strict;

    sub new();
    sub getRecord();

=over

=item $parser = new ParseFasta($file, $head, $sep) ;

Creates a new parser object reading from file $file and using a specific
header separator ($header - default '>') and a line separator 
($sep - default '')

=cut

sub new()
{
    my $pkg = shift;
    my $file = shift;
    my $headsep = shift;
    my $linesep = shift;

    my $self = {};
    bless $self;

    $self->{headsep} = '>'; 
    $self->{headsep} = $headsep if defined $headsep;
    $self->{linesep} = ''; 
    $self->{linesep} = $linesep if defined $linesep;
    $self->{file} = $file;

    $self->{buf} = <$file>;
    if (! defined $self->{buf}){
	print STDERR "File appears empty\n";
	return undef;
#	die("File appears empty\n");
    }
    if ($self->{buf} !~ /^$self->{headsep}/){
	print STDERR "File doesn't start with a header: $headsep\n";
	return undef;
#	die ("File doesn't start with a header: $headsep\n");
    }
    chomp $self->{buf};
#    print STDERR "GOT a line $buf\n";
    return $self;
}

=item ($head, $data) = $parser->getRecord();

Reads a record into $head and $data.  If no more records remain returns undef.

=cut

sub getRecord()
{
    my $self = shift;
    my $head;
    my $data;
    my $file = $self->{file};
    
    if ($self->{buf} !~ /^$self->{headsep}/){ # record must start with a separator
	return ();
    }
    $head = $self->{buf};
    $head =~ s/^$self->{headsep}//;
    $self->{buf} = <$file>;
    chomp $self->{buf};
    while (defined $self->{buf} && $self->{buf} !~ /^$self->{headsep}/){
	$data .= $self->{buf} . $self->{linesep};
	$self->{buf} = <$file>;
	chomp $self->{buf};
    }
    return ($head, $data);
}

}

1;
