#!/bin/bash
# $1 first file
# $2 second file
paste $1 $2 | awk 'BEGIN {print "pix\t1st\t2nd\tdiff\tscaling"} {if ($2 == 0) printf ("%3d\t%4d\t%4d\t%4d\t Divide by zero\n", $1, $2, $4, $4-$2); else {printf ("%3d\t%4d\t%4d\t%4d\t%5.1f %%", $1, $2, $4, $4-$2, $4/$2*100); if($4/$2*100 > 85 || $4/$2*100 < 75) printf ("\t <--\n"); else printf("\n")}}' | less
