param(
    [ValidateSet('win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'win-release'
)

. "$PSScriptRoot/common.ps1"

$BuildDir = Join-Path $PolosRoot "out/build/sdk/$Preset"
$PackageDir = Join-Path ([System.IO.Path]::GetTempPath()) ([guid]::NewGuid().ToString())
$Version = (Get-Content (Join-Path $PolosRoot 'VERSION') -Raw).Trim()
$ArchiveDir = Join-Path $PolosRoot 'out/packages'
$Archive = Join-Path $ArchiveDir "polos-sdk-$Version-$Preset.zip"

Initialize-PolosBuild $Preset $BuildDir (Join-Path $PolosRoot "out/sdk/$Preset") 'OFF'
Invoke-CMake --build $BuildDir --target polos
New-Item -ItemType Directory -Path $PackageDir, $ArchiveDir -Force | Out-Null
try {
    Invoke-CMake --install $BuildDir --prefix $PackageDir --component Runtime
    Invoke-CMake --install $BuildDir --prefix $PackageDir --component SDK
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
