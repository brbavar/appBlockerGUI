#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        CFStringRef commandLineArg = CFStringCreateWithCString(kCFAllocatorDefault, argv[1], kCFStringEncodingUTF8);
        CFURLRef fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, commandLineArg, kCFURLPOSIXPathStyle, false);
        CFRelease(commandLineArg);

        NSError *error = nil;
        NSURL *url = (__bridge NSURL *)fileURL;
        [url setResourceValue:nil forKey:NSURLQuarantinePropertiesKey error:&error];

        if (error) NSLog(@"Failed to remove quarantine attribute: %@", error);
    }
    return 0;
}