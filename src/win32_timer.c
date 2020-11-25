typedef struct w32_timer w32_timer;
struct w32_timer
{
    LARGE_INTEGER perfFrequency;
    LARGE_INTEGER lastCounter;
    u64 lastCycle;
};

internal void W32_InitTimer(w32_timer *timer)
{
    QueryPerformanceFrequency(&timer->perfFrequency);
    QueryPerformanceCounter(&timer->lastCounter);
    timer->lastCycle = __rdtsc();
}

internal void W32_TimerEndFrame(w32_timer *timer)
{
    LARGE_INTEGER endCounter;
    u64 endCycle = __rdtsc();
    QueryPerformanceCounter(&endCounter);

    u64 cycles = endCycle - timer->lastCycle;
    i64 timeElapsed = endCounter.QuadPart - timer->lastCounter.QuadPart;

    f64 mspf = 1000.0 * timeElapsed / timer->perfFrequency.QuadPart;
    f64 fps = (f64)timer->perfFrequency.QuadPart / (f64)timeElapsed;
    f64 mcpf = cycles / (1000.0 * 1000.0);

    TCHAR timingBuff[256];

    StringCbPrintfA(timingBuff, 256 * sizeof(TCHAR), TEXT("%.02f ms/f,  %.02f fps,  %.02f mc/f\n"),
                    mspf, fps, mcpf);
    // OutputDebugStringA(timingBuff);

    timer->lastCounter = endCounter;
}
