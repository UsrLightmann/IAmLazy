#import <Foundation/Foundation.h>

@class IALGeneralManager;

@interface IALBackupManager : NSObject {
    NSArray<NSString *> *_controlFiles;
}
@property (nonatomic, retain) IALGeneralManager *generalManager;
-(void)makeBackupOfType:(NSInteger)type withFilter:(BOOL)filter;
@end
