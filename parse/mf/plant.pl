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

my $url = "http://iplants.ru/encikl.php";
my $lwp = LWP::UserAgent->new;
my $r = HTTP::Request->new("GET","$url");
$r->header(User_Agent => "User-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9.2.18) Gecko/20110615 Ubuntu/10.10 (maverick) Firefox/3.6.18",
			Connection => "keep-alive",
);
my $response = $lwp->request($r);

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
	if ($tag eq "option" && $attr->{'value'}=~ /htm/)
	{
		my $mini_url =  "http://iplants.ru/".$attr->{'value'};
		$r=HTTP::Request->new("GET","$mini_url");
		$r->header(User_Agent => "User-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9.2.18) Gecko/20110615 Ubuntu/10.10 (maverick) Firefox/3.6.18",
					Connection => "keep-alive");
		$response = $lwp->request($r);
		$_=$response->content;#получили описание растения
		open FIL, ">>", "./fold/".$attr->{'value'}."l";
		print FIL $_;
		close FIL;
	}

}
 
sub text 
{
	my ($self, $text) = @_;
    	if ($div_flag) 
	{ 
		$div_tag .=$text; 
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
if ($response->is_success)
{
	my $_ = $response->content;
	open FIL, ">>", "outfile.html";
		print FIL $_;
	close FIL;
	my $p = new IdentityParse; 
	$p->parse_file("/home/max/p_space/parse/mf/outfile.html");

	die "";
}


