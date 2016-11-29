param($installPath, $toolsPath, $package, $project)

$dir = split-Path $MyInvocation.MyCommand.Path;
& "$dir\37f3f9923e954d008a04bfbb1b5beec3.ps1" $installPath $toolsPath $package $project;

