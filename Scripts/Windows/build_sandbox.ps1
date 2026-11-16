param(
    [ValidateSet('debug-runtime', 'release-runtime', 'win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'debug-runtime'
)

. "$PSScriptRoot/common.ps1"

$BuildDir = Join-Path $PolosRoot "out/build/$Preset"
$BinDir = Join-Path $PolosRoot "out/bin/$Preset"

Initialize-PolosBuild $Preset $BuildDir $BinDir 'ON'
Invoke-CMake --build $BuildDir --target ResourcePreprocessor
$Preprocessor = Join-Path $BuildDir 'Tools/ResourcePreprocessor/ResourcePreprocessor.exe'
& $Preprocessor (Join-Path $PolosRoot 'Examples/sandbox')
if ($LASTEXITCODE -ne 0) {
    throw "ResourcePreprocessor exited with code $LASTEXITCODE."
}
Invoke-CMake --build $BuildDir --target install-runtime
