# --- Benchmark Script for Searchtool vs Grep ---

$targetFile = "10mb-examplefile-com.txt"
$pattern = "Wikipedia"

# 1. Find Searchtool
if (Test-Path ".\build\searchtool.exe") {
    $searchtool = ".\build\searchtool.exe"
} elseif (Test-Path ".\searchtool.exe") {
    $searchtool = ".\searchtool.exe"
} else {
    Write-Error "Could not find searchtool.exe. Please build the project first."
    exit
}

# 2. Find Grep
$grepPath = "C:\Program Files\Git\usr\bin\grep.exe"
if (-not (Test-Path $grepPath)) {
    $grepPath = (Get-Command grep -ErrorAction SilentlyContinue).Source
}

if (-not $grepPath) {
    Write-Error "Could not find grep.exe."
    exit
}

Write-Host "`n==========================================" -ForegroundColor Cyan
Write-Host "   BENCHMARK: SEARCHTOOL vs GREP" -ForegroundColor White
Write-Host "==========================================`n" -ForegroundColor Cyan

Write-Host "File: $targetFile"
Write-Host "Pattern: $pattern`n"

# Run Searchtool
Write-Host "[1/2] Running Searchtool..." -ForegroundColor Yellow
$timeSearchtool = Measure-Command { & $searchtool $targetFile $pattern | Out-Null }
Write-Host "Done: $($timeSearchtool.TotalMilliseconds) ms" -ForegroundColor Green

# Run Grep
Write-Host "[2/2] Running Grep..." -ForegroundColor Yellow
$timeGrep = Measure-Command { & $grepPath $pattern $targetFile | Out-Null }
Write-Host "Done: $($timeGrep.TotalMilliseconds) ms" -ForegroundColor Green

Write-Host "`n------------------------------------------"
$diff = $timeGrep.TotalMilliseconds - $timeSearchtool.TotalMilliseconds
if ($diff -gt 0) {
    Write-Host "RESULT: Searchtool is faster by $($diff) ms!" -ForegroundColor Cyan
} else {
    Write-Host "RESULT: Grep is faster by $($[Math]::Abs($diff)) ms!" -ForegroundColor Cyan
}
Write-Host "------------------------------------------`n"
