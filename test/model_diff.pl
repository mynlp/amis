#!/usr/bin/perl

$ret = 0;

$prec = 0.1 ** $ARGV[0];

open(MODEL1, $ARGV[1]) or die "Can't open file \"$ARGV[0]\"\n";
open(MODEL2, $ARGV[2]) or die "Can't open grades: \"$ARGV[1]\"\n";

while ($line1 = <MODEL1>) {
    if ($line2 = <MODEL2>) {
	($feature1, $weight1) = split(" ", $line1);
	($feature2, $weight2) = split(" ", $line2);

	if ($feature1 != $feature2) {
	    $ret = -1;
	    print "< $feature1\n";
	    print "> $feature2\n";
	}
	else {
	    $diff = ($weight1 / $weight2) - 1.0;
	    if (abs($diff) > $prec) {
		$ret = -1;
		print "< $feature1 $weight1\n";
		print "> $feature2 $weight2\n";
	    }
	}
    }
    else {
	$ret = -1;
	print "< $line1";
	while ($line1 = <MODEL1>) {
	    print "< $line1";
	}
    }
}

while ($line2 = <MODEL2>) {
    $ret = -1;
    print "> $line2";
}

exit $ret;
