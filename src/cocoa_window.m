#include "gl.h"
#include "window.h"
#import <Cocoa/Cocoa.h>
#include <dlfcn.h>

static struct
{
    NSOpenGLContext *glContext;
    b32 quit;
} g_Window;

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate

- (void)windowWillClose:(id)sender
{
    (void)sender;
    g_Window.quit = true;
}

@end

static void *OSX_GetProcAddress(const char *name)
{
    return dlsym(RTLD_DEFAULT, name);
}

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

    WindowDelegate *windowDelegate = [[WindowDelegate alloc] init];
    [window setDelegate:windowDelegate];

    NSOpenGLPixelFormatAttribute pixelFormatAttribs[] = {NSOpenGLPFAOpenGLProfile,
                                                         NSOpenGLProfileVersion3_2Core,
                                                         NSOpenGLPFAColorSize,
                                                         24,
                                                         NSOpenGLPFAAlphaSize,
                                                         8,
                                                         NSOpenGLPFADoubleBuffer,
                                                         NSOpenGLPFAAccelerated,
                                                         0};

    NSOpenGLPixelFormat *pixelFormat =
        [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttribs];
    NSOpenGLContext *openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat
                                                                shareContext:nil];
    [openGLContext setView:[window contentView]];
    LoadOpenGLProcs((GetOpenGLProc)OSX_GetProcAddress);

    g_Window.glContext = openGLContext;
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

void WindowBeginDraw(void)
{
    [g_Window.glContext makeCurrentContext];
}

void WindowSwapBuffers(void)
{
    [g_Window.glContext flushBuffer];
}

i32 Window_Width(void)
{
    return 0;
}

i32 Window_Height(void)
{
    return 0;
}
