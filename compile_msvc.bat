cl /O2                /Fo:bin/buddha.exe       /Fe:bin/buddha.exe      buddhabrot.cpp
cl /O2                /Fo:bin/buddha_omp1.obj  /Fe:bin/buddha_omp1.exe buddhabrot_omp1.cpp
cl /O2                /Fo:bin/buddha_omp2.obj  /Fe:bin/buddha_omp2.exe buddhabrot_omp2.cpp
cl /O2                /Fo:bin/buddha_omp3.obj  /Fe:bin/buddha_omp3.exe buddhabrot_omp3.cpp
cl /O2                /Fo:bin/buddha_x64.obj   /Fe:bin/buddhabrot.exe  buddhabrot_omp4.cpp
cl /O2 /favor:INTEL64 /Fo:bin/buddha_int.obj   /Fe:bin/buddha_int.exe  buddhabrot_omp4.cpp
cl /O2 /favor:AMD64   /Fo:bin/buddha_amd.obj   /Fe:bin/buddha_amd.exe  buddhabrot_omp4.cpp

