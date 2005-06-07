#!/usr/bin/perl -w
use Getopt::Std;

# ------------------------------------------------------------------------------
#										options and error checking
# ------------------------------------------------------------------------------

my %options=();
getopts("hX:Y:x:y:t:T:",\%options);

if (defined($options{"h"}) || (scalar(@ARGV) == 0))
{
	die("plot [options] filename1 filename2 ... \noptions are:\n\t-h - this help message\n\t-x number - column number for x axis (default = 1)\n\t-y number - column number for y axis (default = 2)\n\t-X label for y axis (default from log files)\n\t-Y label for y axis (default from log files)\n\t-t specify a title(default comes from the input-files)\n\t-T as -t, except appends default information to it\n");
}

$x = 1;
$y = 2;
if (defined($options{"x"})) { $x = $options{"x"}; }
if (defined($options{"y"})) { $y = $options{"y"}; }

$count = 0;
$plot = "";
foreach $argument (@ARGV)
{

	unless (-e $argument) { die("file '$argument' does not exist\n"); }

# ------------------------------------------------------------------------------
#										get the labels
# ------------------------------------------------------------------------------
	open(ARG, $argument);
	$line = readline(*ARG); # read the first line
	close(ARG);

	$line =~ s/#//g; # clear the # off the front
	@keywords = split(/ /, $line);
	$xlabel = $keywords[$x-1];
	$ylabel = $keywords[$y-1];
	$xlabel =~ s/_/ /g;
	$ylabel =~ s/_/ /g;


# ------------------------------------------------------------------------------
# 							figure out the details of the results
# ------------------------------------------------------------------------------
	# get the type 
	$argument =~ /(.+?)_sim-(.+?)_(.+)/;
	$comment = $1;
	$type = $2;
	$param = $3;

#	print $1."\n";
#	print $2."\n";
#	print $1."\n";
	$param =~ s/_/ /g;

	$comment =~ /.+\/(.+)/;

	if ($type eq "bpred")
	{
		$param =~ /-bpred (\S+) -bpred:\S+ (.*)/;
		$bpred_type = $1;
		$bpred_params = $2;
		if ($bpred_type eq "2lev")
		{
			$bpred_params =~ /(\d+) (\d+) (\d+) (\d+)/;
			$description = "2 Level Adaptive";
			$sub_description = "$2 2nd level entries";
		}
		else # "bimod"
		{
			$description = "Bimodal";
			if (!defined($bpred_params))
			{
				print $argument."\n";
			}
			$sub_description = "$bpred_params byte table";
		}
	}
	else
	{
		#figure out the size of the cache
		$param =~ /.* dl2:(\d+):(\d+):(\d+):.*/;
#		$sets = $1;
		$line_size = $2;
		$associativity = $3;
		$cache_size = $1 * $2 * $3 / (1024*1024);

		if ($associativity != 1) {	$description = "Fully Associative"; }
		else{	$description = "Direct Mapped"; }
		$sub_description = $cache_size."MB - ";
		$sub_description .= "$line_size byte line";

	}
	$names[0][$count] = $comment;
#	$names[1][$count] = $param;
	$names[1][$count] = $description;
	$names[2][$count] = $sub_description;
	$names[3][$count] = $xlabel;
	$names[4][$count] = $ylabel;
	$count++;
}

# figure out the format for the key, output_filename and filename
$xlabel = $names[3][0];
if (defined($options{"X"})) { $xlabel = $options{"X"}." per key"; }
	
$ylabel = $names[4][0];
if (defined($options{"Y"})) { $ylabel = $options{"Y"}." per key"; }

$output_filename = "";;
$all_the_same = 1;
for($i = 0; $i < scalar(@names); $i++)
{
	$same[$i] = 1;
	@this_names = @{$names[$i]};
	foreach $name (@this_names)
	{
		#print "name $i - $name\n";
		if ($name ne $this_names[0])
		{
			$same[$i] = 0;
			$all_the_same = 0;
			last;
		}
	}
	# set the output_filename
	if ($same[$i])
	{
		$output_filename .= $names[$i][0]." - ";
	}
}
#print $output_filename;

chop $output_filename;
chop $output_filename;
chop $output_filename;
if (defined($options{"t"})) { $output_filename = $options{"t"}; }
if (defined($options{"T"})) { $output_filename = $options{"T"}; }
$output_filename =~ s/_+/_/g;
$output_filename =~ s/ /_/g;

$plot = "";
$count = 0;
foreach $argument (@ARGV)
{
#	create the plot string
	$plot .= "\\\n'$argument' using $x:$y title '";
	if (!$same[0])
	{
		$temp = $names[0][$count];
		$temp =~ s/_/ /g;
		$temp =~ /.*\/(.+)/;
		$plot .= $1." - ";
	}
	if (!$same[1])
	{
		$plot .= $names[1][$count]." - ";
	}
	if (!$same[2])
	{
		$plot .= $names[2][$count]." - ";
	}
	chop $plot;
	chop $plot;
	chop $plot;
	$plot .= "',";
	$count++;
}
chop $plot;

#$redirection .= "set grid\n";
#$redirection .= "set title '$output_filename'\n";
if ($all_the_same)
{
	$redirection .= "set nokey\n";
}
else
{
	$redirection .= "set key top left Left reverse\n";
}
#$redirection .= "set xtics 65536\n";
#$redirection .= "set noborder";
$redirection .= "set xrange [4096:4194304]\n";
$redirection .= "set yrange [0:]\n";
$redirection .= "set xtics (4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304)\n";
$redirection .= "set format x \"%8.0f\"\n";
$redirection .= "set logscale x\n";
$redirection .= "set data style linespoints\n";
$redirection .= "set term postscript eps color\n";
$redirection .= "set output 'plots/$output_filename.eps'\n";
$redirection .= "set xlabel '$xlabel'\n";
$redirection .= "set ylabel '$ylabel'\n";
$redirection .= "plot $plot\n";

#print "\n\noutput filename: \"$output_filename\"\n";
# -----------------------------------------------------------------------------
# 							run it
# -----------------------------------------------------------------------------
open(SAVE, "> ./plot_inputs/$output_filename");
print SAVE $redirection;
close(SAVE);

@plot_output = `gnuplot ./plot_inputs/$output_filename 2>&1`; # this also prints to STDERR
print @plot_output;
