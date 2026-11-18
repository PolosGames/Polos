param(
    [Parameter(Position = 0)]
    [string]$Destination,

    [Parameter(Position = 1)]
    [ValidateSet('debug-sdk', 'release-sdk', 'win-debug', 'win-release', 'win-relwithdebinfo')]
    [string]$Preset = 'release-sdk'
)

. "$PSScriptRoot/common.ps1"

# Preserve the previous invocation with a single positional preset.
if ($Destination -in @('debug-sdk', 'release-sdk', 'win-debug', 'win-release', 'win-relwithdebinfo') -and
    -not $PSBoundParameters.ContainsKey('Preset')) {
    $Preset = $Destination
    $Destination = ''
}

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

    if ($Destination) {
        $Destination = [System.IO.Path]::GetFullPath($Destination)
        New-Item -ItemType Directory -Path $Destination -Force | Out-Null
        $StageDir = Join-Path $Destination ('.polos-sdk.' + [guid]::NewGuid().ToString())
        $SdkDir = Join-Path $Destination 'sdk'
        $PreviousSdk = Join-Path $StageDir 'previous'
        New-Item -ItemType Directory -Path $StageDir | Out-Null
        try {
            Invoke-CMake -E copy_directory $PackageDir (Join-Path $StageDir 'sdk')
            if (Test-Path -LiteralPath $SdkDir) {
                Move-Item -LiteralPath $SdkDir -Destination $PreviousSdk
            }
            Move-Item -LiteralPath (Join-Path $StageDir 'sdk') -Destination $SdkDir
        } finally {
            if ((Test-Path -LiteralPath $PreviousSdk) -and -not (Test-Path -LiteralPath $SdkDir)) {
                Move-Item -LiteralPath $PreviousSdk -Destination $SdkDir
            }
            Remove-Item -LiteralPath $StageDir -Recurse -Force
        }
        Write-Output $SdkDir
    }
} finally {
    Remove-Item -LiteralPath $PackageDir -Recurse -Force
}
Write-Output $Archive
