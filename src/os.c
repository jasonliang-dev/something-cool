internal void OS_PushEvent(os_event_t event)
{
    if (os->eventCount < ArrayCount(os->events))
    {
        os->events[os->eventCount++] = event;
    }
}

internal b32 OS_GetNextEvent(os_event_t *event)
{
    if (os->eventCount == 0)
    {
        return false;
    }

    *event = os->events[--os->eventCount];
    return true;
}

internal void OS_AssertionFailure(char *expresssion, char *file, u32 line)
{
    OS_DisplayError("ASSERT FAILED: (%s) %s:%d", expresssion, file, line);
    *(int *)0 = 0;
}
