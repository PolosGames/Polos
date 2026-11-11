param(
    [ValidateSet('win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'win-debug'
)

. "$PSScriptRoot/common.ps1"

& "$PSScriptRoot/build_sandbox.ps1" -Preset $Preset
Push-Location (Join-Path $PolosRoot "out/bin/$Preset")
try {
    & ./sandbox.exe
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
