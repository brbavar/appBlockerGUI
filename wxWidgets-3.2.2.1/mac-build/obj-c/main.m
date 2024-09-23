#import "BNRLogger.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        BNRLogger *logger = [[BNRLogger alloc] init];
        
        // NSTask *makeReadable = [[NSTask alloc] init];
        // [makeReadable setLaunchPath:@"/bin/chmod"];
        // [makeReadable setArguments:@[@"+r", @"../.blocklist.txt"]];
        // [makeReadable launch];
        // [makeReadable waitUntilExit];

        // NSString *string = [NSString stringWithContentsOfFile:@"../.blocklist.txt"
        //                                              encoding:NSASCIIStringEncoding
        //                                                 error:NULL];
        
        // NSTask *makeUnreadable = [[NSTask alloc] init];
        // [makeUnreadable setLaunchPath:@"/bin/chmod"];
        // [makeUnreadable setArguments:@[@"-r", @"../.blocklist.txt"]];
        // [makeUnreadable launch];

        NSString *string = [NSString stringWithUTF8String:argv[1]];
        
        NSLog(@"string = %@", string);
        [logger setBlocklist:string];  // Expects array of strings as arg, not just a string

        [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:logger
                                                 selector:@selector(appLaunch:)
                                                     name:NSWorkspaceDidLaunchApplicationNotification
                                                   object:nil];
        [[NSRunLoop currentRunLoop] run];
    }
    return 0;
}
