#import "BNRLogger.h"

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
    }
}
- (void)appLaunch:(NSNotification *)note {
    NSString *appPath = note.userInfo[@"NSApplicationPath"];
    if ([_blocklist containsObject:appPath])
    {
        NSString *exePath = [[NSBundle bundleWithPath:appPath] executablePath];
        
        NSTask *pgrep = [[NSTask alloc] init];
        [pgrep setLaunchPath:@"/usr/bin/pgrep"];
        [pgrep setArguments:@[@"-f", exePath]];

        NSPipe *output = [NSPipe pipe];
        [pgrep setStandardOutput:output];
        [pgrep launch];
        [pgrep waitUntilExit];
        NSData *dataOut = [[output fileHandleForReading] readDataToEndOfFile];
        NSString *pid = [[[NSString alloc] initWithData:dataOut
                                        encoding:NSUTF8StringEncoding] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];

        NSTask *kill = [[NSTask alloc] init];
        [kill setLaunchPath:@"/bin/kill"];
        [kill setArguments:@[pid]];
        [kill launch];
    }
}
@end
