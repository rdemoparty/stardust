cd .\cmake
call .\cmake_vs2013_x86.cmd
cd ..
msbuild build_vs2013_x86\stardust.sln /p:Configuration=Debug