#include "input.h"
#include "gl.h"
#include "window.h"
#include <dlfcn.h>

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>

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

static i32 OSX_KeyCodeToInputKey(u16 keyCode)
{
    switch (keyCode)
    {
    case kVK_ANSI_0:
        return Key_0;
    case kVK_ANSI_1:
        return Key_1;
    case kVK_ANSI_2:
        return Key_2;
    case kVK_ANSI_3:
        return Key_3;
    case kVK_ANSI_4:
        return Key_4;
    case kVK_ANSI_5:
        return Key_5;
    case kVK_ANSI_6:
        return Key_6;
    case kVK_ANSI_7:
        return Key_7;
    case kVK_ANSI_8:
        return Key_8;
    case kVK_ANSI_9:
        return Key_9;
    case kVK_ANSI_A:
        return Key_A;
    case kVK_ANSI_B:
        return Key_B;
    case kVK_ANSI_C:
        return Key_C;
    case kVK_ANSI_D:
        return Key_D;
    case kVK_ANSI_E:
        return Key_E;
    case kVK_ANSI_F:
        return Key_F;
    case kVK_ANSI_G:
        return Key_G;
    case kVK_ANSI_H:
        return Key_H;
    case kVK_ANSI_I:
        return Key_I;
    case kVK_ANSI_J:
        return Key_J;
    case kVK_ANSI_K:
        return Key_K;
    case kVK_ANSI_L:
        return Key_L;
    case kVK_ANSI_M:
        return Key_M;
    case kVK_ANSI_N:
        return Key_N;
    case kVK_ANSI_O:
        return Key_O;
    case kVK_ANSI_P:
        return Key_P;
    case kVK_ANSI_Q:
        return Key_Q;
    case kVK_ANSI_R:
        return Key_R;
    case kVK_ANSI_S:
        return Key_S;
    case kVK_ANSI_T:
        return Key_T;
    case kVK_ANSI_U:
        return Key_U;
    case kVK_ANSI_V:
        return Key_V;
    case kVK_ANSI_W:
        return Key_W;
    case kVK_ANSI_X:
        return Key_X;
    case kVK_ANSI_Y:
        return Key_Y;
    case kVK_ANSI_Z:
        return Key_Z;
    case kVK_ANSI_Comma:
        return Key_Comma;
    case kVK_ANSI_Equal:
        return Key_Equal;
    case kVK_ANSI_Grave:
        return Key_Grave;
    case kVK_ANSI_LeftBracket:
        return Key_LeftBracket;
    case kVK_ANSI_Minus:
        return Key_Minus;
    case kVK_ANSI_Period:
        return Key_Period;
    case kVK_ANSI_Quote:
        return Key_Quote;
    case kVK_ANSI_RightBracket:
        return Key_RightBracket;
    case kVK_ANSI_Slash:
        return Key_ForwardSlash;
    case kVK_ANSI_Backslash:
        return Key_BackSlash;
    case kVK_ANSI_KeypadDecimal:
        return Keypad_Decimal;
    case kVK_ANSI_KeypadMultiply:
        return Keypad_Multiply;
    case kVK_ANSI_KeypadPlus:
        return Keypad_Plus;
    case kVK_ANSI_KeypadClear:
        return Keypad_Clear;
    case kVK_ANSI_KeypadDivide:
        return Keypad_Divide;
    case kVK_ANSI_KeypadEnter:
        return Keypad_Enter;
    case kVK_ANSI_KeypadMinus:
        return Keypad_Minus;
    case kVK_ANSI_KeypadEquals:
        return Keypad_Equals;
    case kVK_ANSI_Keypad0:
        return Keypad_0;
    case kVK_ANSI_Keypad1:
        return Keypad_1;
    case kVK_ANSI_Keypad2:
        return Keypad_2;
    case kVK_ANSI_Keypad3:
        return Keypad_3;
    case kVK_ANSI_Keypad4:
        return Keypad_4;
    case kVK_ANSI_Keypad5:
        return Keypad_5;
    case kVK_ANSI_Keypad6:
        return Keypad_6;
    case kVK_ANSI_Keypad7:
        return Keypad_7;
    case kVK_ANSI_Keypad8:
        return Keypad_8;
    case kVK_ANSI_Keypad9:
        return Keypad_9;
    case kVK_ANSI_Semicolon:
        return Key_Semicolon;
    case kVK_Return:
        return Key_Enter;
    case kVK_Tab:
        return Key_Tab;
    case kVK_Space:
        return Key_Space;
    case kVK_Delete:
        return Key_Delete;
    case kVK_Escape:
        return Key_Esc;
    case kVK_Shift:
        return Key_LeftShift;
    case kVK_Option:
        return Key_LeftAlt;
    case kVK_Control:
        return Key_LeftCtrl;
    case kVK_RightShift:
        return Key_RightShift;
    case kVK_RightOption:
        return Key_RightAlt;
    case kVK_RightControl:
        return Key_RightCtrl;
    case kVK_F1:
        return Key_F1;
    case kVK_F2:
        return Key_F2;
    case kVK_F3:
        return Key_F3;
    case kVK_F4:
        return Key_F4;
    case kVK_F5:
        return Key_F5;
    case kVK_F6:
        return Key_F6;
    case kVK_F7:
        return Key_F7;
    case kVK_F8:
        return Key_F8;
    case kVK_F9:
        return Key_F9;
    case kVK_F10:
        return Key_F10;
    case kVK_F11:
        return Key_F11;
    case kVK_F12:
        return Key_F12;
    case kVK_F13:
        return Key_F13;
    case kVK_F14:
        return Key_F14;
    case kVK_F15:
        return Key_F15;
    case kVK_F16:
        return Key_F16;
    case kVK_F17:
        return Key_F17;
    case kVK_F18:
        return Key_F18;
    case kVK_F19:
        return Key_F19;
    case kVK_F20:
        return Key_F20;
    case kVK_Home:
        return Key_Home;
    case kVK_PageUp:
        return Key_PageUp;
    case kVK_End:
        return Key_End;
    case kVK_PageDown:
        return Key_PageDown;
    case kVK_LeftArrow:
        return Key_Left;
    case kVK_RightArrow:
        return Key_Right;
    case kVK_DownArrow:
        return Key_Down;
    case kVK_UpArrow:
        return Key_Up;

    case kVK_ForwardDelete:
    case kVK_Help:
    case kVK_VolumeUp:
    case kVK_VolumeDown:
    case kVK_Mute:
    case kVK_Function:
    case kVK_Command:
    case kVK_CapsLock:
    default:
        return Key_Unknown;
    }
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
