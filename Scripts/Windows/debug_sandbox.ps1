param(
    [ValidateSet('debug-runtime', 'release-runtime', 'win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'debug-runtime'
)

. "$PSScriptRoot/common.ps1"

& "$PSScriptRoot/build_sandbox.ps1" -Preset $Preset
Push-Location (Join-Path $PolosRoot "out/bin/$Preset")
try {
    & lldb -o run -- ./sandbox.exe
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
