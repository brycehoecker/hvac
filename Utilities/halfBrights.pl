#!/usr/bin/perl

my ($day,$mo,$yr) = (gmtime)[3,4,5];
my $date = sprintf '%4d%02d%02d',$yr+1900,$mo+1,$day;

for (my $i=1; $i<=4; $i+=1) {
    $hvin = "../HVFiles/T${i}_default.hv";
    $hvout = "../HVFiles/T${i}_innerHalfBright_${date}.hv";
    $command = "awk -f halfBrightInner.wk $hvin > $hvout";
    print $command."\n";
    system $command;
    $command = "scp $hvout telectl-t${i}:/usr/local/veritas/SP05-VHV/vhv/HVFiles";
    print $command."\n";
    system $command;

    $hvout = "../HVFiles/T${i}_outerHalfBright_${date}.hv";
    $command = "awk -f halfBrightOuter.wk $hvin > $hvout";
    print $command."\n";
    system $command;
    $command = "scp $hvout telectl-t${i}:/usr/local/veritas/SP05-VHV/vhv/HVFiles";
    print $command."\n";
    system $command;
}
