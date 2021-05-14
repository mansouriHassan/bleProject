<?php
	$myfile = fopen("file.txt", "w") or die("Unable to open file!");

	$name = $_POST['name'];

	if ($name == null) {
    		$name = 'guest';
	}

	$message = $_POST['message'];

	if ($message == null) {
    		$message = 'hello there';
	}


	echo "$name says: $message";

	fwrite($myfile, $name);
        fwrite($myfile, "\n");
	fwrite($myfile, $message);
	fwrite($myfile, "\n");
	fclose($myfile);
?>
