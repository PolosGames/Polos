Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$PolosRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path

function Invoke-CMake {
    & cmake @args
    if ($LASTEXITCODE -ne 0) {
        throw "CMake exited with code $LASTEXITCODE."
    }
}

function Initialize-PolosBuild {
    param([string]$Preset, [string]$BuildDir, [string]$InstallDir, [string]$BuildExamples, [string]$Example = '')
    $Options = @(
        "-DCMAKE_INSTALL_PREFIX=$InstallDir"
        '-DPOLOS_INSTALL_DIR:STRING=.'
        "-DBUILD_EXAMPLES=$BuildExamples"
        "-DPOLOS_EXAMPLE:STRING=$Example"
    )
    if ($BuildExamples -eq 'OFF') {
        $Options += '-DBUILD_TESTS=OFF'
    }
    Invoke-CMake -S $PolosRoot --preset $Preset -B $BuildDir @Options
}
