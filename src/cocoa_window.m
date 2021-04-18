#include "window.h"
#import <Cocoa/Cocoa.h>

static struct
{
    b32 quit;
} g_Window;

@interface MainWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation MainWindowDelegate

- (void)windowWillClose:(id)sender
{
    (void)sender;
    g_Window.quit = true;
}

@end

b32 WindowCreate(i32 width, i32 height, const char *title)
{
    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect initialFrame = NSMakeRect((screenRect.size.width - width) * 0.5,
                                     (screenRect.size.height - height) * 0.5, width, height);

    NSWindow *window = [[NSWindow alloc]
        initWithContentRect:initialFrame
                  styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                            NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                    backing:NSBackingStoreBuffered
                      defer:NO];

    [window setTitle:@(title)];
    [window makeKeyAndOrderFront:nil];

    MainWindowDelegate *mainWindowDelegate = [[MainWindowDelegate alloc] init];
    [window setDelegate:mainWindowDelegate];

    g_Window.quit = false;

    return true;
}

b32 WindowShouldClose(void)
{
    return g_Window.quit;
}

void WindowPollEvents(void)
{
    NSEvent *Event;

    do
    {
        Event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                   untilDate:nil
                                      inMode:NSDefaultRunLoopMode
                                     dequeue:YES];

        switch ([Event type])
        {
        default:
            [NSApp sendEvent:Event];
        }
    } while (Event != nil);
}

void WindowSwapInterval(i32 interval)
{
    (void)interval;
}

void WindowSwapBuffers(void)
{
}

i32 Window_Width(void)
{
    return 0;
}

i32 Window_Height(void)
{
    return 0;
}
