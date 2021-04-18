#include "gl.h"
#include "input.h"
#include "window.h"
#import <Cocoa/Cocoa.h>
#include <dlfcn.h>

static struct
{
    NSOpenGLContext *glContext;
    b32 quit;
} g_Window;

@interface CustomWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation CustomWindowDelegate

- (void)windowWillClose:(id)sender
{
    (void)sender;
    g_Window.quit = true;
}

@end

@interface CustomWindow : NSWindow
@end

@implementation CustomWindow

- (void)keyDown:(NSEvent *)event
{
    (void)event;
}

@end

static const void *OSX_GetProcAddress(const char *name)
{
    return dlsym(RTLD_DEFAULT, name);
}

b32 WindowCreate(i32 width, i32 height, const char *title)
{
    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect initialFrame = NSMakeRect((screenRect.size.width - width) * 0.5,
                                     (screenRect.size.height - height) * 0.5, width, height);

    NSWindow *window = [[CustomWindow alloc]
        initWithContentRect:initialFrame
                  styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                            NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                    backing:NSBackingStoreBuffered
                      defer:NO];

    [window setBackgroundColor:NSColor.blackColor];
    [window setTitle:@(title)];
    [window makeKeyAndOrderFront:nil];

    CustomWindowDelegate *windowDelegate = [[CustomWindowDelegate alloc] init];
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
    [openGLContext makeCurrentContext];
    LoadOpenGLProcs(OSX_GetProcAddress);

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
    NSEvent *event;

    do
    {
        event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                   untilDate:nil
                                      inMode:NSDefaultRunLoopMode
                                     dequeue:YES];

        if (event == nil)
        {
            break;
        }

        switch ([event type])
        {
        case NSEventTypeKeyDown:
            OnKeyPress(Key_Unknown);
            break;
        case NSEventTypeKeyUp:
            OnKeyRelease(Key_Unknown);
            break;
        default:
            [NSApp sendEvent:event];
        }
    } while (event != nil);
}

void WindowSwapInterval(i32 interval)
{
    [g_Window.glContext setValues:&interval forParameter:NSOpenGLCPSwapInterval];
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
