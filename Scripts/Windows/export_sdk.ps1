param(
    [ValidateSet('debug-sdk', 'release-sdk', 'win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'release-sdk'
)

. "$PSScriptRoot/common.ps1"

$BuildDir = Join-Path $PolosRoot "out/build/sdk/$Preset"
$PackageDir = Join-Path ([System.IO.Path]::GetTempPath()) ([guid]::NewGuid().ToString())
$Version = (Get-Content (Join-Path $PolosRoot 'VERSION') -Raw).Trim()
$ArchiveDir = Join-Path $PolosRoot 'out/packages'
$Archive = Join-Path $ArchiveDir "polos-sdk-$Version-$Preset.zip"

New-Item -ItemType Directory -Path $PackageDir, $ArchiveDir -Force | Out-Null
try {
    Initialize-PolosBuild $Preset $BuildDir $PackageDir 'OFF'
    Invoke-CMake --build $BuildDir --target install-sdk
    Push-Location $PackageDir
    try {
        Invoke-CMake -E tar cf $Archive --format=zip .
    } finally {
        Pop-Location
    }
} finally {
    Remove-Item -LiteralPath $PackageDir -Recurse -Force
}
Write-Output $Archive
