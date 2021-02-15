struct w32_timer_t
{
    LARGE_INTEGER perfFrequency;
    LARGE_INTEGER lastCounter;
    u64 lastCycle;
};

internal void W32_InitTimer(w32_timer_t *timer)
{
    QueryPerformanceFrequency(&timer->perfFrequency);
    QueryPerformanceCounter(&timer->lastCounter);
    timer->lastCycle = __rdtsc();
}

internal void W32_TimerEndFrame(w32_timer_t *timer)
{
    LARGE_INTEGER endCounter;
    QueryPerformanceCounter(&endCounter);

    i64 timeElapsed = endCounter.QuadPart - timer->lastCounter.QuadPart;
    f32 deltaTime = timeElapsed / (f32)timer->perfFrequency.QuadPart;

    os->deltaTime = deltaTime;

    // u64 endCycle = __rdtsc();
    // u64 cycles = endCycle - timer->lastCycle;

    // f64 msPerFrame = 1000.0f * timeElapsed / timer->perfFrequency.QuadPart;
    // f64 framesPerSecond = (f64)timer->perfFrequency.QuadPart / (f64)timeElapsed;
    // f64 mcPerFrame = cycles / (1000.0 * 1000.0);

    // OS_DebugPrint("%.02f ms/f,  %.02f fps,  %.02f mc/f\n", deltaTime * 1000.0f, fps, mcPerFrame);

    timer->lastCounter = endCounter;
}
