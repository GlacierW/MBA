# Script that synchronizes the local library dependencies
#
# Version: 20161107

$GitUrlPrefix = "https://github.com/libyal"
$LocalLibs = "libbfio libcdata libcerror libcfile libclocale libcnotify libcpath libcsplit libcsystem libcthreads libfcache libfdata libfdatetime libfwnt libuna"
$LocalLibs = ${LocalLibs} -split " "

foreach (${LocalLib} in ${LocalLibs})
{
	# PowerShell will raise NativeCommandError if git writes to stdout or stderr
	# therefore 2>&1 is added and the output is stored in a variable.
	$Output = Invoke-Expression -Command "git clone ${GitUrlPrefix}/${LocalLib}.git ${LocalLib}-${pid} 2>&1"

	If (Test-Path ${LocalLib}-${pid})
	{
		$LocalLibVersion = Get-Content -Path ${LocalLib}-${pid}\configure.ac | select -skip 4 -first 1 | % { $_ -Replace " \[","" } | % { $_ -Replace "\],","" }

		If (Test-Path ${LocalLib})
		{
			Remove-Item -Path ${LocalLib} -Force -Recurse
		}
		New-Item -ItemType directory -Path ${LocalLib} -Force | Out-Null

		If (Test-Path ${LocalLib})
		{
			Copy-Item -Path ${LocalLib}-${pid}\${LocalLib}\*.[chly] -Destination ${LocalLib}\
			Get-Content -Path ${LocalLib}-${pid}\${LocalLib}\${LocalLib}_definitions.h.in | % { $_ -Replace "@VERSION@",${LocalLibVersion} } > ${LocalLib}\${LocalLib}_definitions.h
		}
		Remove-Item -Path ${LocalLib}-${pid} -Force -Recurse
	}
}

