Get-ChildItem -Recurse -Include *.h, *.hpp, *.cpp -File |
    Where-Object { $_.FullName -notmatch '\\(out|ThirdParty)\\' } |
    ForEach-Object { clang-format --style=file -i $_.FullName }

Write-Host "Formatting complete."
