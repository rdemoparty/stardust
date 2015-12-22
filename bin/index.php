<?php

$width = 2048;
$height = 2048;
$rawFile = "font.raw";


$content = file_get_contents($rawFile);
print "Read content of size " . strlen($content);

$im = ImageCreateTrueColor($width, $height);


for ($y =0; $y < $height; $y++) {
	for ($x =0; $x < $width; $x++) {

		$offset = ($x + $y*$width)* 4;

		$r = ord($content{$offset++});
		$g = ord($content{$offset++});
		$b = ord($content{$offset++});
		$a = ord($content{$offset++});

		$pixel = $a << 24 + $a << 16 + $a << 8 + $a;
		ImageSetPixel($im, $x, $y, $pixel);
	}
}


ImagePng($im, "font.png");
