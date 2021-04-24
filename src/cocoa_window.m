#include "cocoa_input.h"
#include "gl.h"
#include "window.h"
#import <Cocoa/Cocoa.h>
#include <dlfcn.h>

static struct
{
    NSWindow *handle;
    NSOpenGLContext *glContext;
    b32 quit;
    i32 width;
    i32 height;
} g_Window;

@interface CustomWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation CustomWindowDelegate

- (void)windowWillClose:(id)sender
{
    (void)sender;
    g_Window.quit = true;
}

- (void)windowDidResize:(NSNotification *)notification
{
    (void)notification;

    NSRect content = [g_Window.handle contentRectForFrameRect:[g_Window.handle frame]];
    g_Window.width = content.size.width;
    g_Window.height = content.size.height;
    glViewport(0, 0, g_Window.width, g_Window.height);
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

    g_Window.handle = window;
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
            (event.isARepeat ? OnKeyRepeat : OnKeyPress)(OSX_KeyCodeToInputKey(event.keyCode));
            break;
        case NSEventTypeKeyUp:
            OnKeyRelease(OSX_KeyCodeToInputKey(event.keyCode));
            break;
        case NSEventTypeLeftMouseDown:
            OnMouseClick(Mouse_Left);
            break;
        case NSEventTypeLeftMouseUp:
            OnMouseRelease(Mouse_Left);
            break;
        case NSEventTypeRightMouseDown:
            OnMouseClick(Mouse_Right);
            break;
        case NSEventTypeRightMouseUp:
            OnMouseRelease(Mouse_Right);
            break;
        case NSEventTypeMouseMoved: {
            NSPoint mouse = [event locationInWindow];
            NSRect content = [g_Window.handle contentRectForFrameRect:[g_Window.handle frame]];
            OnMouseMove(mouse.x, content.size.height - mouse.y);
            break;
        }
        default:
            break;
        }

        [NSApp sendEvent:event];
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
    return g_Window.width;
}

i32 Window_Height(void)
{
    return g_Window.height;
}
