#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "cssl.h"
#include "unicode.h"
#include "socket_server.h"
#include "http_parser.h"

struct socket_server * ss;
cssl_t *serial;

struct http_parser_settings settings;

int id = 0;

int isURL = 0;
char buf[2048] = { 0 };
int buf_len = 0;

void
urldecode(char *p)
{
	register i = 0;
	while (*(p + i))
	{
		if ((*p = *(p + i)) == '%')
		{
			*p = *(p + i + 1) >= 'A' ? ((*(p + i + 1) & 0XDF) - 'A') + 10 : (*(p + i + 1) - '0');
			*p = (*p) * 16;
			*p +=*(p + i + 2) >= 'A' ? ((*(p + i + 2) & 0XDF) - 'A') + 10 : (*(p + i + 2) - '0');
			i += 2;
		}
		else if (*(p + i) == '+')
		{
			*p = ' ';
		}
		p++;
	}
	*p = '\0';
}

// 开始解析协议
int
begin_cb(http_parser *p)
{
	return 0;
}

// 解析到 URL 位置
int
url_cb(http_parser *parser, const char *p, size_t len)
{
	// 判断URL==/sms，则继续，否则不处理
	memset(buf, 0, 2048);
	if (memcmp(p, "/sms", len) == 0)
		isURL = 1;
	else
		isURL = 0;
	return 0;
}

// 头部解析到字段位置
int
field_cb(http_parser *parser, const char *p, size_t len)
{
	return 0;
}

// 头部解析到字段的值位置
int
value_cb(http_parser *parser, const char *p, size_t len)
{
	return 0;
}

// 头部解析结束
int
herders_cb(http_parser *p)
{
	return 0;
}

// 解析到 body 消息体位置
int
body_cb(http_parser *parser, const char *p, size_t len)
{
	memcpy(&buf[buf_len],p,len); // 将多次收到body数据合并到buf
	buf_len += len;
	return 0;
}

// 协议解析结束
int
end_cb(http_parser *p)
{
  // 这里接收到数据，如果是 HTTP 协议还需要取出URL的TEL和SMS的值。
  // 并将 SMS 的值转编码为 UniCode 的值。
  // 格式：
  //      SMS+TEL+#+DATA
  // SMS  = 短信标识
  // TEL  = 发送的手机号码
  // #    = 中文短信分割符
  // DATA = UniCode 大端编码
  //
  // 例子：SMS13950065000#4F60597D

	char* pos;
	char tel[32] = { 0 }; // 手机号字符串
	char sms[2048] = { 0 }; // 短信字符串

	if (isURL == 1)
	{
		pos = strtok(&buf, "=");
		if (strcmp(pos, "tel") == 0)
		{
			pos = strtok(NULL, "&");
			strncpy(tel, pos, strlen(pos)); // 获得手机号码
			//printf("tel = %s\n",tel);
		}
		pos = strtok(NULL, "=");
		if (strcmp(pos, "sms") == 0)
		{
			pos = strtok(NULL, "\r\n");
			strncpy(sms, pos, strlen(pos)); // 获得短信内容
			urldecode(&sms); // URL解码
			//printf("sms = %s\n",sms);
		}

		// 清空 buf
		memset(buf,0,2048);
		buf_len = 0;

		int n = 0;
		int unicode = 0;
		char comData[512] = { 0 }; // 清空数组 为0
		strcat(comData, "SMS"); // 复制 'SMS' 到字符串中
		strcat(comData, tel); // 复制 电话号码 到字符串中
		strcat(comData, "#"); // 复制 '#' 到字符串中(中文需要 # 隔开，英文用：隔开 )

		int len = strlen(sms); // 获得sms内容的长度
		sms[len] = 0x00;
		sms[len+1] = 0x00;
		sms[len+2] = 0x00;
		sms[len+3] = 0x00;
		int alen = strlen(comData); // 获得发送内容的长度


		char StrUni[8]={0};
		int UniLen = 0;
		int i = 0;
		int j = 0;
		char Str0[4]={0};
		for (; i <= len; )
		{
			n = UTF8toUnicode(&sms[j], &unicode); // 将 UTF8 转成 Unicode 编码
			memcpy(&StrUni,&unicode,4);
			UniLen = strlen(StrUni); // 计算转换后的字符串长度
			unicode = htonl(unicode); // 大小端转换

			memcpy(&StrUni,&unicode,4);
			//printf("unicode = %08X\n", unicode);

			if(n==1) // 返回1表示为ASCII码，需要在前面加0x00。
			{
				strncat((char*) &comData[alen], (char*) &Str0[0], 1);
				alen += 1;
			}

			strncat((char*) &comData[alen], (char*) &StrUni[4-UniLen], UniLen);
			alen += UniLen;
			i = i + (4-UniLen);

			j = j + n;
		} // for

		// 发送信息到串口
		//cssl_putstring(serial, comData); // 发字符串
		cssl_putdata(serial, comData,alen); // 发数据 
printf("alen=%d\n",alen);
	} // if


	char *tmp = malloc(64); // 分配 tmp 的内存空间
	memset(tmp, 0, 64); // 清空内存空间
	strcpy(tmp, "HTTP/1.1 200 ok\r\nContent-Length: 2\r\n\r\nOK"); // 复制数据到 tmp
	socket_server_send(ss, id, tmp, strlen(tmp));
	return 0;
}

