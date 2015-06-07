# Buddhabrot

Single core, multi core, CUDA, and OpenCL versions of [Buddhabrot](en.wikipedia.org/wiki/Buddhabrot)

# Status

* Single core
* Multi core (OpenMP)
* Multi core (OpenCL) - not started
* Multi core (CUDA) - not started

                     +-------+------+---------+----------------+
                     | Linux | OSX  | Windows | Filename       |
    +----------------+-------+------+---------+----------------+
    | Single core    | Done  | Done | TODO    | bin/buddhabrot |
    | OpenMP         | Done  | Done | TODO    | bin/omp2       |
    | OpenCL         | TODO  | TODO | TODO    | n/a            |
    | CUDA           | TODO  | TODO | TODO    | n/a            |
    +----------------+-------+------+---------+----------------+

Legend:

    cpu1   Single core master reference
    omp1   Multi core (OpenMP) initial version 1
    omp2   Multi core (OpenMP) faster  version 2
    cuda   Multi core (CUDA  )
    ocl    Multi core (OpenCL)
