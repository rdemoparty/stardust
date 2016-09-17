<?php

if (isset($argv))
	evaluateCliCommand($argv);
else
	serveVersions();

function evaluateCliCommand($argv) {
	$command = isset($argv[1]) ? $argv[1] : '';
	switch ($command) {
		case 'new-version':
			addVersion($argv);
			break;
		default:
			printUsage($argv[0]);
			exit(1);
	}
}

function addVersion($argv) {
	if (count($argv) < 6) {
		printUsage($argv[0]);
		exit(1);
	}

	list($program, $option, $version, $archive, $commit, $timestamp) = $argv;


	$newVersion = new stdClass();
	$newVersion->version = $version;
	$newVersion->date = $timestamp;
	$newVersion->commit = $commit;
	$newVersion->filename = $archive;

	$versionsFilename = 'versions.json';
	if (file_exists($versionsFilename)) {
		$versions = json_decode(file_get_contents($versionsFilename));
	}

	if (!isset($versions)) {
		$versions = new stdClass();
		$versions->versions = array();
	}

	array_unshift($versions->versions, $newVersion);

	file_put_contents($versionsFilename, json_encode($versions));
}


function printUsage($programName) {
	print "Usage: " . $programName . " [options]\n\n";
	print "\t" . $programName . " new-version <version> <archive-file> <commit-id> <timestamp>\n";
	print "\n";
}

function serveVersions() {
	header("Access-Control-Allow-Origin: *");

	$versionsFilename = 'versions.json';
	readfile($versionsFilename);
}
