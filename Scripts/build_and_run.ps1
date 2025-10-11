# Set strict mode to catch common errors and set the error action preference.
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# --- Configuration ---
param (
    [string]$BuildType = "Debug"
)

if (-not $BuildType) {
    Write-Host "Usage: .\build_and_run.ps1 -BuildType [Debug,Release]"
    exit 1
}

$Preset = switch ($BuildType)
{
    Debug { "win-debug" }
    Release { "win-release" }
}

$ProjectRoot = Resolve-Path -Path (Join-Path $PSScriptRoot "..")
$InstallDir = Join-Path -Path $ProjectRoot -ChildPath "out/build/$Preset"

Write-Host "Project Root: $ProjectRoot"
Write-Host "Install Directory: $InstallDir" -ForegroundColor Cyan
Write-Host ""

# --- Format the code ---
Write-Host "Running clang-format..." -ForegroundColor Green
& (Join-Path $PSScriptRoot "format_code.ps1")
Write-Host ""


# --- Configure ---
Write-Host "Configuring CMake with preset '$Preset'..." -ForegroundColor Green
cmake --preset $Preset -DPOLOS_INSTALL_DIR=$InstallDir

if ($LASTEXITCODE -ne 0) {
  Write-Host "CMake configuration failed!" -ForegroundColor Red
  exit $LASTEXITCODE
}
Write-Host ""

# --- Build ---
Write-Host "Building project with preset '$Preset'..." -ForegroundColor Green
cmake --build --preset $Preset

if ($LASTEXITCODE -ne 0) {
  Write-Host "Build failed!" -ForegroundColor Red
  exit $LASTEXITCODE
}
Write-Host "Build successful."
Write-Host ""


# --- Run the Application ---
Write-Host "Starting application..." -ForegroundColor Green
$ExePath = Join-Path -Path $InstallDir -ChildPath "dummy.exe"

# Check if the executable exists before trying to run it.
if (-not (Test-Path $ExePath)) {
    Write-Host "Error: Executable not found at '$ExePath'" -ForegroundColor Red
    exit 1
}

Start-Process -FilePath $ExePath -WorkingDirectory $I
