//
//  WBFaceVerifyCustomerService.h
//  WBFaceV2
//
//  Created by tank on 25/10/2016.
//  Copyright © 2016 webank. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN
#define WBFaceV2SDKVersion @"2.2.8"

// SDK版本号
UIKIT_EXTERN NSString *const WBCloudFaceVerifySDKVersion;

// SDK界面风格等配置字段key值
UIKIT_EXTERN NSString *const WBFaceExternalIsShowSuccessPageKey;//BOOL
UIKIT_EXTERN NSString *const WBFaceExternalIsShowFailurePageKey;// BOOL
UIKIT_EXTERN NSString *const WBFaceExternalIsLightnessKey; // BOOL

// SDK自带比对源功能相关配置参数
UIKIT_EXTERN NSString *const WBFaceExternalIsUsingSourceImageKey; //BOOL
UIKIT_EXTERN NSString *const WBFaceExternalIsUsingHDImageKey; //BOOL (图像是否为高清图像/或者公安网文图像)
UIKIT_EXTERN NSString *const WBFaceExternalSourceImageKey; //UIImage(自带比对源图片大小不能超过2M)

// SDK在运行结束退出时候会发出通知. 具体的通知内容可以见delegate方法wbfaceVerifyCustomerServiceDidFinished:中的注释
UIKIT_EXTERN NSString *const WBFaceVerifyCustomerServiceDidFinishedNotification;

//登录的错误码
typedef NS_ENUM(NSInteger, WBFaceVerifyLogin) {
    WBFaceVerifyLogin_ERROR = -10000, // 登录请求返回报错
    WBFaceVerifyLogin_PARAMS_ERROR = -20000,// 登录请求传入参数有误
    WBFaceVerifyLogin_NORESPONSE_ERROR = -30000 // 登录请求网络错误
};

typedef NS_ENUM(NSInteger, WBFaceVerifySDKErrorCode) {
    WBFaceVerifySDKErrorCode_SUCESS = 0,// 身份认证成功
    WBFaceVerifySDKErrorCode_FAILURE = 1,// 身份认证出错
    WBFaceVerifySDKErrorCode_CANCELLED = 2,// 用户取消认证
};

typedef NS_ENUM(NSInteger,WBFaceVerifyType){
    WBFaceVerifyTypeMiddle,
    WBFaceVerifyTypeAdvanced,
};

typedef void (^WBFaceLoginSuccessBlock)(void);
//登录过程中 loginCode  细分错误码 ---  -2 表示登录时网络错误, -1 表示登录时webank后台返回出错, 0 表示登录通用默认码, 其他数字透传webank后台错误码(请参考后台文档)
typedef void (^WBFaceLoginFailureBlock)(WBFaceVerifyLogin errorCode, NSString *loginCode, NSString *message);

@interface WBFaceUserInfo : NSObject
@property (nonatomic,copy) NSString *orderNo; // 订单号（合作方上送，每次唯一）
@property (nonatomic,copy) NSString *name;    // 姓名
@property (nonatomic,copy) NSString *idType;  // 证件类型
@property (nonatomic,copy) NSString *idNo;    // 证件号码

/**
 判断UserInfo信息是否满足要求,内部只判断属性是否有nil
 */
-(BOOL)isPropertyRight;
@end

/**
   处理刷脸回调
 */
@class WBFaceVerifyCustomerService;
@protocol WBFaceVerifyCustomerServiceDelegate  <NSObject>

/**
 注意:
 1. 如果实现该代理方法(wbfaceVerifyServiceGetViewController:),则向WBFaceVerifyCustomerService SDK中通过代理传入一个ViewController,在sdk登录成功以后会在该viewController通过presentViewControllerxxxx方法拉起人脸认证页面.

 2. 如果没有实现该代理方法(wbfaceVerifyServiceGetViewController:), 那么SDK会创建一个UIWindow覆盖在当前界面,并在新创建的UIWindow界面进行人脸认证,并且可以通过实现 wbfaceVerifyServiceGetWindowLevel 代理方法,传入创建的UIWindow的windowLevel, 传入的windowLevel必须是1~999, 默认情况如果不实现 wbfaceVerifyServiceGetWindowLevel 方法,windowLevel = UIWindowLevelNormal + 1

 */
