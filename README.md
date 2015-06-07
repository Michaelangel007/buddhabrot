# Buddhabrot

Single core, multi core, CUDA, and OpenCL versions of [Buddhabrot](en.wikipedia.org/wiki/Buddhabrot)

# Status

* Single core - Done!
* Multi core (OpenMP) - Done!
* Multi core (CUDA) - not yet started
* Multi core (OpenCL) - not yet started

                         +-------+------+---------+----------------+
                         | Linux | OSX  | Windows | Filename       |
        +----------------+-------+------+---------+----------------+
        | Single core    | Done  | Done | TODO    | bin/buddhabrot |
        | OpenMP         | Done  | Done | TODO    | bin/omp2       |
        | CUDA           | TODO  | TODO | TODO    | n/a            |
        | OpenCL         | TODO  | TODO | TODO    | n/a            |
        +----------------+-------+------+---------+----------------+


# Benchmarks (min:sec)

        +--------------+------+------+------+------+------+-----+
        | Hardware     | org  | cpu1 | omp1 | omp2 | cuda | ocl |
        +--------------+------+------+------+------+------+-----+
        | i7 @ 2.6 GHz | 0:57 | 0:55 | 0:21 | 0:17 | n/a  | n/a |
        | 955@ 3.5 GHz | 1:37 | 1:29 | 1:00 | 0:42 | n/a  | n/a |
        +--------------+------+------+------+------+------+-----+

= Hardware =

* Intel i7 @ 2.6 GHz, OSX
* AMD Phenom II 955BE @ 3.5 Ghz, Linux

= Legend: =

    cpu1   Single core master reference
    omp1   Multi core (OpenMP) initial version 1
    omp2   Multi core (OpenMP) faster  version 2
    cuda   Multi core (CUDA  )
    ocl    Multi core (OpenCL)

# References

* PDF: [A "Hands-on" Introduction to OpenMP](http://openmp.org/mp-documents/omp-hands-on-SC08.pdf)
* PDF: [OpenMP by Example](http://people.math.umass.edu/~johnston/PHI_WG_2014/OpenMPSlides_tamu_sc.pdf)

