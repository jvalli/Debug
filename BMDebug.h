//
//  BMDebug.h
//
//  Created by Jeronimo Valli on 2/15/11.
//

#import <Foundation/Foundation.h>


#ifdef __clang__
#define NOTNULL(...) __attribute__((nonnull (__VA_ARGS__)))
#define NORETURN __attribute__((analyzer_noreturn))
#else
#define NOTNULL(...) 
#define NORETURN 
#endif


// INTERNAL. Do not use directly.
bool BMAmIBeingDebugged(void);

// If the app is running attached to a debugger, it breaks. If not, it crashes.
// INTERNAL. Do not use directly.
void BMDebugBreak(void) NORETURN; 

// Prints a message and the call stack to the console, and calls BMDebugBreak
// INTERNAL. Do not use directly.
void BMAssert(NSString* message, const char* filename, int lineNumber, const char* functionName) NORETURN;

// Prints a formatted string to the console.
// INTERNAL. Do not use directly.
//TODO: research __attribute__ ((format (printf, 1, 2)))
void BMTrace(NSString* format, ...);

// INTERNAL. Do not use directly.
void BMDie(NSString* reason, const char* filename, int line, const char* function) NORETURN;


/*
 Use this when you have a variable that is only used on certain configurations (tipically DEBUG).
 It has zero runtime cost.
 
 Example:
 
 ----------------------------
 int length = [string length];
 ASSERT(length > 0);
 // If we never use length again, the compiler will complain in release builds.
 UNUSED(variable);
 ----------------------------

 Another use is to shut up the compiler when we don't use a parameter.
 
 */
#define UNUSED(variable) ((void)variable)

/*
 TRACE is a nicer version of NSLog, with the added benefit that it does nothing on release builds.
 
 Usage: 
 
 TRACE();
 TRACE(@"Hello world");
 TRACE(@"The value is: %d", value);
 TRACE("The value is: %d", value);
 
 */
#ifndef DEBUG
	#define TRACE(...) do { } while(0)
#else
	#define TRACE(format, ...) do {BMTrace((@"%s:%d:1 [%s] " format), __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);} while (0);
#endif

/*
 ASSERT is a nicer version of NSAssertX, with the added benefit that it does nothing on release builds.
 Oh, one more thing. If you're running on the debugger, you can continue. Yep, it's like a good old conditional breakpoint.
 */
#ifndef DEBUG
	#define ASSERT(condition) do { } while(0);
#else
	#define ASSERT(condition) do {if(!(condition)) BMAssert([NSString stringWithCString:#condition encoding:NSUTF8StringEncoding], __FILE__, __LINE__, __PRETTY_FUNCTION__); } while (0);
#endif

// Use this when you want to signal an impossible or unexpected situation.
// It's similar to ASSERT, but it remains on release builds.
// Prints the reason, the location of the failure, and the call stack to the console.
// If its running under a debugger, it breaks. Else, it exits.
// NOTE: it only prints the call stack on iOS4+
#define DIE(reason) do {BMDie(reason, __FILE__, __LINE__, __PRETTY_FUNCTION__);} while(0);

// Short and clear way to make sure a variable is of the expected type.
#define CHECK_CLASS(x, y) ASSERT([x isKindOfClass:[y class]])
#define CHECK_CLASS_OR_NULL(x, y) ASSERT(!x | [x isKindOfClass:[y class]])
#define CHECK_PROTOCOL(x, y) ASSERT([y conformsToProtocol:@protocol(y)]);

// Release and nil in a single line. 
/*
 IDEA: What if instead of setting to nil we set to a static instance of a class that dumps on every call?
 It might be a nice way to catch zombies. And logic errors.
 */
#define	BMRelease(x) {[x release]; x = nil;}

#define STRING_OR_EMPTY(str) ([str isKindOfClass:[NSString class]] ? str : @"")

@interface BMDebug : NSObject 

+ (void)printCallStack;

@end
