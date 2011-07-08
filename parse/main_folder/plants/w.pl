#!/usr/bin/perl -w



for (100000001..100000010)
{
	my $outf = "id".$_."\.json";
	open FIL, ">", $outf;
	print FIL "\{\n";
	print FIL "\"id\"\: \"".$_."\"\,\n";
	print FIL "\"name_rus\"\: \"\"\,\n";
	print FIL "\"name_eng\"\: \"\"\,\n";
	print FIL "\"description\"\: \"\"\,\n";
	print FIL "\"thumbnail\"\: \"http\:\/\/first_site\.test\/images\/".$_."\_thumbnail\.png\"\,\n";
	print FIL "\"image_url\"\: \"http\:\/\/first_site\.test\/images\/".$_."\_image\.png\"\n";
	print FIL "\}\n";
	close FIL;
}



