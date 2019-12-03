    // The single 16-bit brightness buffer is a shared resource
    // We would incur a major performance penalty via atomic access
    // to keep it in sync amongst the various threads
    // Instead, we give each thread its own independent copy
    // Afterwards, we will merge (add) all copies back into a single brightness buffer
    int       gnThreadsMaximum = 0 ;
    int       gnThreadsActive  = 0 ; // 0 = auto detect; > 0 manual # of threads

    const int MAX_THREADS      = 256; // Threadripper 3990X;
    uint16_t *gaThreadsTexels[ MAX_THREADS ]; // Not max cores but max threads

