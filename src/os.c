internal void OS_PushEvent(os_event event)
{
    if (os->eventCount < ArrayCount(os->events))
    {
        os->events[os->eventCount++] = event;
    }
}

internal b32 OS_GetNextEvent(os_event *event)
{
    if (os->eventCount == 0)
    {
        event = NULL;
        return 0;
    }

    *event = os->events[--os->eventCount];
    return 1;
}
