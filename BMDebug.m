//
//  BMDebug.m
//
//  Created by Jeronimo Valli on 2/15/11.
//

#import "BMDebug.h"
#include <unistd.h>
#include <sys/sysctl.h>

void BMAssert(NSString* message, const char* filename, int lineNumber, const char* functionName)
{
	NSLog(@"***************************************************************************");
	NSLog(@"Assertion failed at %s: %@", functionName, message);
	NSLog(@"%s:%d", filename, lineNumber);
	NSLog(@"***************************************************************************");
	[BMDebug printCallStack];
	NSLog(@"***************************************************************************");
	BMDebugBreak();
}

// Returns true if the current process is being debugged (either
// running under the debugger or has a debugger attached post facto).
// From http://developer.apple.com/mac/library/qa/qa2004/qa1361.html, midly edited for style.
bool BMAmIBeingDebugged(void)
{
    int                 mib[4];
    struct kinfo_proc   info;
	
    // Initialize the flags so that, if sysctl fails for some bizarre
    // reason, we get a predictable result.
	
    info.kp_proc.p_flag = 0;
	
    // Initialize mib, which tells sysctl the info we want, in this case
    // we're looking for information about a specific process ID.
	
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();
	
    // Call sysctl.
	
    size_t size = sizeof(info);
    int junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
    ASSERT(junk == 0);
	UNUSED(junk);
	
    // We're being debugged if the P_TRACED flag is set.
	
    return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
}

void BMDebugBreak(void)
{
	//TODO: make this work on every compiler.
	if(BMAmIBeingDebugged())
	{
#if TARGET_IPHONE_SIMULATOR
		__asm__ ("int3");
#elif TARGET_OS_IPHONE
		__asm__ ("trap");
#elif TARGET_CPU_X86
		__asm__ ("int 3");
#endif
	}
	else
	{
		abort();
	}
}

void BMTrace(NSString* format, ...) 
{
    va_list ap;
	
    va_start (ap, format);
	format = [format stringByAppendingString: @"\n"];
    NSString* body =  [[NSString alloc] initWithFormat:format arguments:ap];
    va_end (ap);
    
	fprintf(stderr, "%s", [body UTF8String]);
	
    BMRelease(body);
}

void BMDie(NSString* reason, const char* filename, int line, const char* function)
{
	NSLog(@"Impossible situation: %@", reason);
	NSLog(@"%s:%d:1 [%s]", filename, line, function);
	[BMDebug printCallStack];
	BMDebugBreak();
}



@implementation BMDebug

+ (void)printCallStack
{
	// callStackSymbols is available on iOS4+ only
	if([[NSThread class] respondsToSelector:@selector(callStackSymbols)])
	{
		NSArray* stack = [[NSThread class] performSelector:@selector(callStackSymbols)];
		
		for (NSString* line in stack)	
		{
			NSLog(@"%@", line);
		}
	}
	else
	{
		NSLog(@"Stack trace unavailable");
	}
}

@end
