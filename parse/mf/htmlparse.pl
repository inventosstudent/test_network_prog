#!/usr/bin/perl -w
use strict;
use LWP::UserAgent;
use HTTP::Request;
use HTML::Tree;
use LWP;
package IdentityParse;
use base "HTML::Parser";

   
my $img_contents;
my $div_tag  ="";
my $p_tag    ="";
my $rus_name ="";
my $eng_name ="";
my $img      =""; 

# значение флагов для тегов которые ищем
my $div_flag = 0;
my $p_flag = 0;
  
sub start 
{
	my ($self, $tag, $attr, $attrseq, $origtext) = @_;
	if ($tag eq "div" && $attr->{'class'} && $attr->{'class'} eq "headt") 
	{
		$div_flag = 1; #обнаружение тега с русским и английским названием
	} 
	if ($tag eq "img" && $attr->{'alt'} && $attr->{'alt'} =~ $rus_name)
	{		
		$img = $attr->{'src'};
		
	}
	if ($tag eq "p" && $attr->{'class'} && $attr->{'class'} eq "fonta")
	{
		$p_flag = 1;
	}
}
 
sub text 
{
	my ($self, $text) = @_;
    	if ($div_flag) 
	{ 
		$div_tag .=$text; 
		$div_tag =~ s/\(.*\)/""/;
		my @args = split /\s+/, $div_tag;
		$eng_name = pop(@args);
		$rus_name = pop(@args);
	} 
	if ($p_flag)
	{
		$p_tag .=$text;
	}
}
 
sub end 
{
   my ($self, $tag, $origtext) = @_;
   if ($tag eq "div") {$div_flag = 0;}	
   if ($tag eq "p") {$p_flag = 0;}
}

my $id = 100000000;
foreach my $File(</home/max/p_space/parse/mf/fold/*>)
{
	$id = $id + 1;
	$rus_name = "";
	$eng_name = "";
	$img      = "";
	$p_tag    = "";
	print $id."\n";
	my $pp = new IdentityParse;
	$pp->parse_file($File);
	open FIL , ">>", "/home/max/p_space/parse/mf/jsons/".$id.".json";
		print FIL $id."\n\n";
		print FIL $rus_name."\n\n";
		print FIL $eng_name."\n\n";
		print FIL $img."\n\n";
		print FIL $p_tag."\n\n";
	close FIL;
}
