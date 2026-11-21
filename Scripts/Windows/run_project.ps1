param(
    [Parameter(Mandatory, Position = 0)]
    [ValidatePattern('^[A-Za-z0-9_][A-Za-z0-9_.+-]*$')]
    [string]$Project,

    [ValidateSet('debug-runtime', 'release-runtime', 'win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'debug-runtime'
)

. "$PSScriptRoot/common.ps1"

$ProjectDir = Join-Path $PolosRoot "Examples/$Project"
if (-not (Test-Path -LiteralPath (Join-Path $ProjectDir 'CMakeLists.txt') -PathType Leaf)) {
    throw "Unknown Polos example: $Project"
}

$BuildDir = Join-Path $PolosRoot "out/build/projects/$Preset/$Project"
$BinDir = Join-Path $PolosRoot "out/bin/$Preset/$Project"
Initialize-PolosBuild $Preset $BuildDir $BinDir 'ON' $Project
Invoke-PolosResourcePreprocessor $BuildDir $ProjectDir
Invoke-CMake --build $BuildDir --target install-runtime

Push-Location $BinDir
try {
    & "./$Project.exe"
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