@optional
- (UIViewController *)wbfaceVerifyServiceGetViewController:(WBFaceVerifyCustomerService *)service;
@optional
- (NSUInteger)wbfaceVerifyServiceGetWindowLevel;

/**
 *  刷脸身份认证的回调方法,带结果签名sign的回调
 *
 *  @param errorCode iOS SDK定义的概要错误码 -- 重要信息: 身份认证成功与失败都在这里显示
 *  @param faceCode  细分错误码 ---  -2 表示网络错误, -1 表示webank后台返回出错, 0 表示通用默认码, 其他数字透传webank后台错误码(请参考后台文档)
 *  @param faceMsg   身份认证错误的相关提示信息
 *  @param sign      当前身份认证结果的签名信息
 
 
   注意!!!!!!!!!!!!!

   可以不实现该sdk方法,通过注册WBFaceVerifyCustomerServiceDidFinishedNotification这个通知,通过通知的n.userInfo同样可以拿到errorCode, faceCode, faceMsg, sign 以及 orderNo(之前传入sdk的订单号)等刷脸结果返回的信息.
 */
@optional
-(void)wbfaceVerifyCustomerServiceDidFinished:(WBFaceVerifySDKErrorCode)errorCode faceCode:(NSString *)faceCode faceMsg:(NSString *)faceMsg sign:(NSString *)sign;

/**
 *  SDK扩展字段
 *  WBFaceExternalIsShowSuccessPageKey BOOL 表示是否显示身份认证成功的结果页(默认情况显示 - YES)
 *  WBFaceExternalIsShowFailurePageKey BOOL 表示是否显示身份认证失败的结果页(默认情况显示 - YES)
 *  WBFaceExternalIsLightnessKey       BOOL 表示是否使用明亮主题的人脸验证界面风格(默认状态是暗黑主题页面风格)
 *
 *  在delegate实现中实现该方法
 -(NSDictionary *)wbfaceVerifyServiceGetExternalParams:(WBFaceVerifyCustomerService *)service{
     return @{
         WBFaceExternalIsShowFailurePageKey : @(YES),
         WBFaceExternalIsShowSuccessPageKey : @(YES),
         WBFaceExternalIsLightnessKey: @(YES),
         WBFaceExternalIsUsingSourceImageKey, @(NO),
    };
 }
 
 注意: 该方法和 -(void)startWBFaceServiceWithUserid:(NSString *)userid nonce:(NSString *)nonce sign:(NSString *)sign appid:(NSString *)appid userInfo:(WBFaceUserInfo *)userInfo apiVersion:(NSString *)apiVersion faceverifyType:(WBFaceVerifyType)type licence:(NSString *)licence success:(WBFaceLoginSuccessBlock)success failure:(WBFaceLoginFailureBlock)failure externalParams:(NSDictionary *)extenalParams; 方法中的最后一个参数extenalParams功能一致, 可以不用设置delegate, 直接使用该StartWBFaceServiceXXXX方法,在最后一个参数extenalParams传入配置参数.
 */
@optional
-(NSDictionary *)wbfaceVerifyServiceGetExternalParams:(WBFaceVerifyCustomerService *)service;

/**
 *********************************************************************
 ***************************** 废弃接口 *******************************
 *********************************************************************
- (UIViewController *)getViewController NS_DEPRECATED_IOS(2_0, 2_0, "该回调方法是前向兼容方法,请使用 wbfaceVerifyServiceGetViewController: 方法");
-(void)wbfaceVerifyCustomerServiceDidFinished:(WBFaceVerifySDKErrorCode)errorCode faceCode:(NSString *)faceCode faceMsg:(NSString *)faceMsg NS_DEPRECATED_IOS(2_0, 2_0, "该回调方法是前向兼容方法方法,请使用 wbfaceVerifyCustomerServiceDidFinished:faceCode:faceMsg:sign: 方法");
 */
