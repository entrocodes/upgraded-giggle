param(
    [Parameter(Mandatory = $false)]
    [string]$Name,

    [string]$OutputDir
)
if (-not $Name) {
    $Name = Read-Host "Enter system name"
}
if (-not $OutputDir) {
    $OutputDir = Read-Host "Enter output dir"
}

$OutputDir = Resolve-Path $OutputDir
$SystemName = "${Name}System"
$HppPath = Join-Path $OutputDir "$SystemName.hpp"
$CppPath = Join-Path $OutputDir "$SystemName.cpp"

if ((Test-Path $HppPath) -or (Test-Path $CppPath)) {
    Write-Error "System '$SystemName' already exists."
    exit 1
}

$hpp = @"
#pragma once
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"

class $SystemName : public ISystem {
public:
    SystemExec update(GameContext* context);
};
"@

$cpp = @"
#include "$SystemName.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec $SystemName::update(GameContext* context) {
    return { SystemExecResult::Ran };
}
"@

$hpp | Set-Content -Path $HppPath -Encoding utf8
$cpp | Set-Content -Path $CppPath -Encoding utf8

Write-Host "Created:"
Write-Host "  $HppPath"
Write-Host "  $CppPath"
    