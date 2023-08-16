#import "BNRLogger.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        BNRLogger *logger = [[BNRLogger alloc] init];
        
        NSTask *task = [[NSTask alloc] init];
//        [task setExecutableURL:[NSURL fileURLWithPath:@"/bin/chmod"]];
        [task setLaunchPath:@"/bin/chmod"];
        [task setArguments:@[@"+r", @"../.blocklist.txt"]];
        [task launch];

        NSString *string = [NSString stringWithContentsOfFile:@"../.blocklist.txt"
                                                     encoding:NSASCIIStringEncoding
                                                        error:NULL];
        NSLog(@"string = %@", string);
        [logger setBlocklist:string];

        [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:logger
                                                 selector:@selector(appLaunch:)
                                                     name:NSWorkspaceDidLaunchApplicationNotification
                                                   object:nil];
        [[NSRunLoop currentRunLoop] run];
    }
    return 0;
}
