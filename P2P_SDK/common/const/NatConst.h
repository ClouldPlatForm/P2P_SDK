#ifndef __NATCONST_H__
#define __NATCONST_H__
namespace natconst
{
typedef int Socket;
const Socket invalid_socket = -1;
const int socket_error = -1;

#define LOCALPORT (6004)  //�󶨵ı��ض˿�Ĭ��ֵ
#define MAX_MSG_SIZE (1024)

#define RESEND_STUN_COUNT (7)
//#define STUN_MAX_MESSAGE_SIZE (2048)
#define RET_SUCCESS (0);
#define RET_FAILED  (-1);

// status�ṹ
typedef struct _status_
{
    char statusCode[12];
    char statusStr[24];
}Status;
/*****************************************************************************************
*̽�ⲽ���Ϊ5����                                                                       *
*����1��ʾUAC��User Agent Client�����͵�̽�������Ӧ��������sturn��������������IP��port��*
*����10��ʾUAC��User Agent Client�����͵�̽�������sturn��������������IP��port;          *
*sturn ����������Ӧ��ַ�Ǳ��ù�����ַIP��port��                                          *
*����11��ʾUAC����̽������Լ���NAT������ַ�Ͷ˿ڡ�                                      *
*����2��ʾUAC��User Agent Client�����͵�̽�������sturn�������ı��ù�����ַIP��port��    *
*����3��ʾUAC��User Agent Client�����͵�̽�������sturn��������������IP��port;           *
*sturn����������Ӧ������������IP�ͱ��ù�����port��                                       *
******************************************************************************************/
enum NatDetectionStep
{
	//��δ��ʼ̽��׶�
	NatDetectionStep_INIT = 0,
	//P��ʾ����A��ʾ����,sturn���������պ���Ӧ������ipport_ip��
	NatDetectionStep_PP_PP= 1,
    NatDetectionStep_PP_AA= 10,
	NatDetectionStep_AA_AA= 2,
	NatDetectionStep_AA_PA= 3,
	NatDetectionStep_HAIRPIN=11,
	NatDetectionStep_END = 100,
};

//NAT̽��״̬
enum NatDetectionState
{
  DETECTION_STATUS_FAILED =-1,
  DETECTION_STATUS_SUCCESS =0,
  DETECTION_STATUS_BEGIN =1,
  DETECTION_STATUS_GOING =2,
  DETECTION_STATUS_END =3,
};

/*****************************************************
*NAT���ͷ�Ϊ��                                       *
*1��UAC��NAT��                                       *
*2�����UAC��ͬһ��NAT�£�                           *
*3��ȫ׶��NAT;                                       *
*4��������NAT;                                       *
*5���˿�������NAT;                                   *
*6���˿ڹ̶������Գ��ͣ�                             *
*7���˿�����Գ��ͣ�                                 *
*8��IP����Գ��ͣ�                                   *
*9��IP�̶������Գ��ͣ�                               *
*10������SN��                                        *
*11���Գ��ͷ���ǽ                                    *
*12���Գ���IP�仯                                    *
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
	NATINFOISINVALID,//�ж�Nat��Ϣ�Ƿ����
	GETNATTYPE,//��ȡNat type��Ϣ
	DEVICEISOFFLINE,//�豸����
	GETREMOTEADDR,//��ȡ�Է���Nat��ַ
	STARTBEATHOLE,//��ʼ��
	GETBEATHOLERET,//��ȡP2PSvr������P2PResponse2��Ϣ��
	BEATHOLEFAILED,//��ʧ�ܣ�֪ͨp2pSvr
	UDPCONNECT,//��������
	HEARTBEAT,//��������
	KEEPALIVE,//ά������
	BEATHOLESUCCESS,//�򶴳ɹ�����
	BEATEND,//������Դ ������
};

};
#endif