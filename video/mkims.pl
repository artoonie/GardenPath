#!/usr/bin/perl

for($i=3; $i<=240; $i+=2) {
   system("cp", "img1.jpeg", "img$i.jpeg");
}
for($i=4; $i<=240; $i+=2) {
   system("cp", "img2.jpeg", "img$i.jpeg");
}
