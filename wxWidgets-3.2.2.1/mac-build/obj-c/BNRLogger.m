#import "BNRLogger.h"

//@interface BNRLogger ()
//@property (nonatomic) NSArray * blocklist;
//- (void)setBlocklist:(NSString *)string;
//- (void)appLaunch:(NSNotification *)note;
//@end

@implementation BNRLogger
- (void)setBlocklist:(NSString *)string {
    _blocklist = [[NSMutableArray alloc] init];
    
    [_blocklist addObjectsFromArray:[string componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]]];
    
    for (int i = 0; i < [_blocklist count]; i++)
    {
        NSNumber *isSymbolicLink;
        [[NSURL fileURLWithPath:_blocklist[i]] getResourceValue:&isSymbolicLink
                                                        forKey:NSURLIsSymbolicLinkKey
                                                          error:nil];
        if (isSymbolicLink)
        {
            NSTask *readlink = [[NSTask alloc] init];
//            [task setExecutableURL:[NSURL fileURLWithPath:@"/usr/bin/readlink"]];
            [readlink setLaunchPath:@"/usr/bin/readlink"];
            [readlink setArguments:@[@"-f", _blocklist[i]]];

            NSPipe *output = [NSPipe pipe];
            [readlink setStandardOutput:output];
            [readlink launch];
            [readlink waitUntilExit];
            NSData *dataOut = [[output fileHandleForReading] readDataToEndOfFile];
            NSString *target = [[NSString alloc] initWithData:dataOut
                                            encoding:NSUTF8StringEncoding];
            
            [_blocklist replaceObjectAtIndex:i withObject:target];
        }
        
        [_blocklist replaceObjectAtIndex:i withObject:[_blocklist[i] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
        
//        NSLog(@"%@", _blocklist[i]);
//        for (int j = 0; j < [_blocklist[i] length]; j++)
//            NSLog(@"%hu", [_blocklist[i] characterAtIndex:j]);
    }
}
- (void)appLaunch:(NSNotification *)note {
    NSString *appPath = note.userInfo[@"NSApplicationPath"];
    
//    NSLog(@"appPath = %@", appPath);
//    NSLog(@"appPath length = %lu", [appPath length]);
//    for (int i = 0; i < [appPath length]; i++)
//        NSLog(@"%hu", [appPath characterAtIndex:i]);
    
    if ([_blocklist containsObject:appPath])
    {
        NSString *exePath = [[NSBundle bundleWithPath:appPath] executablePath];
        
//        NSLog(@"exePath = %@", exePath);
        
        NSTask *pgrep = [[NSTask alloc] init];
//        [pgrep setExecutableURL:[NSURL fileURLWithPath:@"/usr/bin/pgrep"]];
        [pgrep setLaunchPath:@"/usr/bin/pgrep"];
        [pgrep setArguments:@[@"-f", exePath]];

        NSPipe *output = [NSPipe pipe];
        [pgrep setStandardOutput:output];
        [pgrep launch];
        [pgrep waitUntilExit];
        NSData *dataOut = [[output fileHandleForReading] readDataToEndOfFile];
        NSString *pid = [[[NSString alloc] initWithData:dataOut
                                        encoding:NSUTF8StringEncoding] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];

//        NSLog(@"pid = %@", pid);

        NSTask *kill = [[NSTask alloc] init];
//        [kill setExecutableURL:[NSURL fileURLWithPath:@"/bin/kill"]];
        [kill setLaunchPath:@"/bin/kill"];
        [kill setArguments:@[pid]];
        [kill launch];
    }
}
@end
