#import <Cocoa/Cocoa.h>

#ifndef BNRLogger_h
#define BNRLogger_h

@interface BNRLogger : NSObject
@property (nonatomic) NSMutableArray * blocklist;
- (void)setBlocklist:(NSString *)string;
- (void)appLaunch:(NSNotification *)note;
@end

#endif
