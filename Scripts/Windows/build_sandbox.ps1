param(
    [ValidateSet('debug-runtime', 'release-runtime', 'win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'debug-runtime'
)

. "$PSScriptRoot/common.ps1"

$BuildDir = Join-Path $PolosRoot "out/build/$Preset"
$BinDir = Join-Path $PolosRoot "out/bin/$Preset"

Initialize-PolosBuild $Preset $BuildDir $BinDir 'ON' 'sandbox'
Invoke-PolosResourcePreprocessor $BuildDir (Join-Path $PolosRoot 'Examples/sandbox')
Invoke-CMake --build $BuildDir --target install-runtime
