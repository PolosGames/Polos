. "$PSScriptRoot/common.ps1"

Get-ChildItem -LiteralPath $PolosRoot -Directory |
    Where-Object { $_.Name -in @('Source', 'Examples', 'Tools') } |
    ForEach-Object { Get-ChildItem -LiteralPath $_.FullName -Recurse -File } |
    Where-Object { $_.Extension -in @('.h', '.hpp', '.cpp') } |
    ForEach-Object {
        & clang-format --style=file -i $_.FullName
        if ($LASTEXITCODE -ne 0) {
            throw "clang-format failed for $($_.FullName)."
        }
    }

Write-Host "Formatting complete."
