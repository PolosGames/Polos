$Example = ''
[ValidateSet('debug-runtime', 'release-runtime', 'win-debug', 'win-release', 'win-relwithdebinfo')]
[string]$Preset = 'debug-runtime'
$Preprocess = $false

for ($Index = 0; $Index -lt $args.Count; $Index++) {
    $Option = $args[$Index]
    switch ($Option) {
        { $_ -in '--example', '--preset' } {
            if (($Index + 1 -ge $args.Count) -or
                [string]::IsNullOrEmpty($args[$Index + 1]) -or
                ($args[$Index + 1] -like '-*')) {
                throw "$Option requires a name."
            }

            $Index++
            if ($Option -eq '--example') {
                $Example = $args[$Index]
            } else {
                $Preset = $args[$Index]
            }
        }
        '--preprocess' {
            $Preprocess = $true
        }
        { $_ -in '--help', '-h' } {
            Write-Output 'Usage: ./run_example.ps1 --example <name> [--preset <name>] [--preprocess]'
            Write-Output 'The preset defaults to debug-runtime. Use --preprocess to regenerate the resource catalog.'
            exit 0
        }
        default {
            throw "Unknown option: $Option"
        }
    }
}

if (-not $Example) {
    throw 'Missing required --example <name>.'
}
. "$PSScriptRoot/common.ps1"

$ExampleDir = Join-Path $PolosRoot "Examples/$Example"
if (($Example -notmatch '^[A-Za-z0-9_][A-Za-z0-9_.+-]*$') -or
    (-not (Test-Path -LiteralPath (Join-Path $ExampleDir 'CMakeLists.txt') -PathType Leaf))) {
    throw "Unknown Polos example: $Example"
}

$BuildDir = Join-Path $PolosRoot "out/build/projects/$Preset/$Example"
$BinDir = Join-Path $PolosRoot "out/bin/$Preset/$Example"
Initialize-PolosBuild $Preset $BuildDir $BinDir 'ON' $Example
if ($Preprocess) {
    Invoke-PolosResourcePreprocessor $BuildDir $ExampleDir
}
Invoke-CMake --build $BuildDir --target install-runtime

Push-Location $BinDir
try {
    & "./$Example.exe"
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