int
status_cb(http_parser *parser, const char *p, size_t len)
{
	return 0;
}

/* 接收到网络数据 */
void
OnReceive(struct socket_message result)
{
	// 开始解析
	http_parser_execute((struct http_parser *)result.opaque, &settings, result.data, result.ud);
}

/* 接收到串口数据 */
static void
callback(int id, uint8_t *buf, int length)
{
  // 这里不用处理串口返回来的数据
  // 串口返回数据
  // SMS SEND OK	发送短信成功
  // SMS SEND FAIL	发生短信失败

  //char *tmp = malloc(256); // 分配 tmp 的内存空间
  //memset(tmp, 0, 256); // 清空内存空间
  //strcpy(tmp, buf); // 复制数据到 tmp
  //printf("send [id=2] len=%d data=%s\n", length, tmp);
  //socket_server_send(ss, id, tmp, length); // 临时解决使用id
}

int sigign() {
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);
	return 0;
}

int
main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Copyright (c) 2014, Dali Wang<wangdali@qq.com>.\n");
		printf("Used: sms ip port serial\n");
		printf("./sms 127.0.0.1 8888 /dev/ttyS0");
		return 0;
	}
  
	sigign(); // 信号处理
	
  
	// 注册返回函数
	settings.on_message_begin = begin_cb;
	settings.on_url = url_cb;
	settings.on_header_field = field_cb;
	settings.on_header_value = value_cb;
	settings.on_headers_complete = herders_cb;
	settings.on_body = body_cb;
	settings.on_message_complete = end_cb;
	settings.on_status = status_cb;

	cssl_start(); // 开启CSSL库

	//"/dev/ttyS1"
	serial = cssl_open(argv[3], callback, 0, 9600, 8, 0, 1); // 打开串口

	// 打开失败
	if (!serial)
	{
		printf("Open COM Fail = %s\n", cssl_geterrormsg()); // 获得错误信息
		return -1;
	}

	ss = socket_server_create(); // 创建 socket_server
	int listen_id = socket_server_listen(ss, 100, argv[1], atoi(argv[2]), 32); // 监听 socket
	socket_server_start(ss, 200, listen_id); // 纳入事件管理

	struct socket_message result; // 声明一个soket消息的结构
	for (;;)
	{
		int type = socket_server_poll(ss, &result, NULL); // 事件循环

		switch (type)
		{
			case SOCKET_EXIT:
				printf("exit(%lu) [id=%d]\n", result.opaque, result.id);
				goto EXIT_LOOP;
			case SOCKET_DATA:
				//printf("data(%lu) [id=%d] %s\n", result.opaque, result.id, result.data);
				OnReceive(result); // 收到数据，调用函数处理
				break;
			case SOCKET_CLOSE:
				printf("close(%lu) [id=%d]\n", result.opaque, result.id);
				free((void*)result.opaque);
				break;
			case SOCKET_OPEN:
				printf("open(%lu) [id=%d] %s\n", result.opaque, result.id, result.data);
				break;
			case SOCKET_ERROR:
				printf("error(%lu) [id=%d]\n", result.opaque, result.id);
				break;
			case SOCKET_ACCEPT:
			{
				printf("accept(%lu) [id=%d %s] from [%d]\n", result.opaque, result.ud, result.data, result.id);

				// 初始化 HTTP 结构
				struct http_parser *parser;
				parser = (http_parser *) malloc(sizeof(http_parser));
				http_parser_init(parser, HTTP_REQUEST);

				socket_server_start(ss, (uintptr_t)parser, result.ud); // 纳入事件管理

				id = result.ud; // 临时解决办法，应该使用 parser->data来传输 。
			}
				break;
		} // switch
	} // for
	EXIT_LOOP: socket_server_release(ss); // 关闭 socket_server
	return 0;
}

