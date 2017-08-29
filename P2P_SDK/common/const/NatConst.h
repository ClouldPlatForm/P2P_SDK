#ifndef __NATCONST_H__
#define __NATCONST_H__
namespace natconst
{
typedef int Socket;
const Socket invalid_socket = -1;
const int socket_error = -1;

#define LOCALPORT (6004)  //绑定的本地端口默认值
#define MAX_MSG_SIZE (1024)

#define RESEND_STUN_COUNT (7)
//#define STUN_MAX_MESSAGE_SIZE (2048)
#define RET_SUCCESS (0);
#define RET_FAILED  (-1);

// status结构
typedef struct _status_
{
    char statusCode[12];
    char statusStr[24];
}Status;
/*****************************************************************************************
*探测步骤分为5步：                                                                       *
*类型1表示UAC【User Agent Client】发送的探测包和响应包都经过sturn服务器的主公网IP和port。*
*类型10表示UAC【User Agent Client】发送的探测包经过sturn服务器的主公网IP和port;          *
*sturn 服务器的响应地址是备用公网地址IP和port。                                          *
*类型11表示UAC发送探测包给自己的NAT公网地址和端口。                                      *
*类型2表示UAC【User Agent Client】发送的探测包经过sturn服务器的备用公网地址IP和port。    *
*类型3表示UAC【User Agent Client】发送的探测包经过sturn服务器的主公网IP和port;           *
*sturn服务器的响应经过主公网的IP和备用公网的port。                                       *
******************************************************************************************/
enum NatDetectionStep
{
	//还未开始探测阶段
	NatDetectionStep_INIT = 0,
	//P表示主，A表示备用,sturn服务器接收和响应的网卡ipport_ip，
	NatDetectionStep_PP_PP= 1,
    NatDetectionStep_PP_AA= 10,
	NatDetectionStep_AA_AA= 2,
	NatDetectionStep_AA_PA= 3,
	NatDetectionStep_HAIRPIN=11,
	NatDetectionStep_END = 100,
};

//NAT探测状态
enum NatDetectionState
{
  DETECTION_STATUS_FAILED =-1,
  DETECTION_STATUS_SUCCESS =0,
  DETECTION_STATUS_BEGIN =1,
  DETECTION_STATUS_GOING =2,
  DETECTION_STATUS_END =3,
};

/*****************************************************
*NAT类型分为：                                       *
*1、UAC无NAT；                                       *
*2、多个UAC在同一个NAT下；                           *
*3、全锥形NAT;                                       *
*4、限制型NAT;                                       *
*5、端口限制型NAT;                                   *
*6、端口固定步长对称型；                             *
*7、端口随机对称型；                                 *
*8、IP随机对称型；                                   *
*9、IP固定步长对称型；                               *
*10、其他SN型                                        *
*11、对称型防火墙                                    *
*12、对称型IP变化                                    *
******************************************************/
enum NatType
{
	NATTYPE_UNKNOW = -1,
	NATTYPE_OPEN_ACCESS=0,
	NATTYPE_FULL_CONE=1,
	NATTYPE_RESTRICTED_CONE=2,
	NATTYPE_PORT_RESTRICTED_CONE=3,
	NATTYPE_SYMMETRIC=4,
	NATTYPE_SYMMETRIC_FIREWALL=5,
	NATTYPE_SYMMETRIC_IPCHANGE=6,
	NATTYPE_OTHER_SN=7,
};

enum channelStatus
{
	STATUS_FAIL  = -1,
	STATUS_OK    = 0,
	STATUS_GOING = 1,
	STATUS_OTHER = 2,
};

enum channelFunc
{
  	BOTH,
	P2P,	
	RELAY,
};
enum hearBeatStatus
{
	UNKNOWN = -1,
	UNNORMAL  = 0,
	NORMAL = 1,
};

enum detectionType
{
	FULLMODE,
	PARTMODE,
};

enum beatHoleStatus
{
   BEATHOLESTATUS_SUCCESS,
   BEATHOLESTATUS_FAILED,
};

enum beatHoleStep
{
	NATINFOISINVALID,//判断Nat信息是否过期
	GETNATTYPE,//获取Nat type信息
	DEVICEISOFFLINE,//设备离线
	GETREMOTEADDR,//获取对方的Nat地址
	STARTBEATHOLE,//开始打洞
	GETBEATHOLERET,//获取P2PSvr反馈的P2PResponse2消息。
	BEATHOLEFAILED,//打洞失败，通知p2pSvr
	UDPCONNECT,//建立连接
	HEARTBEAT,//建立心跳
	KEEPALIVE,//维持心跳
	BEATHOLESUCCESS,//打洞成功结束
	BEATEND,//清理资源 结束打洞
};

};
#endif