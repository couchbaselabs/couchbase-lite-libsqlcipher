function Invoke-CmdScript {
    $cmdLine = """C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"" amd64 & set"
    & $Env:SystemRoot\system32\cmd.exe /c $cmdLine |
    Select-String '^([^=]*)=(.*)$' | ForEach-Object {
        $varName = $_.Matches[0].Groups[1].Value
        $varValue = $_.Matches[0].Groups[2].Value
        Set-Item Env:$varName $varValue
    }
}

# Output directory for public headers
$OUTPUT_DIR="$pwd\libs\include"
if(Test-Path $OUTPUT_DIR -PathType Container) {
    Remove-Item -Recurse -Force $OUTPUT_DIR
}

New-Item -Type Directory $OUTPUT_DIR

cd vendor\sqlcipher

# Clean:
& 'C:\Program Files\Git\bin\git.exe' clean -dfx
& 'C:\Program Files\Git\bin\git.exe' checkout -f

# Configure and Make:
Invoke-CmdScript
& nmake /f Makefile.msc sqlite3.c  

# Output:
Remove-Item -Force ../../src/c/sqlite3.c
Remove-Item -Force ../../src/headers/sqlite3.h
cp sqlite3.c ../../src/c/
cp sqlite3.h ../../src/headers/

# Public headers:
cp sqlite3.h $OUTPUT_DIR

# Exit:
cd ../../
