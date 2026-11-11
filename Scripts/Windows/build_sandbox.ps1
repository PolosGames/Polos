param(
    [ValidateSet('win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'win-debug'
)

. "$PSScriptRoot/common.ps1"

$BuildDir = Join-Path $PolosRoot "out/build/$Preset"
$BinDir = Join-Path $PolosRoot "out/bin/$Preset"

Initialize-PolosBuild $Preset $BuildDir $BinDir 'ON'
Invoke-CMake --build $BuildDir --target sandbox
Invoke-CMake --install $BuildDir --component Runtime
