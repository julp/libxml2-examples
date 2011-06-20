#!/usr/bin/env php
<?php
function random_string($ensemble, $longueur)
{
    return substr(str_shuffle($ensemble), 0, 6);
}

echo random_string('ABCDEFGHIJKLMNOPQRSTUVWXYZ', 6), PHP_EOL;
