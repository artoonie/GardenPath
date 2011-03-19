#!/usr/bin/perl

$num = $#ARGV == 0 ? $ARGV[$0] : 60;

system("ffmpeg -f image2 -i img%d.jpeg -s qqvga -r $num vid.mpg");
