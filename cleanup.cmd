@echo off

pushd "projects/vc14"
rmdir /S /Q ".vs"
rmdir /S /Q "ipch"
del /S "*.db"
del /S "*.opendb"
popd

pushd "bin"
del /S "*.exe"
del /S "*.pdb"
popd


rmdir /S /Q "obj"
rmdir /S /Q "bin/write_directory"

pause