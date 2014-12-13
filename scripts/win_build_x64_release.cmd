cd .\cmake
call .\cmake_vs2013_x64.cmd
cd ..
msbuild build_vs2013_x64\stardust.sln /p:Configuration=Release