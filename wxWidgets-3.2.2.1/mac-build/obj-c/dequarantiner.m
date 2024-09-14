#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        CFURLRef fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR(argv[1]), kCFURLPOSIXPathStyle, false);

        NSError *error = nil;
        NSURL *url = (__bridge NSURL *)fileURL;
        [url setResourceValue:nil forKey:NSURLQuarantinePropertiesKey error:&error];

        if (error) NSLog(@"Failed to remove quarantine attribute: %@", error);
    }
    return 0;
}