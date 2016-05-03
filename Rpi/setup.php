#!/usr/bin/php
<?php

//array of all gpio pin numbers 
$array = array(2,3,4,14,15,18,17,27,22,23,24,10,9,11,7,25,8,5,6,12,13,16,19,20,21,26);

//loop through all gpios and set them to out
for($i=0;$i<count($array);$i++)
{
    shell_exec("gpio -g mode ".$array[$i]." out");
    shell_exec("gpio -g write ".$array[$i]." 0");
}

// Individualy set gpios 26,27,28,29 to mode in and 0
  shell_exec("gpio -g mode 26 in");
  shell_exec("gpio -g write 26 0");
  shell_exec("gpio -g mode 27 in");
  shell_exec("gpio -g write 27 0");
  shell_exec("gpio -g mode 28 in");
  shell_exec("gpio -g write 28 0");
  shell_exec("gpio -g mode 29 in");
  shell_exec("gpio -g write 29 0");

?>
