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
    $Cache = Join-Path $BuildDir 'CMakeCache.txt'
    $RetiredToolchain = Join-Path $PolosRoot 'CMake/Toolchain.cmake'
    if ((Test-Path -LiteralPath $Cache) -and -not (Test-Path -LiteralPath $RetiredToolchain)) {
        $Entry = Select-String -LiteralPath $Cache -Pattern '^CMAKE_TOOLCHAIN_FILE:[^=]*=(.*)$' |
            Select-Object -First 1
        if ($Entry -and ($Entry.Matches[0].Groups[1].Value.Replace('\', '/') -eq $RetiredToolchain.Replace('\', '/'))) {
            $Options += '--fresh'
        }
    }
    if (-not $env:CPM_SOURCE_CACHE) {
        $env:CPM_SOURCE_CACHE = Join-Path $PolosRoot 'out/cache/cpm'
    }
    Invoke-CMake -S $PolosRoot --preset $Preset -B $BuildDir @Options
}

function Invoke-PolosResourcePreprocessor {
    param([string]$BuildDir, [string]$ProjectDir)

    Invoke-CMake --build $BuildDir --target ResourcePreprocessor
    $Preprocessor = Join-Path $BuildDir 'Tools/ResourcePreprocessor/ResourcePreprocessor.exe'
    & $Preprocessor $ProjectDir
    if ($LASTEXITCODE -ne 0) {
        throw "ResourcePreprocessor exited with code $LASTEXITCODE."
    }
}