@end

@interface WBFaceVerifyCustomerService : NSObject
@property (nullable,nonatomic,weak) id<WBFaceVerifyCustomerServiceDelegate> delegate;

/*
 * SDK全局单例,请使用此单例.不要使用init创建对象
 */
+ (instancetype)sharedInstance;

/**
 *  调用SDK服务核心方法 1
 *
 *  @param userid     userid 每个用户唯一的标识
 *  @param nonce      每次请求需要的一次性nonce
 *  @param sign       对nonce,userid等重要信息的签名数据
 *  @param appid      每个与webank合作分配的appid
 *  @param userInfo   用户信息对象,请参考前面定义的内容
 *  @param apiVersion 后台api接口版本号(不是SDK的版本号),默认请填写@"1.0.0"
 *  @param type       身份认证的类型- 简单模式,中级模式,高级模式
 *  @param licence    webank给合作方派发的licence
 *  @param success    调用sdk登录成功回调.请在该回调方法中关闭loading,并且在success block执行以后,sdk为拉起人脸认证页面
 *  @param failure    调用sdk登录失败时回调.请在该回调方法中关闭loading,处理错误逻辑.
 */
-(void)startWBFaceServiceWithUserid:(NSString *)userid
                              nonce:(NSString *)nonce
                               sign:(NSString *)sign
                              appid:(NSString *)appid
                           userInfo:(WBFaceUserInfo *)userInfo
                         apiVersion:(NSString *)apiVersion
                     faceverifyType:(WBFaceVerifyType)type
                            licence:(NSString *)licence
                            success:(WBFaceLoginSuccessBlock)success
                            failure:(WBFaceLoginFailureBlock)failure;


/**
 *  调用SDK服务核心方法2
 *
 *  @param userid     userid 每个用户唯一的标识
 *  @param nonce      每次请求需要的一次性nonce
 *  @param sign       对nonce,userid等重要信息的签名数据
 *  @param appid      每个与webank合作分配的appid
 *  @param userInfo   用户信息对象,请参考前面定义的内容
 *  @param apiVersion 后台api接口版本号(不是SDK的版本号),默认请填写@"1.0.0"
 *  @param type       身份认证的类型- 简单模式,中级模式,高级模式
 *  @param licence    webank给合作方派发的licence
 *  @param success    调用sdk登录成功回调.请在该回调方法中关闭loading,并且在success block执行以后,sdk为拉起人脸认证页面
 *  @param failure    调用sdk登录失败时回调.请在该回调方法中关闭loading,处理错误逻辑.
 *  @param extenalParams  
 *                  参数与"-(NSDictionary *)wbfaceVerifyServiceGetExternalParams:(WBFaceVerifyCustomerService *)service"接口提供的返回参数功能相同
                    传入字典如下:
                     @{
                         WBFaceExternalIsShowFailurePageKey : @(YES),
                         WBFaceExternalIsShowSuccessPageKey : @(YES),
                         WBFaceExternalIsLightnessKey: @(YES),
                         WBFaceExternalIsUsingSourceImageKey, @(NO),
                     }
 */
-(void)startWBFaceServiceWithUserid:(NSString *)userid
                              nonce:(NSString *)nonce
                               sign:(NSString *)sign
                              appid:(NSString *)appid
                           userInfo:(WBFaceUserInfo *)userInfo
                         apiVersion:(NSString *)apiVersion
                     faceverifyType:(WBFaceVerifyType)type
                            licence:(NSString *)licence
                            success:(WBFaceLoginSuccessBlock)success
                            failure:(WBFaceLoginFailureBlock)failure
                     externalParams:(NSDictionary * _Nullable)extenalParams;
@end
NS_ASSUME_NONNULL_END
